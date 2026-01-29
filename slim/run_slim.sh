#!/bin/bash

# 1) 입력 ROOT 파일
INPUT_FILE="$1"

if [ -z "$INPUT_FILE" ]; then
    echo "[ERROR] No input file given" >&2
    exit 1
fi

echo "[INFO] Starting slim for: ${INPUT_FILE}"
date

# 2) ROOT 환경 세팅
source /cvmfs/sft.cern.ch/lcg/releases/LCG_106a/ROOT/6.32.06/x86_64-el9-gcc13-opt/ROOT-env.sh

# 3) Condor 잡마다 고유한 작업 디렉토리 (/tmp 밑에)
CID=${ClusterId:-local}
PID=${ProcId:-$$}
WORKDIR="/tmp/${USER}/slim_${CID}_${PID}"

echo "[INFO] WORKDIR = ${WORKDIR}"
mkdir -p "${WORKDIR}" || {
    echo "[ERROR] Failed to create WORKDIR ${WORKDIR}" >&2
    exit 1
}

# 4) slim_single.C 를 작업 디렉토리로 복사
SRCDIR="/u/user/sucho/cms_open_data_root/sucho_analysis/slim"
cp "${SRCDIR}/slim_single.C" "${WORKDIR}/" || {
    echo "[ERROR] Failed to copy slim_single.C" >&2
    exit 1
}

# 5) 작업 디렉토리로 이동
cd "${WORKDIR}" || {
    echo "[ERROR] cd to WORKDIR failed" >&2
    exit 1
}

# 6) 출력 base 디렉토리
BASE_OUT="/u/user/sucho/SE_UserHome/slimmed"
echo "[INFO] BASE_OUT = ${BASE_OUT}"

# 7) 여기에서만 ACLiC 컴파일 + 실행 (잡마다 다른 디렉토리이므로 충돌 X)
root -b -q "slim_single.C+(\"${INPUT_FILE}\", \"${BASE_OUT}\")"
RET=$?

echo "[INFO] ROOT exited with code: ${RET}"
date

# (원하면 cleanup)
# rm -rf "${WORKDIR}"

exit ${RET}
