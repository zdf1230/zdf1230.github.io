---
layout: post
title: "Who Watches the Agent? Policy Verdicts on Every Model and Tool Call"
tags: [AI, agents, security, policy]
categories: ai
date: 2026-10-06 15:50:00 -0700
---

An AI agent is a loop: think, call a tool, observe the result, repeat. Every turn of that loop is a place where something can go wrong — a prompt injection in a tool result, a runaway `rm -rf`, PII leaking into a log. The industry's answer is converging on a simple idea borrowed from classic security architecture: **put a policy checkpoint on every invocation, and make it return a verdict** — allow, block, redact, or escalate — before the action happens.

This post maps the product landscape, the integration patterns, and the software architecture behind per-invocation agent policy enforcement, based on research done this week (October 2026).

## Two species of products

Roughly everything in this space falls into two species:

**Species A — AI security platforms with semantic/ML classifiers.** These screen prompts, outputs, and tool calls with trained models and return verdicts like allow / block / redact / escalate. Examples:

- **Lakera Guard** (acquired by Check Point, Nov 2025) — screening API returning a `flagged` boolean plus per-category risk scores (prompt injection, jailbreak, PII, moderation). The caller decides what to do with the flag.
- **Zenity Runtime Boundaries** — evaluates *every AI action* in real time → proceed / block / terminate, with a stateful engine that tracks multi-step attack chains rather than single prompts.
- **Noma Security** — policy enforced on every agent action *before execution*, with graduated responses: steer → mask → block → terminate.
- **TrojAI Defend** — runtime firewall plus MCP-specific defense (tool-definition change detection, rogue connection blocking).
- **HiddenLayer Runtime Security** — inline verdicts (detected / redacted / blocked), plus native hooks into Claude Code, Cursor, and Copilot.
- **Aim Security** (acquired by Cato Networks), **CalypsoAI** (acquired by F5), **Prompt Security** (acquired by SentinelOne), **Protect AI** (acquired by Palo Alto Networks → Prisma AIRS), **Robust Intelligence** (acquired by Cisco), **Harmonic**, **Mindgard**, **Cranium** — same genus, different hunting grounds.

**Species B — authorization infrastructure with deterministic policy-as-code.** These return allow/deny from explicit rules, not ML judgment:

- **OPA / Styra** — the canonical Policy Decision Point. Rego policies, JSON in → decision out. Deploy as a sidecar or a centralized service.
- **AWS Cedar / Amazon Verified Permissions** — `permit`/`forbid` policies (forbid wins, default deny); `IsAuthorized` → ALLOW/DENY. AWS's endorsed agent pattern checks agent identity plus delegated `onBehalfOf` user on every call.
- **Permit.io** — fine-grained authz (OPA + OPAL + Cedar) with an MCP Gateway that authorizes *every agent tool call*, tracks the human→agent delegation chain, and enforces zero standing permissions.
- **Cerbos** — self-hosted stateless PDP, YAML policies, per-action (not per-session) checks; gates MCP tool calls directly and even enforces through Claude Code hooks.
- **Topaz** (OSS, ex-Aserto) — OPA engine plus Zanzibar-style directory as a sidecar.

And don't forget the **framework-native** controls: Claude Code's permission system (allow/ask/deny per tool) plus `PreToolUse` hooks that return allow/deny/ask/defer and can even rewrite arguments; LangChain/LangGraph's `wrap_model_call` / `wrap_tool_call` middleware — the canonical in-loop interception points.

The market consolidated hard in 2025–26: most of Species A got acquired by the big security platforms (Palo Alto, Cisco, Check Point, F5, Cato, SentinelOne, CrowdStrike, Fortinet). What survived independently — TrojAI, Zenity, Noma, HiddenLayer, Cranium — is increasingly *agent-specific* rather than generic LLM firewalls.

## Five integration patterns

How do these products actually sit in the request path?

**1. Reverse-proxy / AI gateway.** Sits between your app and the model provider (or between agent and MCP servers). Intercepts every model call; some gateways also intercept MCP tool calls. Examples: Portkey (now Prisma AIRS AI Gateway), Cloudflare AI Gateway, Fastly. Cost: an extra network hop and a new single point of failure — so it must be HA. Benefit: provider-independent, no code changes beyond a base-URL swap.

**2. SDK wrapper / middleware.** Wraps the agent loop in-process. Example: Guardrails AI's `Guard` (`on_fail` = exception / fix / filter / refrain / reask / noop), or LangChain middleware. Lowest overhead for deterministic checks; inherits your app's availability.

**3. Framework hooks.** Pre-tool-call hooks inside the agent framework — e.g. Claude Code's `PreToolUse`, which receives tool name + arguments as JSON and returns a verdict *before* side effects happen. No traffic rerouting; works per-agent. Third parties (Cerbos, HiddenLayer) enforce through these hooks.

**4. Sidecar.** A policy engine running alongside the workload (same host/pod); the app calls it over localhost per invocation. Example: OPA as sidecar — near-zero latency, survives network partitions. Styra's docs frame the trade-off explicitly: sidecar = low latency + fault tolerance vs. centralized = shared data + higher latency.

**5. Centralized PDP API.** The enforcement point calls a remote decision service per invocation: AWS Verified Permissions' `IsAuthorized`, Lakera's Guard API, a remote Cerbos/Permit PDP. Needs batching (`BatchIsAuthorized`), caching, and an explicit degraded-mode policy.

## The software architecture: PEP / PDP / PIP / PAP

This is classic XACML/ABAC terminology, mapped onto agents:

- **PEP (Policy Enforcement Point)** — whatever can actually *stop* the action: the AI gateway, the framework hook, the MCP gateway, the tool server itself. Multiple PEPs are normal; that's defense in depth.
- **PDP (Policy Decision Point)** — OPA, Cedar/AVP, Cerbos, Permit, Lakera, NeMo Guardrails, Zenity. Returns the verdict.
- **PIP (Policy Information Point)** — identity providers, device posture, agent registries, delegation chains, data classification labels, threat intel. The *context* the decision needs.
- **PAP (Policy Administration Point)** — where policies are authored, tested, versioned, distributed: Styra, Cerbos Hub, Permit control plane, GitOps repos.

A few architecture decisions that matter more than product choice:

- **Sync pre-invocation checks vs async audit.** Only synchronous checks *prevent* harm — required for tool calls with side effects, PII egress, budget caps. Async logging is forensics, not controls. As one practitioner put it: "logs are forensics, not controls" — an audit trail never stopped an exfiltration.
- **Fail closed on hard boundaries; fail open only for advisory checks** — and log loudly whenever the fail-open path is taken. Design the degraded mode explicitly: an attacker who can bypass your guardrails by overloading the safety model has a cheap exploit.
- **Policy authoring**: Rego (most expressive, steepest curve), Cedar (authorization-focused, formally verifiable), YAML (Cerbos, GitOps-friendly), Colang (NeMo Guardrails dialog flows), or natural-language intent policies (easiest for security teams, least deterministic — pair with deterministic checks underneath).
- **Tool-argument validation** in layers: schema validation → allowlists → parameter constraints (path prefixes, tenant scoping) → delegation ceilings (the agent can't exceed its human's permissions) → intent-drift checks (does this action match the delegated task?).
- **Observability**: log every verdict with a decision ID — principal, action + arguments, policy, verdict, latency. Blocked requests are the highest-value signal (someone is probing). And watch out for streaming: you can't retract already-delivered tokens, so buffer sensitive output until mandatory checks pass.

## What's the best pattern?

The practitioner consensus as of 2026 is layered, not single-product:

1. **AI gateway as the outer PEP + framework hooks as the inner PEP.** The gateway gives provider-independent enforcement on every model call (auth, budgets, PII redaction, tracing). The hooks give per-tool-call veto *inside* the agent loop, where the gateway can't see tool arguments or agent intent. Do both.
2. **Deterministic policy PDP on the fast path; semantic classifiers as a second layer.** Sub-millisecond, explainable, auditable allow/deny for authorization; ML classifiers for the fuzzy stuff (injection, DLP) with explicit latency budgets. Never rely on a single classifier — one independent 2026 test found a 35% adversarial miss rate on a leading guard API.
3. **Co-locate the PDP with the PEP** (sidecar or in-VPC); keep authoring centralized (GitOps).
4. **Authorize per action, not per session** — agent identity + delegating-user ceiling + zero standing permissions + short-lived credentials. Least privilege is the control that survives prompt injection: even a hijacked agent can't do what it was never authorized to do.
5. **Policy as code, observe-then-enforce rollout**, and a decision ID on every verdict.

Concretely, a 2026 reference stack looks like: AI gateway (Portkey/Prisma AIRS, Cloudflare, or Fastly) → policy PDP (OPA/Cerbos/Permit sidecar, or AVP for AWS shops) called from both the gateway and framework hooks (Claude Code `PreToolUse`, LangGraph `wrap_tool_call`) → a screening API (Lakera Guard or equivalent) for prompt/output semantics → a platform overlay (TrojAI / Zenity / Noma / HiddenLayer) for posture, red-teaming, and runtime behavior monitoring.

What to avoid: system-prompt-only safety, a single semantic classifier as the whole story, audit-only "governance" with no enforcement point, unbuffered streaming, and fail-open defaults on hard boundaries.

## Good reads

- [OWASP Top 10 for Agentic Applications 2026](https://genai.owasp.org/resource/owasp-top-10-for-agentic-applications-for-2026/) — OWASP. The risk taxonomy everything above maps to.
- [Securing Enterprise AI with AI Gateways and Guardrails](https://portkey.ai/blog/securing-enterprise-ai-with-ai-gateway-and-guardrails/) — Portkey blog. Log/flag/block modes; why centralize in the gateway.
- [Enforcing AI Agent Governance at the Infrastructure Layer](https://portkey.ai/blog/ai-agent-governance/) — Portkey blog. The "don't govern inside the framework" argument.
- [Why AI Agents Choose Permit.io for Authorization](https://www.permit.io/blog/why-ai-agents-choose-permitio-for-authorization) — Permit.io. Zero standing permissions, per-tool-call auth, intent checks.
- [Best AI Agent Security and Governance Tools for 2026](https://www.cerbos.dev/blog/best-ai-agent-security-and-governance-tools) — Cerbos. Per-action authorization, MCP gating, observe-then-enforce.
- [How to Deploy OPA](https://docs.styra.com/opa/deploy) — Styra docs. The canonical sidecar-vs-centralized PDP trade-off.
- [Empower AI agents with user context using Amazon Cognito](https://aws.amazon.com/blogs/security/empower-ai-agents-with-user-context-using-amazon-cognito/) — AWS Security Blog. Cedar `onBehalfOf` delegation pattern.
- [LLM Guardrails: Architecture, Evaluation, and Go Guide](https://qubittool.com/blog/llm-guardrails-engineering-guide) — qubittool (independent). Fail-open/closed, streaming bypass, evaluation methodology.
- [Should Agent Guardrails Live in the Model or the Infrastructure?](https://proxifly.dev/blog/should-agent-guardrails-live-in-the-model-or-the-infrastructure) — proxifly (independent). Model-as-planner vs infrastructure-as-enforcer.
- [AI Guardrails: The Missing Safety Layer in Production AI Systems](https://medium.com/@mytestmail415/ai-guardrails-the-missing-safety-layer-in-production-ai-systems-1f0ae096f99b) — independent practitioner. Fail-closed principle, config-as-code.
