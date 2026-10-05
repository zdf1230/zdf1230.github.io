---
layout: post
title: "Jev: The AI That Can't Write a Sentence — and the Clone Wars It Started"
tags: [AI, Jev, decision models]
categories: ai
date: 2026-10-05 01:00:00 -0700
---

The most talked-about AI model of September can't write a single sentence. **Jev**, released September 15 by the startup **TypeSafe AI**, doesn't generate text at all. Instead, it reads your input and returns a *decision*: yes or no, a pick from your list, a score — each with a calibrated probability attached.

TypeSafe calls this new category **"System One" models** (after Kahneman's *Thinking, Fast and Slow*; "Jev" itself is named after economist William Stanley Jevons). Independent commentators like Simon Willison suggest the clearer name is simply **decision models**.

## How it works

A normal LLM takes your prompt and generates tokens one by one; you parse whatever comes out and hope the JSON is valid. Jev flips the interface:

1. You send a **state** (text, JSON, or an array of strings).
2. You define **typed questions** in advance.
3. Jev evaluates all questions **in parallel, in a single pass**, and returns typed answers with probability distributions — nothing to parse, schema-conformance guaranteed by construction.

Three primitives: **Choice** (pick from up to 255 options), **Score** (2–10 ordered levels), and **Noul** (yes/no probability 0–1; "Noul" is short for Bernoulli). The training method is called **RLCD — Reinforcement Learning for Calibrated Decisions** — which replaces RLHF with training for calibrated uncertainty, so stated confidence actually tracks real accuracy.

The pitch, in TypeSafe's words: "a frontier-intelligence function call — unstructured state in, typed probabilistic decisions out." Vendor-claimed specs: $0.042 per million input tokens with **output tokens free**, 70–500 ms latency, 64K context. Note the "can't hallucinate" claim means *format-safe*, not *error-free*: independent tests found wrong answers delivered at 80%+ confidence on math problems.

The company: co-founder/CEO **Diogo Almeida**, an ex-OpenAI researcher (RLHF/InstructGPT/ChatGPT) who left in 2024, emerged from stealth with the launch, backed by a **$40M seed led by DCVC**. The Information later reported talks of a $1B+ raise at a $10B+ valuation — unconfirmed, and Almeida declined to comment.

## Real-world use cases

The strongest third-party evidence comes from **Vercel**: Jev was the fastest-adopted model in AI Gateway history — roughly 13% of paid teams within 24 hours, 2× the GPT-5.6 family. One caveat: that was measured during a free-access window (free through Sept 25), so it's trial velocity, not paid conversion. Cloudflare, LangChain, and Langfuse integrated within three days of launch.

What people actually use it for:

- **Agent infrastructure at Vercel**: tool/subagent selection, continue/retry/stop decisions, urgency and risk scoring, output verification, guardrails — and escalating uncertain cases to humans.
- **Content moderation**: in an independent 900-example benchmark (AI/ML API, Sept 2026), Jev had the **best F1 of six models** on offensive-content moderation, beating Claude Opus 5.5 and GPT-6 Sol — while costing 84–150× less than Opus 5.5.
- **The filter pattern**: as a first-pass filter in front of Opus 5.5, Jev matched Opus's accuracy on classification tasks at 38–45% of the cost. This looks like the honest sweet spot: *fast, cheap, well-calibrated triage in front of an LLM* — not an LLM replacement.
- **Routing and scoring**: ticket routing, lead scoring, review routing, fraud signals.

Be skeptical of the headline numbers, though. Almeida told the WSJ that ~25% of the Fortune 500 "use" Jev with traffic around a trillion tokens a day — but **no companies are named**, "in use" is undefined, and nothing about revenue or retention is disclosed. Treat it as a vendor claim until proven otherwise. Also worth knowing: prompt injection can sway Jev's verdicts, so wiring it into agent decision loops is running ahead of governance (VentureBeat).

## The clone wars

Here's the remarkable part: within **16 days** of Jev's launch, the copycats arrived in force.

| Date | Clone | Backstory |
| ---- | ----- | --------- |
| Sept 29 | **OpenAI Decisions API** (DevDay) | Gives Luna a predefined set of options to choose between. Limited preview, no public docs or pricing at launch. Almeida joked on X about the "clone wars"; TypeSafe declined to comment. |
| Sept 30 | **Databricks `ai_decide`** | Native AI Function via SQL + REST, Jev-API compatible. For model routing, review tagging, agent eval. |
| Oct 1 | **Cloudflare Clef / Clef-flash** | The most aggressive: the first models Cloudflare ever trained itself (27B + 9B, Qwen-based), **Apache 2.0 open weights**, Jev-API compatible — plus a vision encoder and 64K context Jev lacks. Cloudflare claims it beats Jev on 3 of 4 of TypeSafe's own evals with ~13× lower latency. All vendor-run and unreproduced; one independent spot-check found the opposite on nuanced triage. |
| Oct 1 | **AWS Strands Decider 2B** | The best story: it started as distinguished engineer **Marc Brooker's side project** after he saw Jev — he built a homebrew version (~2B params, Qwen3.5 + LoRA + a tiny "pointer head"), briefly topped JevBench for its size class, and AWS productized it in about two weeks. Fully open source. |

On top of that, community open weights appeared within ~10 days: Laya, Kev, Von, Tev1-4B, OpenJev, GLiNER2.5-Decide — trailing Jev zero-shot but competitive after fine-tuning.

We've seen this movie before, just never this fast: ChatGPT (Nov 2022) → Google rushed Bard to testers ~2.5 months later; DeepSeek R1 (Jan 2025) → distilled clones within days and a $589B single-day Nvidia wipeout. Jev compresses the same pattern into *days*.

## Good reads

- [Introducing System One models and Jev](https://typesafe.ai/blog/introducing-system-one-models-and-jev/) — TypeSafe AI launch blog (Sept 15). The primary source.
- [Jev introduces a new shape of LLM — System One, aka Decision Models](https://simonwillison.net/2026/Sep/21/jev/) — Simon Willison (Sept 21). Best independent explainer, with healthy skepticism.
- [Startup TypeSafe AI's Jev Model Sparks Copycats, Talk of LLM Alternatives](https://www.wsj.com/tech/ai/startup-typesafe-ais-jev-model-sparks-copycats-talk-of-llm-alternatives-e39ff57d) — *Wall Street Journal* (Oct 2). The copycat roundup; source of the Fortune-500 claims. (Paywalled.)
- [What Is Jev? TypeSafe's Decision Model, Tested Against LLMs](https://aimlapi.com/blog/what-is-jev) — AI/ML API (Sept 2026). The independent benchmark: accuracy, price, speed, calibration numbers.
- [Jev is the fastest-adopted model in AI Gateway history](https://vercel.com/blog/ai-gateway-jev-model-launch) — Vercel blog. Strongest third-party adoption data.
- [Introducing Clef: Cloudflare's decision models](https://blog.cloudflare.com/clef-decision-models) — Cloudflare blog (Oct 1). The most aggressive clone, in their own words.
- [OpenAI's Jev clone could help the frontier lab stop its swarming agents](https://techcrunch.com/2026/09/30/openais-jev-clone-could-help-the-frontier-lab-stop-its-swarming-agents/) — *TechCrunch* (Sept 30).
- [Amazon releases its own Jev clone as decision models flood the web](https://techcrunch.com/2026/10/01/amazon-releases-its-own-jev-clone-as-decision-models-flood-the-web/) — *TechCrunch* (Oct 1). The Brooker side-project backstory.
- [Companies are putting Jev in charge of AI agent decisions — and prompt injection can influence the verdict](https://venturebeat.com/security/companies-are-putting-jev-in-charge-of-ai-agent-decisions-and-prompt-injection-can-influence-the-verdict) — *VentureBeat*. The security caveat.
- [Decision AI models explained: TypeSafe Jev vs open-source competitors](https://www.marktechpost.com/2026/10/02/decision-ai-models-explained-typesafe-jev-vs-fastino-glide-gliner2-5-decide-and-open-source-competitors/) — *MarkTechPost* (Oct 2). Open-weight clone comparison.
