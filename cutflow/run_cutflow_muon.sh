#!/bin/bash
# run_cutflow_muon.sh

# You will get those arguments in submission file.
INPUT_FILE="$1"
CHANNEL="$2"      # ele or mu
SAMPLETYPE="$3"   # data or mc
OUTDIR="$4"

# ------------------------------------------------------------
# 0) set_env.sh가 source 되었는지 체크
# ------------------------------------------------------------
if [ -z "$RECO_DIR" ]; then
    echo "[ERROR] Environment not set."
    echo "Run: source set_env.sh"
    exit 1
fi

# ------------------------------------------------------------
# 1) 입력 ROOT 파일
# ------------------------------------------------------------
if [ -z "$INPUT_FILE" ] || [ -z "$CHANNEL" ] || [ -z "$SAMPLETYPE" ] || [ -z "$OUTDIR" ]; then
  echo "[ERROR] Usage: $0 <input.root> <ele|mu> <data|mc> <outdir>" >&2
  exit 1
fi

echo "[INFO] Starting cutflow for: ${INPUT_FILE}"
echo "[INFO] Channel=${CHANNEL} SampleType=${SAMPLETYPE}"
date

# ------------------------------------------------------------
# 2) ROOT 환경
# ------------------------------------------------------------
source /cvmfs/sft.cern.ch/lcg/releases/LCG_106a/ROOT/6.32.06/x86_64-el9-gcc13-opt/ROOT-env.sh

# ------------------------------------------------------------
# 3) Condor 작업 디렉토리
# ------------------------------------------------------------
CID=${ClusterId:-local}
PID=${ProcId:-$$}
WORKDIR="/tmp/${USER}/cutflow_${CID}_${PID}"

echo "[INFO] WORKDIR = ${WORKDIR}"
mkdir -p "${WORKDIR}" || exit 1

# ------------------------------------------------------------
# 4) 소스 복사
# ------------------------------------------------------------
cp "${CUTFLOW_DIR}/cutflow_single.C" "${WORKDIR}/" || exit 1
cp -r "${ANALYSIS_BASE}/include" "${WORKDIR}/" || exit 1
cp -r "${ANALYSIS_BASE}/external" "${WORKDIR}/" || exit 1
cp "${ANALYSIS_BASE}/json/Cert_Collisions2022_355100_362760_Golden.json" "${WORKDIR}/"|| exit 1
cp "${ANALYSIS_BASE}/json/Cert_Collisions2023_366442_370790_Golden.json" "${WORKDIR}/"|| exit 1
cp "${ANALYSIS_BASE}/json/Cert_Collisions2024_378981_386951_Golden.json" "${WORKDIR}/"|| exit 1

# ------------------------------------------------------------
# 5) 이동
# ------------------------------------------------------------
cd "${WORKDIR}" || {
    echo "[ERROR] cd to WORKDIR failed" >&2
    exit 1
}

# ------------------------------------------------------------
# 6) 출력 디렉토리
# ------------------------------------------------------------
echo "[INFO] OUTDIR = ${OUTDIR}"
mkdir -p "${OUTDIR}"

# ------------------------------------------------------------
# 7) 연도에 따라 JSON 파일명 선택.
# ------------------------------------------------------------
GOLDEN_JSON=""
if [[ "${INPUT_FILE}" == *"Run2022"* ]]; then
  GOLDEN_JSON="Cert_Collisions2022_355100_362760_Golden.json"
elif [[ "${INPUT_FILE}" == *"Run2023"* ]]; then
  GOLDEN_JSON="Cert_Collisions2023_366442_370790_Golden.json"
elif [[ "${INPUT_FILE}" == *"Run2024"* ]]; then
  GOLDEN_JSON="Cert_Collisions2024_378981_386951_Golden.json"
else
  echo "[ERROR] Cannot determine year (Run2023/Run2024) from INPUT_FILE: ${INPUT_FILE}" >&2
  exit 1
fi

# ------------------------------------------------------------
# 8) 실행.
# ------------------------------------------------------------
root -b -q "cutflow_single.C(\"${SAMPLETYPE}\", \"${INPUT_FILE}\", \"${CHANNEL}\", \"${GOLDEN_JSON}\", \"${OUTDIR}\")"
RET=$?

echo "[INFO] ROOT exited with code: ${RET}"
date
exit ${RET}

