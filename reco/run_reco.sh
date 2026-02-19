#!/bin/bash

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
INPUT_FILE="$1"

if [ -z "$INPUT_FILE" ]; then
    echo "[ERROR] No input file given" >&2
    exit 1
fi

echo "[INFO] Starting reco for: ${INPUT_FILE}"
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
WORKDIR="/tmp/${USER}/reco_${CID}_${PID}"

mkdir -p "${WORKDIR}" || exit 1

# ------------------------------------------------------------
# 4) 소스 복사
# ------------------------------------------------------------
cp "${RECO_DIR}/reco_single.C" "${WORKDIR}/" || exit 1
cp -r "${ANALYSIS_BASE}/include" "${WORKDIR}/" || exit 1
cp "${RECO_DIR}/json.hpp" "${WORKDIR}/"|| exit 1
cp "${RECO_DIR}/Cert_Collisions2022_355100_362760_Golden.json" "${WORKDIR}/"|| exit 1
cp "${RECO_DIR}/Cert_Collisions2023_366442_370790_Golden.json" "${WORKDIR}/"|| exit 1
cp "${RECO_DIR}/Cert_Collisions2024_378981_386951_Golden.json" "${WORKDIR}/"|| exit 1

# ------------------------------------------------------------
# 5) 이동
# ------------------------------------------------------------
cd "${WORKDIR}" || {
    echo "[ERROR] cd to WORKDIR failed" >&2
    exit 1
}

# ------------------------------------------------------------
# 6) 출력 디렉토리 (repo 기준)
# ------------------------------------------------------------
BASE_OUT="/u/user/sucho/SE_UserHome/reco_TEST/"
echo "[INFO] BASE_OUT = ${BASE_OUT}"

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

echo "[INFO] GOLDEN_JSON = ${GOLDEN_JSON}" 

# ------------------------------------------------------------
# 8) 여기에서만 ACLiC 컴파일 + 실행 (잡마다 다른 디렉토리이므로 충돌 X)
# sampleType 은 지금 "data" 로 고정 (MC 돌릴 땐 "mc" 로 submit 파일에서 바꾸면 됨)
# ------------------------------------------------------------
root -b -q "reco_single.C+(\"data\", \"${INPUT_FILE}\", \"${BASE_OUT}\", \"${GOLDEN_JSON}\")"
RET=$?

echo "[INFO] ROOT exited with code: ${RET}"
date

# (원하면 cleanup)
# rm -rf "${WORKDIR}"

exit ${RET}

