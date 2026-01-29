#!/usr/bin/env bash
set -euo pipefail

# Usage:
#   ./find_missing.sh list1.txt list2.txt out_diff.txt
#
# Output format (in out_diff.txt):
#   ONLY_IN_LIST1 <TAB> <original line from list1>
#   ONLY_IN_LIST2 <TAB> <original line from list2>

if [ $# -ne 3 ]; then
  echo "[ERROR] Usage: $0 <list1.txt> <list2.txt> <out_diff.txt>" >&2
  exit 1
fi

L1="$1"
L2="$2"
OUT="$3"

for f in "$L1" "$L2"; do
  if [ ! -f "$f" ]; then
    echo "[ERROR] Cannot find file: $f" >&2
    exit 1
  fi
done

UUID_RE='[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}'

awk -v re="$UUID_RE" '
function tolowhex(s,   i,c,out) {
  out=""
  for (i=1; i<=length(s); i++) {
    c=substr(s,i,1)
    out = out tolower(c)
  }
  return out
}
# Pass 1: read list1
FNR==NR {
  if (match($0, re)) {
    id = tolowhex(substr($0, RSTART, RLENGTH))
    if (!(id in seen1)) { line1[id] = $0 }   # keep first occurrence
    seen1[id] = 1
  }
  next
}
# Pass 2: read list2
{
  if (match($0, re)) {
    id = tolowhex(substr($0, RSTART, RLENGTH))
    if (!(id in seen2)) { line2[id] = $0 }
    seen2[id] = 1
  }
}
END {
  diff=0

  # only in list1
  for (id in seen1) {
    if (!(id in seen2)) {
      print "ONLY_IN_LIST1\t" line1[id]
      diff++
    }
  }

  # only in list2
  for (id in seen2) {
    if (!(id in seen1)) {
      print "ONLY_IN_LIST2\t" line2[id]
      diff++
    }
  }

  print "[INFO] Total differing UUIDs: " diff > "/dev/stderr"
}
' "$L1" "$L2" > "$OUT"

echo "[INFO] Wrote diff report -> $OUT"
