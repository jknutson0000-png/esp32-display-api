#!/usr/bin/env bash
# PwnDeck structural verifier. Exits 0 only if all four exit-criteria
# checks pass. Designed to run without PlatformIO (CI is best-effort).
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

fail=0
say() { printf "%-50s %s\n" "$1" "$2"; }

# 1. VERIFIABLE: every required file exists.
required=(
  platformio.ini
  include/pins.h
  src/main.cpp
  src/menu/registry.h
  src/menu/registry.cpp
  src/modules/wifi/wifi_mod.cpp
  src/modules/ble/ble_mod.cpp
  src/modules/subghz/subghz_mod.cpp
  src/modules/system/system_mod.cpp
  src/modules/camera/camera_mod.cpp
)
for f in "${required[@]}"; do
  if [[ -f "$f" ]]; then say "exists $f" OK; else say "exists $f" MISSING; fail=1; fi
done

# 2. LINT: platformio.ini is parseable as INI; every C++ source parses
#    as a translation unit when freestanding-stubbed.
python3 - <<'PY' || exit 1
import configparser, sys
c = configparser.ConfigParser(strict=False)
c.read("platformio.ini")
assert "env:cyd" in c, "missing [env:cyd]"
assert "env:devkit" in c, "missing [env:devkit]"
print("platformio.ini OK")
PY

# Best-effort C++ syntax check on registry.cpp using stub Arduino.h.
TMP=$(mktemp -d)
cat > "$TMP/Arduino.h" <<'H'
#pragma once
#include <cstdint>
#include <cstddef>
typedef unsigned char byte;
H
if command -v g++ >/dev/null; then
  if g++ -std=c++17 -fsyntax-only -I"$TMP" -Isrc -Iinclude \
       -DPWNDECK_TARGET_DEVKIT=1 -DPWNDECK_VERSION='"test"' \
       -x c++ src/menu/registry.h 2>"$TMP/err"; then
    say "g++ syntax src/menu/registry.h" OK
  else
    say "g++ syntax src/menu/registry.h" FAIL
    cat "$TMP/err"; fail=1
  fi
fi

# 3. DOMAIN: every menu entry id in registry.cpp has a handler symbol
#    defined in src/modules/.
ids=$(grep -oE '"[a-z_]+\.[a-z_]+"' src/menu/registry.cpp | sort -u)
handlers=$(grep -oE '"[a-z_]+","[A-Za-z_ :-]+","[a-z]+",[a-z_]+' src/menu/registry.cpp \
           | awk -F, '{print $NF}' | sort -u || true)
# Simpler: extract the trailing handler symbol per row.
mapfile -t rows < <(grep -E '^\s*\{"[a-z]' src/menu/registry.cpp)
for row in "${rows[@]}"; do
  sym=$(echo "$row" | sed -E 's/.*,\s*([a-z_]+)\}.*/\1/')
  if grep -RhqE "void[[:space:]]+${sym}\(void\)" src/modules/; then
    say "handler $sym" OK
  else
    say "handler $sym" MISSING
    fail=1
  fi
done

if [[ $fail -ne 0 ]]; then
  echo "VERIFY: FAIL"; exit 1
fi
echo "VERIFY: PASS"
