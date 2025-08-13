#!/usr/bin/env python3
"""Analyze BASIC sample programs for instruction usage.

Scans all `.bas` files under the samples directory and generates an HTML
matrix where the first column lists instructions and each subsequent column
shows how many times the instruction appears in a given source file.

A list of rarely used instructions (less than a threshold, default 12) is
appended below the table.
"""
import argparse
import html
import os
import re
from collections import Counter

INSTR_RE = re.compile(r"^[A-Z]+")


def load_keywords():
    """Extract BASIC keywords from the compiler source."""
    keywords = set()
    src = os.path.join(os.path.dirname(__file__), "src", "basicc.c")
    try:
        with open(src, "r", encoding="utf-8", errors="ignore") as f:
            in_array = False
            for line in f:
                if "keywords[]" in line:
                    in_array = True
                elif in_array:
                    if "{NULL" in line:
                        break
                    m = re.search(r'\{"([^"\n]+)"', line)
                    if m:
                        keywords.add(m.group(1))
    except FileNotFoundError:
        pass
    return keywords


def gather_bas_files(samples_dir):
    bas_files = []
    for root, _dirs, files in os.walk(samples_dir):
        for name in files:
            if name.lower().endswith(".bas"):
                bas_files.append(os.path.join(root, name))
    return sorted(bas_files)


def parse_file(path, keywords):
    counts = Counter()
    with open(path, "r", encoding="utf-8", errors="ignore") as f:
        for line in f:
            # Remove strings
            line = re.sub(r'"[^"\n]*"', '', line)
            # Remove single-quote comments
            line = line.split("'", 1)[0]
            # Remove REM comments
            line = re.split(r"\bREM\b", line, flags=re.IGNORECASE)[0]
            for stmt in line.split(":"):
                stmt = stmt.lstrip()
                stmt = re.sub(r"^\d+\s*", "", stmt)  # Drop line numbers
                if not stmt:
                    continue
                m = INSTR_RE.match(stmt.upper())
                if m:
                    word = m.group(0)
                    if word in keywords:
                        counts[word] += 1
    return counts


def analyze(samples_dir):
    keywords = load_keywords()
    bas_files = gather_bas_files(samples_dir)
    file_counts = {}
    all_instrs = set()
    for f in bas_files:
        counts = parse_file(f, keywords)
        file_counts[f] = counts
        all_instrs.update(counts)
    return bas_files, file_counts, all_instrs


def build_html(samples_dir, bas_files, file_counts, all_instrs, threshold):
    headers = ["Instruction"]
    headers.extend(html.escape(os.path.relpath(f, samples_dir)) for f in bas_files)
    lines = ["<table>", "  <thead>", "    <tr>"]
    lines.extend(f"      <th>{h}</th>" for h in headers)
    lines.extend(["    </tr>", "  </thead>", "  <tbody>"])

    global_counts = Counter()
    for counts in file_counts.values():
        global_counts.update(counts)

    for instr in sorted(all_instrs):
        lines.append("    <tr>")
        lines.append(f"      <td>{instr}</td>")
        for f in bas_files:
            c = file_counts[f].get(instr, 0)
            cell = str(c) if c else ""
            lines.append(f"      <td>{cell}</td>")
        lines.append("    </tr>")
    lines.extend(["  </tbody>", "</table>"])

    rare = [i for i, c in global_counts.items() if c < threshold]
    if rare:
        lines.append("<p>Rare instructions (&lt;{} uses): {}</p>".format(
            threshold,
            ", ".join(f"{i} ({global_counts[i]})" for i in sorted(rare))
        ))
    else:
        lines.append("<p>No rare instructions (&lt;{} uses).</p>".format(threshold))
    return "\n".join(lines)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--samples", default=os.path.join(os.path.dirname(__file__), "samples"), help="Directory with BASIC samples")
    parser.add_argument("-o", "--output", default="instruction_usage.html", help="Output HTML file path")
    parser.add_argument("--threshold", type=int, default=12, help="Threshold for rare instruction frequency")
    args = parser.parse_args()

    bas_files, file_counts, all_instrs = analyze(args.samples)
    html_text = build_html(args.samples, bas_files, file_counts, all_instrs, args.threshold)
    with open(args.output, "w", encoding="utf-8") as f:
        f.write(html_text)

    # Echo rare instructions on stdout for convenience
    global_counts = Counter()
    for counts in file_counts.values():
        global_counts.update(counts)
    rare = [i for i, c in global_counts.items() if c < args.threshold]
    if rare:
        print(f"Rare instructions (<{args.threshold} uses):")
        for i in sorted(rare):
            print(f"  {i}: {global_counts[i]}")


if __name__ == "__main__":
    main()
