sucho_analysis  
==============

Compact analysis tool for preocessing NanoAOD.  
Author : Sungbeom Cho @ Korea University  
Email : sucho@cern.ch

---  
- Primarily written for fast H → Zγ study with recent Run3 data.  
- Less factorizations for flexibility.  
- HTCondor scripts are included for batch processing.
---

## How to Configure
```bash
git clone git@github.com:sungbeom0324/sucho_analysis.git
cd sucho_analysis
source set_env.sh

# Download external dependency file.
mkdir -p external/nlohmann
curl -L https://raw.githubusercontent.com/nlohmann/json/v3.12.0/single_include/nlohmann/json.hpp \
     -o external/nlohmann/json.hpp

# Download Golden Json files for your run era and locate them in json directory.
# https://cms-service-dqmdc.web.cern.ch/CAF/certification/ 
mkdir json

cd $ANALYSIS_BASE | ls
# Then, your working directory should look like :
cutflow external include json plot README.md reco set_env.sh slim

# Finally, define your output directory by modifing run_XXX.sh and submit_XXX.sub template files!
```

___
## How to Run
### slim
Input : A list of root files in txt, specified at the last line of submit_XXX.sub  
Output : Root files with only branches of your interest.
```bash
cd $SLIM_DIR
mkdir logs
condor_submit submit_slim_2022EFG_EGamma_NanoAODv12_TEST.sub

# monitor
condor_q
# cancel Job
condor_rm <JobID>
```

### reco
Input : A list of root files in txt, specified at the last line of submit_XXX.sub  
Output : Root files with events passing certain criteria and including user-reconstructed variables, such as Z_mass.
```bash
cd $RECO_DIR
mkdir logs
condor_submit submit_reco_2022EFG_EGamma_NanoAODv12_TEST.sub
```

### cutflow
Input : A list of root files in txt, specified at the last line of submit_XXX.sub  
Output : cutflow.txt for each root files. $\rightarrow$ You can merge them with merge_cutflow.sh
```bash
cd $CUTFLOW_DIR
mkdir logs
condor_submit submit_cutflow_2022EFG_EGamma_NanoAODv12.sub

# Job completed, then merge the output.
./merge_cutflow.sh result_2022EFG_EGamma_NanoAODv12 cutflow_result_2022EFG_EGamma_NanoAODv12.txt
```

### plot
Input : list_reco.txt  
Output : plots/ directory containing multi-page pdf.
```bash
cd plot
# Edit palette_all.C 
root -l -q 'palette_all.C("plots_out")'
```

___
