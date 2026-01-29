#!/bin/bash

# 1) 입력 ROOT 파일 (slimmed 파일)
INPUT_FILE="$1"

if [ -z "$INPUT_FILE" ]; then
    echo "[ERROR] No input file given" >&2
    exit 1
fi

echo "[INFO] Starting reco for: ${INPUT_FILE}"
date

# 2) ROOT 환경 세팅 (slim 때 쓰던 거 그대로)
source /cvmfs/sft.cern.ch/lcg/releases/LCG_106a/ROOT/6.32.06/x86_64-el9-gcc13-opt/ROOT-env.sh

# 3) Condor 잡마다 고유한 작업 디렉토리 (/tmp 밑에)
CID=${ClusterId:-local}
PID=${ProcId:-$$}
WORKDIR="/tmp/${USER}/reco_${CID}_${PID}"

echo "[INFO] WORKDIR = ${WORKDIR}"
mkdir -p "${WORKDIR}" || {
    echo "[ERROR] Failed to create WORKDIR ${WORKDIR}" >&2
    exit 1
}

# 4) reco_single_muon.C 와 필요한 헤더를 작업 디렉토리로 복사
SRCDIR="/u/user/sucho/cms_open_data_root/sucho_analysis/reco"

cp "${SRCDIR}/reco_single_muon.C" "${WORKDIR}/" || {
    echo "[ERROR] Failed to copy reco_single_muon.C" >&2
    exit 1
}

# MakeSkimOutputPath.h 를 reco_single_muon.C 에서 #include "MakeSkimOutputPath.h" 하고 있으니까 같이 복사
cp "${SRCDIR}/MakeSkimOutputPath.h" "${WORKDIR}/" || {
    echo "[ERROR] Failed to copy MakeSkimOutputPath.h" >&2
    exit 1
}

# (다른 헤더들이 필요하면 여기서 추가로 cp 하면 됨)
cp "${SRCDIR}/GoodLumi.h" "${WORKDIR}/"|| {
    echo "[ERROR] Failed to copy GoodLumi.h" >&2
    exit 1
}

# (다른 헤더들이 필요하면 여기서 추가로 cp 하면 됨)
cp "${SRCDIR}/json.hpp" "${WORKDIR}/"|| {
    echo "[ERROR] Failed to copy json.hpp" >&2
    exit 1
}

# (다른 헤더들이 필요하면 여기서 추가로 cp 하면 됨)
cp "${SRCDIR}/Cert_Collisions2023_366442_370790_Golden.json" "${WORKDIR}/"|| {
    echo "[ERROR] Failed to copy Cert_Collisions2023_366442_370790_Golden.json" >&2
    exit 1
}

# (다른 헤더들이 필요하면 여기서 추가로 cp 하면 됨)
cp "${SRCDIR}/Cert_Collisions2024_378981_386951_Golden.json" "${WORKDIR}/"|| {
    echo "[ERROR] Failed to copy Cert_Collisions2024_378981_386951_Golden.json" >&2
    exit 1
}

# (다른 헤더들이 필요하면 여기서 추가로 cp 하면 됨)
# cp "${SRCDIR}/SomeOtherHeader.h" "${WORKDIR}/"

# 5) 작업 디렉토리로 이동
cd "${WORKDIR}" || {
    echo "[ERROR] cd to WORKDIR failed" >&2
    exit 1
}

# 6) 출력 base 디렉토리
BASE_OUT="/u/user/sucho/SE_UserHome/reco"
echo "[INFO] BASE_OUT = ${BASE_OUT}"

# 7) 연도에 따라 JSON 파일명 선택.
GOLDEN_JSON=""
if [[ "${INPUT_FILE}" == *"Run2023"* ]]; then
  GOLDEN_JSON="Cert_Collisions2023_366442_370790_Golden.json"
elif [[ "${INPUT_FILE}" == *"Run2024"* ]]; then
  GOLDEN_JSON="Cert_Collisions2024_378981_386951_Golden.json"
else
  echo "[ERROR] Cannot determine year (Run2023/Run2024) from INPUT_FILE: ${INPUT_FILE}" >&2
  exit 1
fi

echo "[INFO] GOLDEN_JSON = ${GOLDEN_JSON}"

# 7) 여기에서만 ACLiC 컴파일 + 실행 (잡마다 다른 디렉토리이므로 충돌 X)
# sampleType 은 지금 "data" 로 고정 (MC 돌릴 땐 "mc" 로 submit 파일에서 바꾸면 됨)
root -b -q "reco_single_muon.C+(\"data\", \"${INPUT_FILE}\", \"${BASE_OUT}\", \"${GOLDEN_JSON}\")"
RET=$?

echo "[INFO] ROOT exited with code: ${RET}"
date

# (원하면 cleanup)
# rm -rf "${WORKDIR}"

exit ${RET}

