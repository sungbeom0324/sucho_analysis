Docummentation for reco. 2025.11.25
________________________________________________________________________________________

<How to single run>
root -b -q 'reco_single.C+("mc/data", "input", "outDir")'

e.g. 
root -b -q 'reco_single.C+("data", "/u/user/sucho/SE_UserHome/slimmed/data/Run2023B/EGamma0/NANOAOD/NanoAODv15-v1/140000/3cc96889-3859-4ede-b544-dd3421592017_slimmed.root", "/u/user/sucho/cms_open_data_root/sucho_analysis/reco")'
root -b -q 'reco_single_muon.C+("data", "/u/user/sucho/SE_UserHome/slimmed/data/Run2023B/Muon0/NANOAOD/NanoAODv15-v1/100000/84e9eae3-dfe8-4297-854d-846e7d446c24_slimmed.root", "/u/user/sucho/cms_open_data_root/sucho_analysis/reco")'

--------------------------------------------------------------------------------------------

<How to batch run>
Step 1. Define event selection in reco_single.C
Step 2. Set run command options (data/mc, json_o/x) in run_reco.sh
Step 3. Copy -> yourSubmission.sub and set input file list.txt for queue.
Step 4. $ condor_submit submit yourSubmission.sub

--------------------------------------------------------------------------------------------

[reco_single.C]
Main source code.

[MakeSkimOutputPath.h]
Reserves output directory consistent with input.
e.g.
input : 
/pnfs/knu.ac.kr/data/cms/store/user/sucho/cms/store/mc/Run3Summer23BPixNanoAODv12/DYJetsToLL_M-50_TuneCP5_13p6TeV-madgraphMLM-pythia8/NANOAODSIM/Pilot_130X_mcRun3_2023_realistic_postBPix_v2-v3/2540000/97657669-9e19-46cf-bc5c-b132f23269cd.root
output : 
outDir + /mc/Run3Summer23BPixNanoAODv12/DYJetsToLL_M-50_TuneCP5_13p6TeV-madgraphMLM-pythia8/NANOAODSIM/Pilot_130X_mcRun3_2023_realistic_postBPix_v2-v3/2540000/97657669-9e19-46cf-bc5c-b132f23269cd.root

[GoodLumi.h]
Check if the specific event pass the goldenJson. 

[json.hpp] External json library.
wget https://github.com/nlohmann/json/releases/latest/download/json.hpp
