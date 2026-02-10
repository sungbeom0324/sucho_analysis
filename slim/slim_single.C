// 파일 이름: slim_single.C

#include "TFile.h"
#include "TTree.h"
#include "TSystem.h"
#include <string>
#include <iostream>

std::string MakeSlimOutputPath(const char* inFileName,
                               const char* baseOutDir = "/u/user/sucho/SE_UserHome/slimmed")
{
    std::string in(inFileName);
    std::string base(baseOutDir);

    // Parsing inpput file path after final "data/". See e.g. below.
    // input = /u/user/sucho/SE_UserHome/cms/store/data/Run2024D/EGamma0/NANOAOD/PromptReco-v1/000/380/567/00000/d6d3e72d-78f6-4c37-a19f-3e8686f748c9.root
    // relPath = data/Run2024D/EGamma0/...
    const std::string key = "data/";
    size_t pos = in.rfind(key);
    std::string relPath;

    if (pos != std::string::npos) {
        relPath = in.substr(pos);   // e.g.: "data/Run2024D/EGamma0/..."
    } else {
        // If you don't find "/data", just use whole path.
        relPath = gSystem->BaseName(inFileName);
    }

    // relPath -> 디렉토리 + 파일 이름 분리
    std::string tmp = "/" + relPath;                 // 절대경로처럼 보이게
    std::string relDir  = gSystem->DirName(tmp.c_str());   // "/data/Run2024D/..."
    std::string relFile = gSystem->BaseName(tmp.c_str());  // "xxxxx.root"

    if (!relDir.empty() && relDir[0] == '/') relDir.erase(0,1); // 앞 '/' 제거 → "data/Run2024D/..."

    // 3) 출력 디렉토리 전체 경로
    std::string outDir = base + "/" + relDir;
    gSystem->mkdir(outDir.c_str(), true);

    // 4) 파일 이름에 "_slimmed" 삽입
    const std::string ext = ".root";
    size_t dot = relFile.rfind(ext);
    if (dot != std::string::npos) {
        relFile.insert(dot, "_slimmed");
    } else {
        relFile += "_slimmed";
    }

    // 5) 최종 출력 전체 경로
    return outDir + "/" + relFile;
}


void slim_single(const char* inFileName,
                         const char* baseOutDir = "/u/user/sucho/SE_UserHome/slimmed")
{
    // 0) 출력 경로 구성
    std::string outFileName = MakeSlimOutputPath(inFileName, baseOutDir);
    std::cout << "[INFO] Output file = " << outFileName << std::endl;

    // 1) 입력 파일 열기
    TFile *fin = TFile::Open(inFileName, "READ");
    if (!fin || fin->IsZombie()) {
        std::cerr << "[ERROR] Cannot open input file: " << inFileName << std::endl;
        return;
    }

    // 2) 트리 가져오기
    TTree *tin = dynamic_cast<TTree*>(fin->Get("Events"));
    if (!tin) {
        std::cerr << "[ERROR] Cannot find TTree 'Events' in file: " << inFileName << std::endl;
        fin->Close();
        return;
    }

    std::cout << "[INFO] Input entries: " << tin->GetEntries() << std::endl;

    // 3) 브랜치 슬림 설정
    tin->SetBranchStatus("*", 0);
    // 필요하면 추가:
    tin->SetBranchStatus("event", 1);
    tin->SetBranchStatus("run", 1);
    tin->SetBranchStatus("luminosityBlock", 1);

    // PV
    tin->SetBranchStatus("PV_npvs", 1);
    tin->SetBranchStatus("PV_npvsGood", 1);
    tin->SetBranchStatus("PV_x", 1);
    tin->SetBranchStatus("PV_y", 1);
    tin->SetBranchStatus("PV_z", 1);

    // HLT (2023_2024_EGamma/Muon_NanoAODv15)
    tin->SetBranchStatus("HLT_Ele32_WPTight_Gsf", 1); // 31page Table 11.
    tin->SetBranchStatus("HLT_Ele30_WPTight_Gsf", 1); // 31page Table 11.
    tin->SetBranchStatus("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL", 1); // 31page Table 11.
    tin->SetBranchStatus("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ", 1); // 31page Table 11.
    tin->SetBranchStatus("HLT_IsoMu24", 1); // 31page Table 11.
    tin->SetBranchStatus("HLT_IsoMu27", 1); // 31page Table 11.
    tin->SetBranchStatus("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL", 1); // 31page Table 11.
    tin->SetBranchStatus("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ", 1); // 31page Table 11.
    tin->SetBranchStatus("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8", 1); // 31page Table 11.
    tin->SetBranchStatus("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8", 1); // 31page Table 11.
    // HLT (2024_Additional?_EGamma/Muon_PromptReco)
    tin->SetBranchStatus("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8_PFJet30", 1); // ---- What is this? -----

    // Event filters [JetMET POG] 23page, Section 3.2
    tin->SetBranchStatus("Flag_goodVertices", 1);
    tin->SetBranchStatus("Flag_globalSuperTightHalo2016Filter", 1);
    tin->SetBranchStatus("Flag_HBHENoiseFilter", 1);
    tin->SetBranchStatus("Flag_HBHENoiseIsoFilter", 1);
    tin->SetBranchStatus("Flag_EcalDeadCellTriggerPrimitiveFilter", 1);
    tin->SetBranchStatus("Flag_BadPFMuonFilter", 1);
    tin->SetBranchStatus("Flag_BadPFMuonDzFilter", 1);
    tin->SetBranchStatus("Flag_hfNoisyHitsFilter", 1);
    tin->SetBranchStatus("Flag_eeBadScFilter", 1);
    tin->SetBranchStatus("Flag_ecalBadCalibFilter", 1);

    tin->SetBranchStatus("nElectron", 1);
    tin->SetBranchStatus("Electron_charge", 1);
    tin->SetBranchStatus("Electron_dz", 1); // New Dec06 (AN-22-027_v4.pdf_3.1.1 Electrons)
    tin->SetBranchStatus("Electron_dzErr", 1); // New Dec06 (AN-22-027_v4.pdf_3.1.1 Electrons)
    tin->SetBranchStatus("Electron_dxy", 1); // New Dec06 (AN-22-027_v4.pdf_3.1.1 Electrons)
    tin->SetBranchStatus("Electron_dxyErr", 1); // New Dec06 (AN-22-027_v4.pdf_3.1.1 Electrons)
    tin->SetBranchStatus("Electron_pt", 1);
    tin->SetBranchStatus("Electron_eta", 1);
    tin->SetBranchStatus("Electron_phi", 1);
    tin->SetBranchStatus("Electron_mass", 1);
    tin->SetBranchStatus("Electron_cutBased", 1);
    tin->SetBranchStatus("Electron_mvaIso_WP80", 1);
    tin->SetBranchStatus("Electron_mvaIso_WP90", 1);
    tin->SetBranchStatus("Electron_mvaHZZIso", 1); // New Dec06 (AN-22-027_v4.pdf_3.1.1 Electrons)
    tin->SetBranchStatus("Electron_miniPFRelIso_all", 1); // New Dec06 (AN-22-027_v4.pdf_3.1.1 Electrons)
    tin->SetBranchStatus("Electron_photonIdx", 1);
    tin->SetBranchStatus("Electron_fsrPhotonIdx", 1);
    tin->SetBranchStatus("Electron_deltaEtaSC", 1); // New Dec06 (AN-22-027_v4.pdf_3.1.1 Electrons)

    tin->SetBranchStatus("nPhoton", 1);
    tin->SetBranchStatus("Photon_pt", 1);
    tin->SetBranchStatus("Photon_eta", 1);
    tin->SetBranchStatus("Photon_phi", 1);
    tin->SetBranchStatus("Photon_pt", 1);
    tin->SetBranchStatus("Photon_cutBased", 1);
    tin->SetBranchStatus("Photon_mvaID_WP80", 1);
    tin->SetBranchStatus("Photon_mvaID_WP90", 1);
    tin->SetBranchStatus("Photon_electronIdx", 1);
    tin->SetBranchStatus("Photon_superclusterEta", 1); // New Dec06 (AN-22-027_v4.pdf_3.1.3)
    tin->SetBranchStatus("Photon_isScEtaEB", 1); // New Dec06 (AN-22-027_v4.pdf_3.1.3)
    tin->SetBranchStatus("Photon_isScEtaEE", 1); // New Dec06 (AN-22-027_v4.pdf_3.1.3)

    tin->SetBranchStatus("nFsrPhoton", 1); // New Dec06 (AN-22-027_v4.pdf_3.1.4 FsrPhotons)
    tin->SetBranchStatus("FsrPhoton_electronIdx", 1); // New Dec06 (AN-22-027_v4.pdf_3.1.4 FsrPhotons)
    tin->SetBranchStatus("FsrPhoton_muonIdx", 1); // New Dec06 (AN-22-027_v4.pdf_3.1.4 FsrPhotons)
    tin->SetBranchStatus("FsrPhoton_pt", 1); // New Dec06 (AN-22-027_v4.pdf_3.1.4 FsrPhotons)
    tin->SetBranchStatus("FsrPhoton_eta", 1); // New Dec06 (AN-22-027_v4.pdf_3.1.4 FsrPhotons)
    tin->SetBranchStatus("FsrPhoton_phi", 1); // New Dec06 (AN-22-027_v4.pdf_3.1.4 FsrPhotons)
    tin->SetBranchStatus("FsrPhoton_relIso03", 1); // New Dec06 (AN-22-027_v4.pdf_3.1.4 FsrPhotons)
    tin->SetBranchStatus("FsrPhoton_dROverEt2", 1); // New Dec06 (AN-22-027_v4.pdf_3.1.4 FsrPhotons)


    tin->SetBranchStatus("nMuon", 1);
    tin->SetBranchStatus("Muon_dz", 1); // New Dec06 (AN-22-027_v4.pdf_3.1.2)
    tin->SetBranchStatus("Muon_dzErr", 1); // New Dec06 (AN-22-027_v4.pdf_3.1.2)
    tin->SetBranchStatus("Muon_dxy", 1); // New Dec06 (AN-22-027_v4.pdf_3.1.2)
    tin->SetBranchStatus("Muon_dxyErr", 1); // New Dec06 (AN-22-027_v4.pdf_3.1.2)
    tin->SetBranchStatus("Muon_sip3d", 1); // New Dec06 (AN-22-027_v4.pdf_3.1.2)
    tin->SetBranchStatus("Muon_charge", 1);
    tin->SetBranchStatus("Muon_pt", 1);
    tin->SetBranchStatus("Muon_eta", 1);
    tin->SetBranchStatus("Muon_phi", 1);
    tin->SetBranchStatus("Muon_mass", 1);
    tin->SetBranchStatus("Muon_isGlobal", 1);
    tin->SetBranchStatus("Muon_looseId", 1);
    tin->SetBranchStatus("Muon_highPtId", 1);
    tin->SetBranchStatus("Muon_mediumId", 1);
    tin->SetBranchStatus("Muon_tightId", 1);
    tin->SetBranchStatus("Muon_fsrPhotonIdx", 1);
    tin->SetBranchStatus("Muon_pfRelIso03_all", 1);
    tin->SetBranchStatus("Muon_pfRelIso04_all", 1);
    tin->SetBranchStatus("Muon_isTracker", 1); // New Dec06 (AN-22-027_v4.pdf_3.1.2)
    tin->SetBranchStatus("Muon_triggerIdLoose", 1); // New Dec06 (AN-22-027_v4.pdf_3.1.2) central cut-based looseID?
    tin->SetBranchStatus("Muon_tkRelIso", 1); // New Dec06 (AN-22-027_v4.pdf_3.1.2)
    tin->SetBranchStatus("Muon_miniPFRelIso_all", 1); // New Dec06 (AN-22-027_v4.pdf_3.1.2)

    // 4) 출력 파일 생성
    TFile *fout = TFile::Open(outFileName.c_str(), "RECREATE");
    if (!fout || fout->IsZombie()) {
        std::cerr << "[ERROR] Cannot create output file: " << outFileName << std::endl;
        fin->Close();
        return;
    }

    // 5) 활성 브랜치만 모든 엔트리 복사
    TTree *tout = tin->CloneTree(-1, "fast");
    if (!tout) {
        std::cerr << "[ERROR] CloneTree failed" << std::endl;
        fout->Close();
        fin->Close();
        return;
    }

    std::cout << "[INFO] Output entries: " << tout->GetEntries() << std::endl;

    // 6) 저장
    fout->cd();
    tout->Write();
    fout->Close();
    fin->Close();

    std::cout << "[INFO] Done." << std::endl;
}

