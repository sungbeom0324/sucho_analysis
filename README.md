---
## sucho_analysis  
Compact analysis tool for preocessing NanoAOD format data.  
Author : Sungbeom Cho (Korea Univ.)  
  
- Primarily written for fast H → Zγ study with recent Run3 data.  
- Less factorizations for rapid application and readability.  
- HTCondor scripts are included for batch processing.

NOTE : To be updated to use relative paths, along with a cleanup of external dependency files.  

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
