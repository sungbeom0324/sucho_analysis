#!/bin/bash
# merge_cutflow.sh
# Usage:
#   ./merge_cutflow.sh <RESULT_DIR> <OUTPUT_FILE>
# ./merge_cutflow.sh results_2023_EGamma cutflow_2023_EGamma.txt
# ./merge_cutflow.sh results_2024_EGamma cutflow_2024_EGamma.txt 

if [ "$#" -ne 2 ]; then
  echo "Usage: $0 <RESULT_DIR> <OUTPUT_FILE>"
  exit 1
fi

RESULT_DIR="$1"
OUT="$2"

if [ ! -d "$RESULT_DIR" ]; then
  echo "[ERROR] Result directory not found: $RESULT_DIR"
  exit 1
fi

awk '
{
  # FNR = file-local line number = cut index
  sum[FNR] += $1

  # description은 첫 파일 기준으로 저장
  if (!(FNR in desc)) {
    $1 = ""
    sub(/^ /, "")
    desc[FNR] = $0
  }
}
END {
  print "=== Merged Cutflow (all files summed) ==="
  for (i = 1; i <= length(sum); i++) {
    printf "%-12d %s\n", sum[i], desc[i]
  }
}
' "${RESULT_DIR}"/cutflow_*.txt > "${OUT}"

echo "[INFO] merged cutflow -> ${OUT}"
