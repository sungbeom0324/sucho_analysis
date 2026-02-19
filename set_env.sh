#!/usr/bin/env bash
# usage: source set_env.sh

# Setting environments before running.
# This defines input / output path of every jobs.

export ANALYSIS_BASE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export SLIM_DIR="${ANALYSIS_BASE}/slim"
export RECO_DIR="${ANALYSIS_BASE}/reco"
export CUTFLOW_DIR="${ANALYSIS_BASE}/cutflow"

echo
echo "[ENV] ANALYSIS_BASE = ${ANALYSIS_BASE}"
echo "[ENV] SLIM_DIR = ${SLIM_DIR}"
echo "[ENV] RECO_DIR = ${RECO_DIR}"
echo "[ENV] CUTFLOW_DIR = ${CUTFLOW_DIR}"
echo
echo "Last Remaining Config Step : "
echo "Please modify your ROOT and OUTPUT_PATH in run_slim.sh, run_skim.sh, run_skim_muon.sh before job submission."
echo
