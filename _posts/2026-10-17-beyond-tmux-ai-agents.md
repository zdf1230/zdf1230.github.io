---
layout: post
title: "Beyond tmux: Managing a Fleet of AI Coding Agents"
tags: [AI, Claude Code, Codex, tmux]
categories: ai
date: 2026-10-17 12:00:00 -0700
---

I run my coding agents in tmux. It's the obvious setup: one pane per agent, sessions that survive a closed laptop, zero dependencies on any server. It works — until it doesn't. The question I kept asking: is there actually something better, or is tmux the ceiling? Here's what I found after digging through practitioner writeups from the last few months.

## The bottleneck moved

Adoption numbers are absurd now — JetBrains' 2026 survey has 90% of developers using AI coding agents weekly, 68% daily. But only ~10% of work code is fully AI-generated, and the viral complaint of September was that mandated AI tooling turned engineers into "quality control inspectors on an assembly line," spending 12–13 hours a day reviewing AI PRs. The job is now spec → direct → verify → integrate. Writing was never the bottleneck; **review is**. And that's exactly where raw tmux gives you nothing.

## What tmux still does best

Be fair to the tool. tmux's session durability is unmatched — even cmux, the purpose-built macOS terminal for agent work, does *not* restore live processes on restart (layout and scrollback yes, running sessions gone). tmux works on any SSH server with zero config, and every CLI agent — Claude Code, Codex, Gemini CLI, Aider — runs in it unchanged. For 1–3 agents you actively watch, raw tmux is still perfect.

Where it breaks, in order of pain:

1. **No identity on notifications.** Five panes, five identical "waiting for your input" alerts. You alt-tab hunting the right one.
2. **No coordination.** Two agents grab the same task, edit overlapping files, duplicate work. Files are the only shared state.
3. **Manual worktree bookkeeping.** Creating worktrees, copying env files, merging, pruning — roughly 10 minutes of setup per parallel batch.
4. **Notifications break on remote boxes.** tmux swallows desktop-notification escape sequences unless `set -g allow-passthrough on` is set, and laptop sleep kills everything.
5. **No review surface.** The new bottleneck, and tmux has zero opinion about it.

## The three upgrades that actually matter

Every purpose-built tool I found is just packaging some combination of these three primitives. Understand the primitives and you can decide whether you need the tool at all.

### 1. Isolation: one worktree per agent

The universal primitive. `git worktree add` per agent — they share git history but not working files, and conflicts get deferred to merge time:

```bash
git worktree add ../myapp-claude -b claude-refactor
git worktree add ../myapp-codex  -b codex-tests
tmux new -s claude -c ../myapp-claude
tmux new -s codex  -c ../myapp-codex
```

Claude Code now ships this natively (`claude --worktree`, with subagent-level `isolation: "worktree"` definitions and auto-cleanup). The pattern worth stealing is the `/task-master` slash command: one command fans N task IDs into N isolated tmux sessions — worktree creation, branch, session spawn, all in one step.

### 2. Attention routing: know *which* agent needs you

The key insight from practitioners: separate two events. "Blocked and waiting on you" (permission prompt, clarifying question — every second is wasted wall time) vs. "finished the turn" (a diff waiting for review — nothing wasted while you finish your current task). Wire both to the same loud channel and you'll mute it.

- **Claude Code:** `Notification` hooks with matchers on `permission_prompt`, `idle_prompt`, `agent_completed` route blocked vs. done to different channels. The `Stop` hook fires once per turn with the last assistant message — the right place for "here's what landed" alerts. Phone push is one curl in a hook: `curl -d "agent needs you" ntfy.sh/your-topic`.
- **Codex CLI:** a top-level `notify` key in `~/.codex/config.toml` runs your script on turn-complete. Known gap as of this summer: approval requests reach the TUI's desktop notification but *not* your external script (openai/codex#19921, still open) — so script your "finished" alerting and lean on the built-in TUI notification for "needs approval."
- **tmux specifics:** without `set -g allow-passthrough on` in `~/.tmux.conf`, tmux swallows the notification escape sequences entirely.

### 3. A review surface

The most underrated upgrade. Options:

- **The Stop hook as a "done" gate:** block the agent's stop (exit 2) while the build is still red — run `tsc --noEmit`, print errors to stderr, exit 2. Guard with `stop_hook_active` to avoid infinite loops. Pair with a `PostToolUse` per-edit check. Keep checks fast: typecheck and lint, not the full suite.
- **Codex `/review`:** reports findings against the base branch, uncommitted changes, or a commit — without touching the working tree.
- **Second-agent review:** Offrun's pattern — a different agent reviews the diff before you accept it.
- **GUI flows:** Conductor and Cursor 2.0 bundle diff review, PR creation, and CI checks into the agent workspace itself.

## The tools, ordered by distance from tmux

**Closest to home (stay in the terminal):**

- **[cmux](https://ddewhurst.com/blog/cmux-purpose-built-terminal-for-parallel-agent-workflow/)** — native macOS terminal built for parallel agent work, ~5k stars since January. Pane-level notifications (blue ring, sidebar badge, jump-to shortcut), a workspace sidebar showing git branch and PR state, and a Unix-socket JSON-RPC API so an orchestrator agent can spawn panes and read their screens. Philosophy: "a primitive, not a solution." Rough edges: macOS only, AGPL-3.0, and no live-process restore — tmux still wins on durability.
- **[Claude Squad](https://dev.to/stravukarl/best-tools-for-managing-parallel-ai-coding-agents-in-2026-14l8)** — terminal TUI running tmux + git worktrees under the hood, one task per isolated workspace, mixing Claude Code, Codex, OpenCode, Aider in slots. The closest thing to "tmux with structure" for people who won't leave the terminal.
- **tmux-ide** — declarative `ide.yml` defining layouts, commands, and agent roles; `tmux-ide restart` rebuilds everything from config.

**Dashboards and control planes (sessions outlive the laptop):**

- **[AMUX](https://github.com/mixpeek/amux)** — open-source control plane, single Rust binary, MIT. Parallel workers with an atomic compare-and-swap task kanban (two agents never grab the same card), inter-worker messaging, phone steering, cron schedules, model switching on running workers, and a self-healing watchdog that auto-compacts below 20% context and restarts crashed sessions.
- **[AQ](https://aq.dev/guides/get-notified-when-claude-code-finishes/)** — real CLIs in persistent tmux sessions on a team VM, streamed to the browser. Teammates can watch or take over the same live session. Their honest positioning: "if you run one or two sessions on your own machine, a notification hook and a phone push are the whole answer — AQ earns its place when sessions must outlive laptops."
- **[Offrun](https://offrun.dev/)** — free Mac app; each agent in its own worktree on your own accounts, a second agent can review a diff before you accept, and your code never touches their servers.
- **AgentsRoom** — multi-agent dashboard with live agent timeline and desktop/mobile push.

**GUI worktree managers:**

- **[Conductor](https://ai-deck.app/blog/conductor-vs-superset-vs-cmux/)** (Melty Labs) — "just isolate everything for me." One git worktree per agent, built-in diff review, PR creation, AI review, CI checks. Free for single-user; public adopters include Linear, Vercel, Notion, Ramp, Spotify. Landmine: re-clone-per-worktree adds latency on heavy iteration.

**Vendor-native (the simplest upgrade path):**

- **Claude Code Agent Teams** (experimental) — one session becomes a team lead spawning teammates in separate tmux panes, each owning specific files. Model-mixing is the biggest cost lever: Opus lead, Sonnet teammates, Haiku subagents.
- **Codex desktop app** — multi-thread with worktree isolation and visual diff review; CLI remains the choice for `codex exec` headless runs and CI.
- **Cursor 2.0** — up to 8 parallel agents with built-in worktree management. Caveat from practitioners: worktrees prevent file conflicts but *not* logical conflicts — two agents with incompatible assumptions still need human verification.

## Efficiency: what actually moves the needle

- **Model routing is the biggest cost lever.** Classify work, dispatch cheap classes to cheap models — Warp's benchmark-driven routing cut costs 63% on certain task types without quality loss.
- **Context budgets, not more context.** "Your agent does not need more context. It needs a budget." Subagents get independent context windows — delegation is the workaround for the hard limit.
- **Hooks for non-negotiables, instructions for the rest.** Anything that must happen 100% of the time (lint after edit, block `rm -rf`) belongs in a hook, not in a prompt the agent might skim.
- **Plan first.** `/plan` in Codex, plan mode in Claude Code — approve the plan before implementation for anything spanning multiple files.
- **Know when not to use agents:** architecture decisions, security-sensitive code (auth, payments — careful human review every time), large ambiguous refactors, and tightly-coupled tasks you shouldn't parallelize at all.

## The honest verdict

tmux is still the best *session multiplexer*. It is not an *agent manager*. The upgrade is never "a fancier tmux" — it's **worktrees + notifications + a review surface**, hand-rolled with hooks or bought via a tool. If you want to stay in the terminal, Claude Squad or cmux add structure without leaving it; if you want the bookkeeping done for you, Conductor; if sessions must outlive your laptop, AMUX or AQ. Pick your position on the GUI⇔CLI × isolation⇔direct map first, then check pricing and licensing. Everything else is configuration.

### Good reads

- [cmux: purpose-built terminal for parallel agent workflow](https://ddewhurst.com/blog/cmux-purpose-built-terminal-for-parallel-agent-workflow/) — the best single essay on what tmux lacks for agent work (notifications with identity, the durability trade-off).
- [Get notified when Claude Code finishes](https://aq.dev/guides/get-notified-when-claude-code-finishes/) — the definitive guide to the notification stack: hooks, tmux passthrough, ntfy phone push, Codex's notify gap.
- [Building multi-agent systems with Claude Code: coordination patterns that work in production](https://dev.to/thebrierfox/building-multi-agent-systems-with-claude-code-coordination-patterns-that-work-in-production-1m75) — inbox/outbox file coordination, the inspectable-resumable argument.
- [Run 4 Claude Code tasks simultaneously and merge with git](https://dev.to/subprime2010/claude-code-parallel-agents-run-4-tasks-simultaneously-and-merge-with-git-3471) — the canonical raw-tmux multi-agent recipe, including the honest 10-minute setup overhead.
- [Codex CLI real-world coding workflow](https://ofox.ai/blog/codex-cli-real-world-coding-workflow/) — a day-in-the-life with worktrees, `/plan`, `/review`, and profiles; "the second week you realize it's about the four files you wrote on day one."
- [Conductor vs Superset vs cmux](https://ai-deck.app/blog/conductor-vs-superset-vs-cmux/) — the two-axis decision map (GUI⇔CLI × isolation⇔direct-work).
