
How to run this "slim" directory.
You may slim ALL era. And then select you want in slimmed data.
======= Step 0. List up ==========
# 2023
## NanoAODv12
find /pnfs/knu.ac.kr/data/cms/store/data/Run2023*/Muon*/NANOAOD/22Sep2023* -type f -name "*root" > list_pnfs_2023_Muon_NanoAODv12.txt
find /pnfs/knu.ac.kr/data/cms/store/data/Run2023*/EGamma*/NANOAOD/22Sep2023* -type f -name "*root" > list_pnfs_2023_EGamma_NanoAODv12.txt
## NanoAODv15
find /pnfs/knu.ac.kr/data/cms/store/data/Run2023*/EGamma*/NANOAOD/NanoAODv15* -type f -name "*.root" > list_pnfs_2023_EGamma_NanoAODv15.txt
find /pnfs/knu.ac.kr/data/cms/store/data/Run2023*/Muon*/NANOAOD/NanoAODv15* -type f -name "*.root" > list_pnfs_2023_Muon_NanoAODv15.txt

# 2024
find /pnfs/knu.ac.kr/data/cms/store/data/Run2024*/EGamma*/NANOAOD/PromptReco* -type f -name "*.root" > list_pnfs_2024_EGamma_PromptReco.txt
find /pnfs/knu.ac.kr/data/cms/store/data/Run2024*/Muon*/NANOAOD/PromptReco* -type f -name "*.root" > list_pnfs_2024_Muon_PromptReco.txt

# draw_pico Comparison. 22Sep2023 corresponds to NanoAODv12.
# https://github.com/sungbeom0324/nano2pico/blob/4813277b90b5d01206f4b04f695310a8aa22f335/txt/datasets/NanoAODv12_htozgamma_2023_data_dataset_paths#L4

1. Copy .sub and change input-file-list.txt
2. Branches are defined in slim_single.C
3. Throw jobs : condor_submit XXX.sub

# Example
970 805 859 838 10461 8678

[sucho@cms02 slim]$ condor_submit submit_slim_2023_EGamma_NanoAODv12.sub
Submitting job(s)
970 job(s) submitted to cluster 779176.
[sucho@cms02 slim]$ condor_submit submit_slim_2023_Muon_NanoAODv12.sub
Submitting job(s)
859 job(s) submitted to cluster 779177.
[sucho@cms02 slim]$ condor_submit submit_slim_2023_EGamma_NanoAODv15.sub
Submitting job(s)
805 job(s) submitted to cluster 779178.
[sucho@cms02 slim]$ condor_submit submit_slim_2023_Muon_NanoAODv15.sub
Submitting job(s)
838 job(s) submitted to cluster 779179.

