#!/bin/bash
# run_cutflow.sh

INPUT_FILE="$1"
CHANNEL="$2"      # ele or mu
SAMPLETYPE="$3"   # data or mc
OUTDIR="$4"       # <-- submit에서 받기

if [ -z "$INPUT_FILE" ] || [ -z "$CHANNEL" ] || [ -z "$SAMPLETYPE" ] || [ -z "$OUTDIR" ]; then
  echo "[ERROR] Usage: $0 <input.root> <ele|mu> <data|mc> <outdir>" >&2
  exit 1
fi

echo "[INFO] Starting cutflow for: ${INPUT_FILE}"
echo "[INFO] Channel=${CHANNEL} SampleType=${SAMPLETYPE}"
date

# ROOT environment (same as reco)
source /cvmfs/sft.cern.ch/lcg/releases/LCG_106a/ROOT/6.32.06/x86_64-el9-gcc13-opt/ROOT-env.sh

CID=${ClusterId:-local}
PID=${ProcId:-$$}
WORKDIR="/tmp/${USER}/cutflow_${CID}_${PID}"

echo "[INFO] WORKDIR = ${WORKDIR}"
mkdir -p "${WORKDIR}" || exit 1

# your cutflow source dir
SRCDIR="/u/user/sucho/cms_open_data_root/sucho_analysis/cutflow"
cp "${SRCDIR}/cutflow_single.C" "${WORKDIR}/" || exit 1
cp -r "${SRCDIR}/include" "${WORKDIR}/" || exit 1
cp "${SRCDIR}/GoodLumi.h" "${WORKDIR}/" || exit 1
cp "${SRCDIR}/PhotonSCeta.h" "${WORKDIR}/" || exit 1
cp "${SRCDIR}/json.hpp" "${WORKDIR}/" || exit 1
cp "${SRCDIR}/Cert_Collisions2022_355100_362760_Golden.json" "${WORKDIR}/" || exit 1
cp "${SRCDIR}/Cert_Collisions2023_366442_370790_Golden.json" "${WORKDIR}/" || exit 1
cp "${SRCDIR}/Cert_Collisions2024_378981_386951_Golden.json" "${WORKDIR}/" || exit 1

cd "${WORKDIR}" || exit 1

# results directory on shared filesystem (IMPORTANT)
# -> condor worker local(/tmp)에 저장하면 잡 끝나면 사라질 수 있어서,
#    /u/user/... 처럼 "항상 보이는 곳"으로 저장하는게 안전.
# OUTDIR="/u/user/sucho/cms_open_data_root/sucho_analysis/cutflow/results_2023_EGamma_NanoAODv15"

echo "[INFO] OUTDIR = ${OUTDIR}"
mkdir -p "${OUTDIR}"

# 7) 연도에 따라 JSON 파일명 선택.
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

# run
root -b -q "cutflow_single.C(\"${SAMPLETYPE}\", \"${INPUT_FILE}\", \"${CHANNEL}\", \"${GOLDEN_JSON}\", \"${OUTDIR}\")"
RET=$?

echo "[INFO] ROOT exited with code: ${RET}"
date
exit ${RET}

