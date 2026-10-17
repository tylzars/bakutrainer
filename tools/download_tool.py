#!/usr/bin/env python3
"""Downloads a third-party build tool into tools/."""
import argparse
import hashlib
import io
import stat
import sys
import urllib.request
import zipfile
from pathlib import Path

URLS = {
    "wibo": "https://github.com/decompals/wibo/releases/download/{tag}/wibo-x86_64",
    "objdiff": "https://github.com/encounter/objdiff/releases/download/{tag}/objdiff-cli-linux-x86_64",
    # Metrowerks CodeWarrior compilers: DS 1.2 and 2.0 builds, DSi 1.1 and 1.2 builds
    "mwccarm": "http://decomp.aetias.com/files/mwccarm.zip",
}
# The compiler bundle is only published as "latest", so the files the build runs are pinned by hash (paths are
# relative to --path, where the zip's mwccarm/ folder lands).
SHA256 = {
    "mwccarm/2.0/sp2p2/mwccarm.exe": "b1da5a2f191d1cdea3fd078cedc9c504de46609642ae8e44c015014cef5419a7",
    "mwccarm/2.0/sp2p2/mwldarm.exe": "a41aea5f282041e16dcd6c3570328009cb1eefb32b0cd05f60a8fdb365aec47c",
    "mwccarm/2.0/sp2p3/mwccarm.exe": "900e6b2ce072b5c178e98d47468f83caa8156eda7fb2e1efa786535b73cf86f2",
    "mwccarm/2.0/sp2p3/mwldarm.exe": "a41aea5f282041e16dcd6c3570328009cb1eefb32b0cd05f60a8fdb365aec47c",
    "mwccarm/2.0/sp2p3/mwasmarm.exe": "f68e9567fc966c03e1cd6b1f5b0aa85db9a239996ee1824960b45192beaca6e2",
    "mwccarm/2.0/sp1p5/mwccarm.exe": "89b2fc16c7f1f29ede1e52bdee5bc8ae4d63fe7e50d8074354dfd92c43d47aca",
}

parser = argparse.ArgumentParser()
parser.add_argument("tool", choices=URLS)
parser.add_argument("tag")
parser.add_argument("--path", type=Path, required=True, help="output file, or extraction dir for zips")
parser.add_argument("--outputs", type=Path, nargs="*", default=[],
                    help="files the build needs from this tool; if all exist already, nothing is downloaded")
args = parser.parse_args()


def verify(root: Path) -> list[str]:
    bad = []
    for rel, digest in SHA256.items():
        f = root / rel
        if not f.is_file() or hashlib.sha256(f.read_bytes()).hexdigest() != digest:
            bad.append(str(f))
    return bad


# Ninja reruns this edge whenever .ninja_log has no entry for it (a new checkout or build copy), so tools that are
# already in place are kept rather than fetched again.
if args.outputs and all(p.is_file() for p in args.outputs):
    if args.tool == "mwccarm" and (bad := verify(args.path)):
        sys.exit(f"{', '.join(bad)}: missing or not the pinned compiler build (SHA-256)")
    sys.exit(0)

url = URLS[args.tool].format(tag=args.tag)
print(f"Downloading {args.tool} {args.tag} from {url}")
with urllib.request.urlopen(url) as resp:
    data = resp.read()

if url.endswith(".zip"):
    zipfile.ZipFile(io.BytesIO(data)).extractall(args.path)
    if args.tool == "mwccarm" and (bad := verify(args.path)):
        sys.exit(f"{', '.join(bad)}: missing from the download or not the pinned compiler build (SHA-256)")
else:
    args.path.parent.mkdir(parents=True, exist_ok=True)
    args.path.write_bytes(data)
    args.path.chmod(args.path.stat().st_mode | stat.S_IEXEC)
