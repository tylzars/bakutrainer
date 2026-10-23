#!/usr/bin/env python3
"""Tracks matching progress over time and renders the README graph.

  progress.py update   append the current objdiff report to progress/history.csv
                       (if it changed), redraw progress/progress.svg, rewrite the
                       README section between the progress markers
  progress.py check    exit 1 if the report differs from the last recorded point
                       (used by .githooks/pre-push)

Both read build/<ver>/report.json, so run `ninja report` first.

Every percentage is a share of one fixed size: the bytes of the code sections in config/ (kind:code sections of every
module's delinks.txt). objdiff's own total only covers bytes that have a symbol, so it grows whenever a merge names
bytes for the first time (a missed function, a table in .text) and the percentage would drop although nothing got
worse. Measured against the fixed size, progress only moves when matched bytes do. Each history row keeps the size
it was measured against (image_code), so a config change that moves bytes between code and data (the .text/.rodata
boundary) shows as one step instead of redrawing every earlier point.
"""
import argparse
import csv
import json
import re
import sys
from datetime import datetime, timezone
from pathlib import Path
from project import VERSION, module_paths, modules  # noqa: E402  (tools/project.py)

ROOT = Path(__file__).resolve().parent.parent
HISTORY = ROOT / "progress" / "history.csv"
SVG = ROOT / "progress" / "progress.svg"
README = ROOT / "README.md"
START, END = "<!-- progress:start -->", "<!-- progress:end -->"
FIELDS = ["date", "matched_code", "complete_code", "total_code", "matched_functions", "total_functions",
          "fuzzy_match_percent", "image_code"]
# The code size of every row recorded before image_code was a column.
FIRST_IMAGE_CODE = 389724

# (key, label, light, dark): categorical slots 1-3, validated light and dark.
# matched_code: compiled C byte-identical to the original assembly.
# fuzzy_match_percent: objdiff's overall similarity, crediting near-matches too.
# complete_code: code in units built from source and linked into the ROM.
SERIES = [
    ("matched_code", "Matched code", "#2a78d6", "#3987e5"),
    ("fuzzy_match_percent", "Overall accuracy", "#eb6834", "#d95926"),
    ("complete_code", "Linked code", "#1baf7a", "#199e70"),
]


def image_code() -> int:
    """Bytes of every module's code sections (the section lines at the top of its delinks.txt with kind:code)."""
    total = 0
    for module in modules():
        delinks = module_paths(module)[0] / "delinks.txt"
        if not delinks.is_file():
            continue
        for line in delinks.read_text().splitlines():
            if line.strip() and not line[0].isspace():
                break  # the first unit: the section list is over
            f = dict(w.split(":", 1) for w in line.split()[1:] if ":" in w)
            if f.get("kind") == "code":
                total += int(f["end"], 16) - int(f["start"], 16)
    return total - linker_fill()


def linker_fill() -> int:
    """Bytes of zero fill the link lays down in code sections (lcf_edits.txt `org` lines): from the end of the last
    .text range below each `org` address up to it. No unit holds them, so they are not code to decompile."""
    edits = module_paths("main")[0] / "lcf_edits.txt"
    if not edits.is_file():
        return 0
    ends = [int(m.group(1), 16) for m in re.finditer(r"^\s+\.text\s+start:0x[0-9a-f]+ end:(0x[0-9a-f]+)",
                                                      (module_paths("main")[0] / "delinks.txt").read_text(), re.M)]
    fill = 0
    for line in edits.read_text().splitlines():
        words = line.split("#", 1)[0].split()
        if len(words) == 3 and words[0] == "org":
            address = int(words[2], 0)
            fill += address - max(e for e in ends if e <= address)
    return fill


def complete_code() -> int:
    """Bytes of the code sections' ranges held by complete units (delinks.txt). objdiff's complete_code only counts
    bytes a function symbol covers, so the bytes no symbol covers inside linked units (the secure area outside its
    entry points, alignment between functions) would keep linked code short of the fixed size for good."""
    total = 0
    for module in modules():
        delinks = module_paths(module)[0] / "delinks.txt"
        if not delinks.is_file():
            continue
        kinds, unit, complete = {}, None, False
        for line in delinks.read_text().splitlines():
            if not line.strip() or line.startswith("//"):
                continue
            if not line[0].isspace():
                unit, complete = line, False
                continue
            words = line.split()
            f = dict(w.split(":", 1) for w in words[1:] if ":" in w)
            if unit is None:
                kinds[words[0]] = f.get("kind")
            elif words[0] == "complete":
                complete = True
            elif complete and kinds.get(words[0]) == "code":
                total += int(f["end"], 16) - int(f["start"], 16)
    return total


def read_report(ver: str) -> dict:
    path = ROOT / "build" / ver / "report.json"
    if not path.is_file():
        sys.exit(f"{path} missing; run `ninja report` first")
    m = json.loads(path.read_text())["measures"]
    row = {k: int(m.get(k, 0)) for k in FIELDS[1:-2]}
    row["complete_code"] = complete_code()
    row["fuzzy_match_percent"] = round(float(m.get("fuzzy_match_percent", 0)), 3)
    row["image_code"] = IMAGE_CODE
    return row


def read_history() -> list[dict]:
    if not HISTORY.is_file():
        return []
    with HISTORY.open() as f:
        rows = [{k: v if k == "date" else float(v) if k == "fuzzy_match_percent" else int(v)
                 for k, v in row.items() if v not in (None, "")} for row in csv.DictReader(f)]
    for row in rows:
        row.setdefault("image_code", FIRST_IMAGE_CODE)
    return rows


def same(a: dict, b: dict) -> bool:
    return all(a[k] == b[k] for k in FIELDS[1:])


IMAGE_CODE = image_code()


def pct(row: dict, key: str) -> float:
    """A share of the fixed code size. objdiff's fuzzy percentage is over its own total, so it is turned back into
    credited bytes first (bytes without a symbol carry no credit either way). Each row uses the size it was measured
    against."""
    size = row.get("image_code", IMAGE_CODE)
    if key == "fuzzy_match_percent":
        return row[key] * row["total_code"] / size if size else 0.0
    return 100 * row[key] / size if size else 0.0


def nice_ceiling(v: float) -> float:
    for c in (1, 2, 5, 10, 20, 25, 50, 75, 100):
        if v <= c:
            return c
    return 100


def _theme_css(light: bool) -> str:
    surface, text, muted, grid = ("#fcfcfb", "#52514e", "#898781", "#e1e0d9") if light else \
                                 ("#1a1a19", "#c3c2b7", "#898781", "#2c2c2a")
    css = f".bg{{fill:{surface}}}.grid{{stroke:{grid}}}.muted{{fill:{muted}}}.lbl{{fill:{text}}}"
    for i, (_, _, lc, dc) in enumerate(SERIES):
        c = lc if light else dc
        css += f".ln{i}{{stroke:{c}}}.dot{i}{{fill:{c};stroke:{surface}}}"
    return css


BREAK_GAP = 3600  # seconds; longer pauses between points are collapsed
BREAK_WIDTH = 1800  # seconds of x axis a collapsed pause takes up


def active_times(stamps: list[float]) -> tuple[list[float], list[int]]:
    """Maps timestamps onto working time: a pause longer than BREAK_GAP counts as BREAK_WIDTH, so days without
    work don't stretch the x axis. Also returns the indices of points that follow such a break."""
    times, breaks = [0.0], []
    for i in range(1, len(stamps)):
        gap = stamps[i] - stamps[i - 1]
        if gap > BREAK_GAP:
            gap = BREAK_WIDTH
            breaks.append(i)
        times.append(times[-1] + gap)
    return times, breaks


def render_svg(rows: list[dict]) -> str:
    W, H = 720, 300
    L, R, T, B = 48, 176, 24, 36
    pw, ph = W - L - R, H - T - B
    dates = [datetime.fromisoformat(r["date"]) for r in rows]
    times, breaks = active_times([d.timestamp() for d in dates])
    t0, t1 = times[0], times[-1]
    ymax = nice_ceiling(max(pct(r, k) for r in rows for k, *_ in SERIES))

    def x(t):
        return L + (pw / 2 if t1 == t0 else (t - t0) / (t1 - t0) * pw)

    def y(v):
        return T + ph - v / ymax * ph

    out = [
        f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 {W} {H}" width="{W}" height="{H}" '
        f'role="img" aria-label="Decompilation progress over time">',
        "<style>",
        _theme_css(light=True),
        "@media (prefers-color-scheme: dark){" + _theme_css(light=False) + "}",
        "text{font:12px -apple-system,BlinkMacSystemFont,'Segoe UI',Helvetica,Arial,sans-serif}",
        "</style>",
        f'<rect class="bg" width="{W}" height="{H}" rx="6"/>',
    ]
    for i in range(5):
        v = ymax * i / 4
        out.append(f'<line x1="{L}" x2="{L + pw}" y1="{y(v):.1f}" y2="{y(v):.1f}" class="grid" stroke-width="1"/>')
        out.append(f'<text x="{L - 8}" y="{y(v) + 4:.1f}" class="muted" text-anchor="end">{v:g}%</text>')
    # Collapsed pauses: a dashed rule midway through the gap.
    for i in breaks:
        bx = (x(times[i - 1]) + x(times[i])) / 2
        out.append(f'<line x1="{bx:.1f}" x2="{bx:.1f}" y1="{T}" y2="{T + ph}" class="grid" stroke-width="1" '
                   f'stroke-dasharray="3 3"/>')
    # Date labels (local time) at the start of each work session that begins a new day. The first and the latest
    # day always show; a day in between is skipped if its label would overlap one already placed.
    starts, seen = [], set()
    for i in [0, *breaks]:
        day = dates[i].astimezone().date()
        if day not in seen:
            seen.add(day)
            starts.append((i, day))
    # Only the first label carries the year; the rest are month-day, which keeps them narrow enough to fit.
    spans = []
    for k, (i, day) in enumerate(starts):
        text, w = (f"{day:%Y-%m-%d}", 72) if k == 0 else (f"{day:%m-%d}", 36)
        lx = x(times[i])
        anchor = "start" if lx < L + w / 2 else "end" if lx > L + pw - w / 2 else "middle"
        left = lx if anchor == "start" else lx - w if anchor == "end" else lx - w / 2
        spans.append((left, left + w, lx, anchor, text, k in (0, len(starts) - 1)))
    placed = [sp for sp in spans if sp[5]]
    for sp in spans:
        if not sp[5] and all(sp[1] + 8 <= q[0] or sp[0] >= q[1] + 8 for q in placed):
            placed.append(sp)
    for _, _, lx, anchor, text, _ in sorted(placed):
        out.append(f'<text class="muted" x="{lx:.1f}" y="{H - 12}" text-anchor="{anchor}">{text}</text>')

    # Lines first, then end dots with surface rings, then direct labels (nudged apart).
    ends = []
    for i, (key, label, *_) in enumerate(SERIES):
        pts = [(x(t), y(pct(r, key))) for t, r in zip(times, rows)]
        if len(pts) > 1:
            d = " ".join(f"{px:.1f},{py:.1f}" for px, py in pts)
            out.append(f'<polyline points="{d}" fill="none" class="ln{i}" stroke-width="2" '
                       f'stroke-linejoin="round" stroke-linecap="round"/>')
        ends.append([pts[-1][1], i, label, pct(rows[-1], key), pts[-1][0]])
    for ey, i, _, _, ex in ends:
        out.append(f'<circle cx="{ex:.1f}" cy="{ey:.1f}" r="4" class="dot{i}" stroke-width="2"/>')
    # Stack labels upward from the lowest so they never collide or drop below the plot.
    ends.sort(reverse=True)
    ends[0][0] = min(ends[0][0], T + ph)
    for j in range(1, len(ends)):
        ends[j][0] = min(ends[j][0], ends[j - 1][0] - 16)
    # If the stack ran above the plot (all series near the top), move it down so the top label is not clipped.
    if ends[-1][0] < T:
        for e in ends:
            e[0] += T - ends[-1][0]
    for ly, i, label, v, _ in ends:
        out.append(f'<line x1="{L + pw + 10}" x2="{L + pw + 22}" y1="{ly:.1f}" y2="{ly:.1f}" class="ln{i}" stroke-width="2"/>')
        out.append(f'<text class="lbl" x="{L + pw + 28}" y="{ly + 4:.1f}">{label} {v:.2f}%</text>')
    out.append("</svg>")
    return "\n".join(out) + "\n"


def readme_section(rows: list[dict]) -> str:
    r = rows[-1]
    fn = f"{r['matched_functions']:,} / {r['total_functions']:,}"
    return "\n".join([
        START,
        "![Decompilation progress over time](progress/progress.svg)",
        "",
        "| Measure | Progress |",
        "|---|---|",
        f"| Matched code | {pct(r, 'matched_code'):.2f}% ({r['matched_code']:,} / {IMAGE_CODE:,} bytes) |",
        f"| Linked code (complete units) | {pct(r, 'complete_code'):.2f}% ({r['complete_code']:,} bytes) |",
        f"| Overall accuracy (fuzzy) | {pct(r, 'fuzzy_match_percent'):.2f}% |",
        f"| Matched functions | {fn} |",
        "",
        *([("All code is linked from this repository's source and the ROM matches byte for byte. See "
            "[docs/decompiling.md](docs/decompiling.md) for functions objdiff scores below 100% with identical "
            "code."), ""] if r["complete_code"] == IMAGE_CODE else []),
        f"_Last updated {r['date'][:10]}. Full history: [`progress/history.csv`](progress/history.csv)._",
        END,
    ])


def main() -> None:
    p = argparse.ArgumentParser()
    p.add_argument("cmd", choices=["update", "check"])
    p.add_argument("--version", default=VERSION)
    a = p.parse_args()

    cur = read_report(a.version)
    rows = read_history()

    if a.cmd == "check":
        if not rows or not same(rows[-1], cur):
            sys.exit("Progress graph is out of date: run `ninja report && tools/progress.py update` and commit.")
        return

    if not rows or not same(rows[-1], cur):
        rows.append({"date": datetime.now(timezone.utc).isoformat(timespec="seconds"), **cur})
        HISTORY.parent.mkdir(exist_ok=True)
        with HISTORY.open("w", newline="") as f:
            wr = csv.DictWriter(f, FIELDS)
            wr.writeheader()
            wr.writerows(rows)
    SVG.write_text(render_svg(rows))

    text = README.read_text()
    if START in text and END in text:
        text = text[:text.index(START)] + readme_section(rows) + text[text.index(END) + len(END):]
    else:
        text = text.rstrip() + "\n\n## Progress\n" + readme_section(rows) + "\n"
    README.write_text(text)
    print(f"matched {pct(rows[-1], 'matched_code'):.2f}%, overall {pct(rows[-1], 'fuzzy_match_percent'):.2f}%, "
          f"linked {pct(rows[-1], 'complete_code'):.2f}% ({len(rows)} points)")


if __name__ == "__main__":
    main()
