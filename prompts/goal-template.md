# `/goal` Prompt Template

A reusable scaffold for writing effective `/goal` conditions in Claude Code.
`/goal` runs Claude autonomously turn after turn until a fast evaluator
(Haiku) confirms the condition is met. Conditions must be **measurable and
verifiable** — never subjective.

## Template

```
/goal <one-line task summary>:
  exit when ALL of the following hold:
    1. <verifiable command>      e.g. `npm test` exits 0
    2. <file / repo state check> e.g. `git status` is clean except <paths>
    3. <compile / lint check>    e.g. `tsc --noEmit` and `eslint .` exit 0
    4. <domain-specific check>   e.g. every call site of <symbol> in <dir>
                                       imports from <module>
  do NOT exit on:
    - partial progress, "looks good", or subjective judgement
    - skipped / xfail tests; treat them as failures
  scope guardrails:
    - touch only: <allowed paths / globs>
    - never edit: <forbidden paths>
  stop after <N> turns if condition still not met and report the blocker.
```

## Filled example — auth migration

```
/goal migrate auth module to new session API:
  exit when ALL of the following hold:
    1. `pnpm test test/auth` passes and `pnpm test` exits 0
    2. `rg "legacy_session" src/` returns no matches
    3. `tsc --noEmit` exits 0
    4. every call site of `getSession` in src/ imports from `@/auth/session`
  scope: only touch src/auth/** and test/auth/**; never edit src/db/**.
  stop after 20 turns if not met.
```

## Filled example — ESP32 API endpoint

```
/goal add PATCH /api/esp32-data endpoint with partial update support:
  exit when ALL of the following hold:
    1. `bundle exec rackup config.ru -p 3000` boots without errors
    2. `curl -X PATCH http://localhost:3000/api/esp32-data -H "Content-Type: application/json" -d '{"temp1":"500"}'`
       returns HTTP 200 and merges only the supplied keys into esp32_data.json
    3. existing GET and POST behavior is unchanged (manual curl checks pass)
    4. README.md documents the new endpoint with request/response examples
  scope: touch only config.ru, README.md, and public/app.js if needed.
  stop after 15 turns if not met.
```

## Checklist for writing a good condition

- [ ] Every clause is a shell command, file check, or string match a tool can run
- [ ] No words like "clean", "looks right", "best", "good enough"
- [ ] Skipped/xfail tests are explicitly disallowed
- [ ] Scope (allowed / forbidden paths) is stated
- [ ] A turn or time budget is set
- [ ] The full condition is under 4,000 characters

## Companion commands

| Command                              | Purpose                                  |
| ------------------------------------ | ---------------------------------------- |
| `/goal`                              | Status: condition, turns, tokens, reason |
| `/goal clear` (or stop/off/cancel)   | Cancel active goal                       |
| `claude -p "/goal <condition>"`      | Non-interactive single-shot loop         |
| `/goal ... or stop after 20 turns`   | Hard turn cap                            |

Pair with auto mode for fully unattended runs. Resume after a crash with
`--resume` or `--continue` — the condition carries over, counters reset.
