
How to run this "slim" directory.
You may slim ALL era. And then select you want in slimmed data.
======= Step 0. List up ==========
find /pnfs/knu.ac.kr/data/cms/store/data/Run2023*/EGamma*/NANOAOD/NanoAODv15* -type f -name "*.root" > list_pnfs_2023_EGamma_NanoAODv15.txt
find /pnfs/knu.ac.kr/data/cms/store/data/Run2023*/Muon*/NANOAOD/NanoAODv15* -type f -name "*.root" > list_pnfs_2023_Muon_NanoAODv15.txt
find /pnfs/knu.ac.kr/data/cms/store/data/Run2024*/EGamma*/NANOAOD/PromptReco* -type f -name "*.root" > list_pnfs_2024_EGamma_PromptReco.txt
find /pnfs/knu.ac.kr/data/cms/store/data/Run2024*/Muon*/NANOAOD/PromptReco* -type f -name "*.root" > list_pnfs_2024_Muon_PromptReco.txt

find /pnfs/knu.ac.kr/data/cms/store/data/Run2023*/Muon*/NANOAOD/22Sep2023* -type f -name "*root" > list_pnfs_2023_Muon_NanoAODv12.txt
find /pnfs/knu.ac.kr/data/cms/store/data/Run2023*/EGamma*/NANOAOD/22Sep2023* -type f -name "*root" > list_pnfs_2023_EGamma_NanoAODv12.txt
find /pnfs/knu.ac.kr/data/cms/store/data/Run2023D/EGamma*/NANOAOD/22Sep2023* -type f -name "*root" > list_pnfs_2023D_EGamma_NanoAODv12.txt

# draw_pico Comparison. 22Sep2023 corresponds to NanoAODv12.
# https://github.com/sungbeom0324/nano2pico/blob/4813277b90b5d01206f4b04f695310a8aa22f335/txt/datasets/NanoAODv12_htozgamma_2023_data_dataset_paths#L4


1. Copy .sub and change input-file-list.txt
2. Branches are defined in slim_single.C
3. Throw jobs : condor_submit XXX.sub

