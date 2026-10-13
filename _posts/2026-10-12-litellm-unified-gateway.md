---
layout: post
title: "LiteLLM: One OpenAI-Compatible Gateway for Every LLM Provider"
tags: [AI, LLM, gateway, litellm, infrastructure]
categories: ai
date: 2026-10-12 22:10:00 -0700
---

Every app that calls more than one LLM provider eventually builds the same plumbing: per-provider clients, per-provider error handling, retry logic, usage tracking, key management. [LiteLLM](https://github.com/BerriAI/litellm) — BerriAI's open-source (MIT) AI gateway, ~60k GitHub stars — is the project that says: stop building it yourself. It puts 100+ providers behind one OpenAI-compatible interface, then adds the control plane: routing, fallbacks, budgets, guardrails, observability.

This post is a practical tour: what LiteLLM is, how to integrate it, and what to watch out for in production. All config below is verified against [docs.litellm.ai](https://docs.litellm.ai) as of October 2026.

## Two shapes: SDK and Proxy

LiteLLM comes in two forms that stack:

- **The Python SDK** — an in-process translation library. `litellm.completion(model="anthropic/claude-sonnet-4-5", ...)` works the same as the OpenAI call it mirrors, and `litellm.Router` adds load balancing, retries, and fallbacks across deployments.
- **The Proxy Server** — a standalone HTTP service (default port 4000) exposing OpenAI-compatible endpoints (`/v1/chat/completions`, `/v1/embeddings`, `/v1/images/generations`, plus `/v1/messages` in Anthropic format, `/v1/responses`, `/v1/rerank`, and more). The proxy adds the control plane: virtual keys, per-key/team budgets, RPM/TPM limits, caching, guardrails, logging, an admin UI.

The layering, from the repo's own architecture doc: **your app → LiteLLM Proxy → LiteLLM SDK → provider APIs**.

## Integration 1: drop-in Python SDK

```bash
pip install litellm
```

```python
import os
from litellm import completion

os.environ["OPENAI_API_KEY"] = "sk-..."
os.environ["ANTHROPIC_API_KEY"] = "sk-ant-..."
# Bedrock reads AWS_ACCESS_KEY_ID / AWS_SECRET_ACCESS_KEY / AWS_REGION

for model in ["gpt-4o", "anthropic/claude-sonnet-4-5",
              "bedrock/anthropic.claude-sonnet-4-20250514-v1:0"]:
    r = completion(model=model, messages=[{"role": "user", "content": "Hello!"}])
    print(model, "->", r.choices[0].message.content[:60])
    print("cost:", r._hidden_params["response_cost"])  # per-call cost, any provider
```

The model naming convention is `<provider>/<provider-model-id>`; a bare `gpt-4o` means OpenAI. Azure is `azure/<deployment-name>` plus `api_base`/`api_version`; Ollama is `ollama/<model>` (defaults to `http://localhost:11434`); any OpenAI-compatible server works via `openai/<model>` + `api_base`. Async and streaming use the same shapes (`acompletion`, `stream=True`).

Token and cost helpers are built in: `litellm.token_counter(model, messages)`, `litellm.get_max_tokens(model)`, `litellm.completion_cost(response)` — no more hand-rolled tokenizer glue.

For reliability inside Python, `Router` treats same-`model_name` deployments as one load-balanced group:

```python
from litellm import Router

router = Router(
    model_list=[
        {"model_name": "gpt-4o",
         "litellm_params": {"model": "openai/gpt-4o",
                            "api_key": os.environ["OPENAI_API_KEY"], "rpm": 500}},
        {"model_name": "gpt-4o",
         "litellm_params": {"model": "azure/gpt-4o",
                            "api_key": os.environ["AZURE_API_KEY"],
                            "api_base": os.environ["AZURE_API_BASE"],
                            "api_version": "2024-08-01-preview", "rpm": 900}},
    ],
    fallbacks=[{"gpt-4o": ["anthropic/claude-sonnet-4-5"]}],
    routing_strategy="simple-shuffle",  # the docs' recommended default
    num_retries=3,
)
resp = await router.acompletion(model="gpt-4o", messages=[...])
```

## Integration 2: the Proxy Server

When more than one app or language needs the same gateway, run the proxy. Docker one-liner:

```bash
cat > config.yaml <<'EOF'
model_list:
  - model_name: gpt-4o
    litellm_params:
      model: openai/gpt-4o
      api_key: os.environ/OPENAI_API_KEY
  - model_name: claude-sonnet
    litellm_params:
      model: anthropic/claude-sonnet-4-5
      api_key: os.environ/ANTHROPIC_API_KEY

litellm_settings:
  num_retries: 3

general_settings:
  master_key: os.environ/LITELLM_MASTER_KEY
EOF

docker run -v $(pwd)/config.yaml:/app/config.yaml \
  -e OPENAI_API_KEY -e ANTHROPIC_API_KEY -e LITELLM_MASTER_KEY \
  -p 4000:4000 \
  docker.litellm.ai/berriai/litellm:1.90.2 \
  --config /app/config.yaml
```

(Pin a version tag in production — more on why below. Admin UI at `http://localhost:4000/ui`.)

Then every client just speaks OpenAI:

```python
import openai
client = openai.OpenAI(api_key="sk-your-virtual-key",
                       base_url="http://localhost:4000/v1")
resp = client.chat.completions.create(
    model="gpt-4o", messages=[{"role": "user", "content": "Hello!"}])
```

Mint virtual keys with the master key — never hand apps the master key itself:

```bash
curl -X POST http://localhost:4000/key/generate \
  -H "Authorization: Bearer $LITELLM_MASTER_KEY" \
  -H "Content-Type: application/json" \
  -d '{"key_alias": "team-a-svc", "models": ["gpt-4o", "claude-sonnet"],
       "max_budget": 50, "budget_duration": "30d",
       "tpm_limit": 100000, "rpm_limit": 60,
       "max_parallel_requests": 10, "duration": "90d"}'
# -> {"key": "sk-...", ...}  # shown once
```

Keys support model allowlists, spend budgets, RPM/TPM caps, parallel-request caps, and expiry — managed via `/key/info|list|update|delete|block|unblock`; teams via `/team/new`.

### Fallbacks across providers

```yaml
litellm_settings:
  fallbacks: [{"smart": ["smart-fallback-1", "smart-fallback-2"]}]
```

Point `smart` at OpenAI, `smart-fallback-1` at Anthropic, `smart-fallback-2` at Bedrock — one config line buys you provider-outage resilience. There are also `context_window_fallbacks` and `content_policy_fallbacks` variants. **Tune `num_retries`**: with defaults, one independent benchmark saw ~4.7s spent retrying a dead primary before failover; `num_retries: 0` cut failover to tens of milliseconds.

### Budgets

Budgets live at three levels — per key (`/key/generate` body: `max_budget`, `budget_duration`), per team, and global (`litellm_settings.max_budget`). On exceed, the proxy rejects the request with a budget-exceeded error. And this one is load-bearing:

> **Budgets are NOT enforced without Postgres.** The docs say so explicitly — without a database, `max_budget` is inert and virtual keys don't work at all.

### Guardrails (current format)

Use the current top-level `guardrails:` format (the older `litellm_settings.guardrails` + callbacks style is legacy):

```yaml
guardrails:
  - guardrail_name: pii-mask
    litellm_params:
      guardrail: presidio   # OSS PII masking
      mode: pre_call
  - guardrail_name: injection-check
    litellm_params:
      guardrail: bedrock    # AWS Bedrock ApplyGuardrail
      mode: pre_call
      guardrailIdentifier: <your-bedrock-guardrail-id>
      guardrailVersion: "DRAFT"
      aws_region_name: os.environ/AWS_REGION
```

Attach per request (`"guardrails": ["pii-mask"]` in the request body) or always-on via `policies:` pipelines. Note the OSS/Enterprise boundary here: Presidio and Bedrock guardrails are OSS; named callbacks like `lakera_prompt_injection`, `aporia_prompt_injection`, `hide_secrets`, `openai_moderations`, and key/team-scoped guardrails are Enterprise-only.

**Streaming gotcha:** `post_call` guardrails buffer the *entire stream* until the scan passes. That's the safe default, but it adds latency — decide deliberately.

### Observability

```yaml
litellm_settings:
  success_callback: ["langfuse"]
  failure_callback: ["sentry"]
  callbacks: ["otel"]
  service_callbacks: ["datadog", "prometheus"]
  turn_off_message_logging: true   # log metadata only, for privacy
```

Prometheus exposes `litellm_spend`, `litellm_requests`, `litellm_total_tokens_metric_total`, and more. SDK-side: `litellm.success_callback = ["langfuse"]`.

## Production deployment

The official production path: **Postgres** (keys, teams, users, spend logs, config — required for auth, virtual keys, and budgets), **Redis** (rate-limit counters, router/cooldown state, caching — required once you run more than one replica), then 2+ stateless proxy replicas behind a load balancer with `/health/liveliness` and `/health/readiness` probes.

Deployment shapes: Docker Compose for small setups; Helm for Kubernetes — a monolithic `litellm-helm` chart, or a microservices layout (gateway :4000, backend :4001, UI :3000). There are official AWS/GCP Terraform modules.

Gotchas worth knowing before you ship:

1. **Pin your version.** In March 2026, malicious LiteLLM versions (1.82.7/1.82.8) sat on PyPI for ~40 minutes (Docker was unaffected). Pin tags; the `-stable` tags get extended load tests.
2. **Set `LITELLM_SALT_KEY` once and never change it** — it encrypts provider keys in the DB; regenerating it makes stored credentials unreadable.
3. **Autoscale on RPS/TPS, not CPU** — streaming pods sit idle on CPU while holding connections.
4. **Set per-deployment `rpm`/`tpm` honestly** — they also weight the router's shuffle, and share them via Redis across replicas.
5. **v1.84.0 breaking change:** `os.environ/*` is no longer resolved from key/team logging metadata — put callback secrets in trusted `config.yaml`, not DB metadata.
6. The cost catalog (`model_prices_and_context_window.json`) is community-maintained — override `input_cost_per_token`/`output_cost_per_token` for custom models, or your spend numbers lie.

## How it compares

| | LiteLLM | Portkey AI Gateway | OpenRouter | Cloudflare AI Gateway | Raw provider SDKs |
|---|---|---|---|---|---|
| Model | Self-hosted, MIT + Enterprise + managed Cloud | OSS (Apache 2.0) + SaaS; now Palo Alto's Prisma AIRS (acquired May 2026) | Managed SaaS aggregator | Managed SaaS on CF edge | Direct clients |
| Gateway cost | Free self-hosted; Enterprise quote by volume | OSS free; SaaS from $49/mo | ~5.5% on credits | Free on paid CF accounts | $0 |
| Breadth | 100+ providers + custom endpoints | 1,600+ models | 500+ models / 80+ providers | Majors + Workers AI | One each |
| Caching | Response + semantic (Redis, you manage it) | Basic; semantic on select plans | Exact-response + TTL | Edge caching | Prompt cache only |
| Guardrails | Presidio/Bedrock/custom OSS; more via Enterprise | Built-in guardrails + circuit breakers | Org guardrails | DLP/PII free | Provider-side |
| Budgets | Per key/team/user, multi-window | Per-key caps | Per-key credit limits | Daily/weekly/monthly | Provider consoles |

**Choose LiteLLM when** you want MIT-licensed, vendor-neutral infrastructure — especially post-acquisition, if you'd rather your gateway not belong to a security vendor — with self-hosted data, zero per-token markup, and fine-grained per-team budgets. **Don't when** you want zero ops (OpenRouter, Cloudflare, Vercel), need vendor-backed SSO/SCIM/audit logs without self-hosting, or run extreme-throughput latency-critical paths (Rust/Go gateways like Bifrost or Kong AI Gateway exist for that).

Worth noting: LiteLLM is expanding beyond model routing into a "single control plane for LLMs, MCP servers, and agents" — it now ships an MCP gateway (`mcp_servers`, `/mcp`) and A2A agent support with a zero-trust `mcp_jwt_signer` guardrail for MCP egress. The gateway thesis keeps growing.

## Good reads

- [LiteLLM Proxy Quickstart](https://docs.litellm.ai/docs/proxy/docker_quick_start) — docs.litellm.ai. The canonical Docker start; includes the DB-less caveats.
- [Production Deployment](https://docs.litellm.ai/docs/proxy/deploy) — docs.litellm.ai. Monolithic vs microservices, Helm, Postgres/Redis, RPS/TPS autoscaling.
- [Router — Load Balancing](https://docs.litellm.ai/docs/routing) — docs.litellm.ai. Strategies, routing groups, session affinity.
- [Bedrock Guardrails](https://docs.litellm.ai/docs/proxy/guardrails/bedrock) — docs.litellm.ai. Current guardrail format, PII masking, streaming buffering.
- [Completion Token Usage & Cost](https://docs.litellm.ai/docs/completion/token_usage) — docs.litellm.ai. Token counting and cost helpers.
- [Langfuse integration](https://docs.litellm.ai/docs/observability/langfuse_integration) — docs.litellm.ai. Proxy/SDK logging setup.
- [Enterprise vs OSS](https://github.com/berriai/litellm-docs/blob/HEAD/docs/enterprise.md) — litellm-docs. Where the OSS boundary sits.
- [MCP Deployment Guide](https://github.com/berriai/litellm-docs/blob/HEAD/docs/mcp_deployment.md) — litellm-docs. MCP + A2A control plane.
- [Top 5 Enterprise AI Gateways (2026)](http://dev.to/sebuzdugan/top-5-enterprise-ai-gateways-2026-bifrost-vs-kong-vs-litellm-vs-cloudflare-vs-vercel-3i10) — dev.to, Sep 2026. Independent throughput tests and the retry-default gotcha.
- [OpenRouter alternatives: 6 unified LLM APIs compared (2026)](https://www.braintrust.dev/articles/openrouter-alternatives-2026) — Braintrust, Aug 2026. Seven-way comparison matrix (vendor-written).
- [Best Portkey Alternatives in 2026](https://dev.to/daniel_willson/best-portkey-alternatives-in-2026-top-3-open-source-ai-gateways-1k4n) — dev.to, Oct 2026. Gateway requirements post-acquisition.
- [LiteLLM: One OpenAI-Compatible Gateway for 100+ LLMs](https://addrom.com/litellm-one-openai-compatible-gateway-for-100-llms/) — addrom.com. Independent practitioner explainer.
