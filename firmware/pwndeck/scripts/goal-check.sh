#!/usr/bin/env bash
# PwnDeck /goal acceptance check — the filled-in version of the
# template in the session goal. Exits 0 iff ALL four conditions hold.
#
# TASK SUMMARY: build the most user-friendly custom Bruce-alternative
# ESP32 red-team firmware (PwnDeck) for the hardware in the Drive
# folder Esp32 (CYD, DevKit V1, ESP32-CAM, CC1101, E07-M1101D), with
# local camera access exposed over LAN.
#
# Exit when ALL of the following hold:
#   1. VERIFIABLE  : pio run -e cyd, -e devkit, -e cam all exit 0
#   2. FILE/STATE  : git status is clean except firmware/pwndeck/.pio/
#                    (build artifacts are gitignored)
#   3. COMPILE/LINT: firmware/pwndeck/scripts/verify.sh exits 0
#                    (INI parse + g++ -fsyntax-only on registry.h)
#   4. DOMAIN      : every menu entry id in src/menu/registry.cpp has
#                    a matching `void <handler>(void)` definition in
#                    src/modules/*/.
#
# Scope guardrails:
#   touch only : firmware/pwndeck/**, .github/workflows/pwndeck-build.yml
#   never edit : README.md (root), config.ru, public/**
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../../.." && pwd)"
cd "$ROOT"

ok=1
hr() { printf '\n=== %s ===\n' "$1"; }

hr "1/4 pio run all envs"
for e in cyd devkit cam; do
  if (cd firmware/pwndeck && pio run -e "$e" >/tmp/goal-pio-$e.log 2>&1); then
    echo "  $e SUCCESS"
  else
    echo "  $e FAILED (see /tmp/goal-pio-$e.log)"; ok=0
  fi
done

hr "2/4 git status clean"
dirty=$(git status --porcelain | grep -v '^?? firmware/pwndeck/\.pio/' || true)
if [[ -z "$dirty" ]]; then echo "  clean"; else echo "$dirty"; ok=0; fi

hr "3/4 verify.sh"
if bash firmware/pwndeck/scripts/verify.sh >/tmp/goal-verify.log 2>&1; then
  tail -2 /tmp/goal-verify.log
else
  cat /tmp/goal-verify.log; ok=0
fi

hr "4/4 menu->handler audit"
mapfile -t syms < <(grep -oE ',\s*[a-z_]+\}' firmware/pwndeck/src/menu/registry.cpp \
                    | sed -E 's/[,} ]//g' | sort -u)
missing=()
for s in "${syms[@]}"; do
  grep -rqE "void[[:space:]]+${s}\(void\)" firmware/pwndeck/src/modules/ || missing+=("$s")
done
if [[ ${#missing[@]} -eq 0 ]]; then
  echo "  ${#syms[@]}/${#syms[@]} handlers resolved"
else
  printf '  missing handler: %s\n' "${missing[@]}"; ok=0
fi

hr "RESULT"
if [[ $ok -eq 1 ]]; then echo "GOAL: PASS"; exit 0; else echo "GOAL: FAIL"; exit 1; fi
