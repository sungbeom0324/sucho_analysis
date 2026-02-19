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

### How to Configure
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
### slim
Input : list.txt of .root files in your store.  
Output : Slimmed .root files in the same hierarchy as store.

### reco
Input : list_slimmed.txt (list of slimmed files).  
Output : Reconstruct objects (eg. Z_mass_ee) and do skimming.

### cutflow
Input : list_reco.txt (list of reco-done files).  
Output : cutflow.txt for each root files. $\rightarrow$ You can merge them with merge_cutflow.sh

### plot
Input : list_reco.txt  
Output : plots/ directory containing multi-page pdf.
___
