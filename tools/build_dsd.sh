#!/usr/bin/env bash
# Builds dsd (ds-decomp) from source with local patches applied.
#
# Why: ds-rom 0.8.0 sizes version-3 ("Korea") banners as 0x1240 bytes, but the
# format (and real ROMs' mastering) uses 0xA40. With the stock binary every
# file in the filesystem lands 0x800 bytes late, so the ROM can't match.
#
# Usage: tools/build_dsd.sh [output_path]   (default: tools/bin/dsd)
set -euo pipefail

DSD_TAG=v0.12.1
DS_ROM_TAG=v0.8.0

root="$(cd "$(dirname "$0")/.." && pwd)"
out="${1:-$root/tools/bin/dsd}"
work="$root/build/dsd-src"

mkdir -p "$work" "$(dirname "$out")"

if [ ! -d "$work/ds-rom" ]; then
    git clone -q --depth 1 --branch "$DS_ROM_TAG" https://github.com/AetiasHax/ds-rom "$work/ds-rom"
    git -C "$work/ds-rom" apply "$root/tools/patches/ds-rom-banner-v3-size.patch"
fi
if [ ! -d "$work/ds-decomp" ]; then
    git clone -q --depth 1 --branch "$DSD_TAG" https://github.com/AetiasHax/ds-decomp "$work/ds-decomp"
    printf '\n[patch.crates-io]\nds-rom = { path = "../ds-rom/lib" }\n' >> "$work/ds-decomp/Cargo.toml"
fi

cargo build --release -q --manifest-path "$work/ds-decomp/Cargo.toml" -p ds-decomp-cli
cp "$work/ds-decomp/target/release/dsd" "$out"
"$out" --version
