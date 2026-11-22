#!/usr/bin/env bash
# Сборка + прошивка synth на Blue Pill (ST-LINK, st-flash) + проверка.
# Использование: ./flash.sh
set -euo pipefail

TOOLCHAIN="/home/prospect/src/storage/arm-gnu-toolchain-12.2.mpacbti-rel1-x86_64-arm-none-eabi/bin"
export PATH="$TOOLCHAIN:$PATH"

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ELF="$ROOT/Debug/synth.elf"
BIN="/tmp/opencode/synth.bin"
CHIP_HEAD="/tmp/opencode/chip_head.bin"

echo "==> 1/4 сборка"
make -C "$ROOT/Debug" -j"$(nproc)" all

echo "==> 2/4 elf -> bin"
mkdir -p "$(dirname "$BIN")"
arm-none-eabi-objcopy -O binary "$ELF" "$BIN"

echo "==> 3/4 прошивка в 0x08000000"
st-flash --reset write "$BIN" 0x08000000

echo "==> 4/4 проверка: чип против свежего elf"
st-flash read "$CHIP_HEAD" 0x08000000 16 >/dev/null 2>&1
if cmp -n 16 "$BIN" "$CHIP_HEAD"; then
    echo "OK: в чипе свежая прошивка ($ELF)"
else
    echo "MISMATCH: в чипе НЕ та прошивка, что собралась!" >&2
    exit 1
fi
