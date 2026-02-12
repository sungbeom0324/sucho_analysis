#include "MakeSkimOutputPath.h"  // used in skim.
#include "GoodLumi.h" // GoldenJson
#include "PhotonSCeta.h" 
#include <TFile.h>
#include <TTree.h>
#include <TTreeFormula.h>
#include <TSystem.h>
#include <TLorentzVector.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <set>

// --- HZZ MVA WP helper (https://github.com/sungbeom0324/nano2pico/blob/master/src/el_producer.cpp) ---
static inline float ConvertMVA(float mva_mini) {
    // 2.0 / (1.0 + exp(-2.0 * response)) - 1
    return 2.0f / (1.0f + std::exp(-2.0f * mva_mini)) - 1.0f;
}

static inline bool HzzId_WP2022(float pt, float etasc, float hzzmvaid) {
    // 2022 WPs for 2018 ID training
    // thresholds are given in "mini" scale -> convert to nano scale via ConvertMVA
    const float aeta = std::fabs(etasc);

    if (pt < 10.0f) {
        if (aeta < 0.8f)         return (hzzmvaid > ConvertMVA(1.6339f));
        else if (aeta < 1.479f)  return (hzzmvaid > ConvertMVA(1.5499f));
        else                     return (hzzmvaid > ConvertMVA(2.0629f));
    } else {
        if (aeta < 0.8f)         return (hzzmvaid > ConvertMVA(0.3685f));
        else if (aeta < 1.479f)  return (hzzmvaid > ConvertMVA(0.2662f));
        else                     return (hzzmvaid > ConvertMVA(-0.5444f));
    }
}

void reco_single_muon(const char* sampleType,
                      const char* inFileName,
                      const char* baseOutDir = "/u/user/sucho/SE_UserHome/reco",
                      const char* goldenJsonFile="")
{
    // --- 1) 출력 경로 만들기 (skim_single 과 동일한 방식) ---
    std::string outFileName = MakeSkimOutputPath(sampleType, inFileName, baseOutDir);

    std::cout << "[INFO] Input  file = " << inFileName  << std::endl;
    std::cout << "[INFO] Output file = " << outFileName << std::endl;

    // --- 2) 입력 파일 / 트리 열기 ---
    TFile *fin = TFile::Open(inFileName, "READ");
    if (!fin || fin->IsZombie()) {
        std::cerr << "[ERROR] Cannot open input file: " << inFileName << std::endl;
        return;
    }

    TTree *tin = dynamic_cast<TTree*>(fin->Get("Events"));
    if (!tin) {
        std::cerr << "[ERROR] Cannot find TTree 'Events' in file: "
                  << inFileName << std::endl;
        fin->Close();
        return;
    }

    Long64_t nAll = tin->GetEntries();
    std::cout << "[INFO] Input entries : " << nAll << std::endl;

    // --- 3) 출력 파일 / 트리 준비 (기존 브랜치 구조 복사) ---
    TFile *fout = TFile::Open(outFileName.c_str(), "RECREATE");
    if (!fout || fout->IsZombie()) {
        std::cerr << "[ERROR] Cannot create output file: "
                  << outFileName << std::endl;
        fin->Close();
        return;
    }

    TTree *tout = tin->CloneTree(0);  // 브랜치 구조만 복사, 엔트리는 0
    if (!tout) {
        std::cerr << "[ERROR] CloneTree(0) failed" << std::endl;
        fout->Close();
        fin->Close();
        return;
    }

    // --- 4) 새 브랜치 추가 (Z_ee, Z_eeg) ---
    Float_t Z_mass_mm  = -1.0;  // 선택된 OS ee 페어의 mass (없으면 -1)
    Float_t Z_mass_mmg = -1.0;  // photon 드레싱 후 mass (없으면 Z_ee, 그마저 없으면 -1)
    Float_t AddPhoton_pt = -1.0;
    Float_t AddPhoton_eta = -99.0;
    Float_t dR_mg = -999; 
    std::vector<float> Photon_SCeta;
    Int_t leadEleIdx = -1, subleadEleIdx = -1;
    Int_t leadMuonIdx = -1, subleadMuonIdx = -1;
    Int_t nGoodEle = 0;
    Int_t nGoodMuon = 0;
    Int_t nGoodPhoton = 0;

    tout->Branch("Z_mass_mm",  &Z_mass_mm,  "Z_mass_mm/F");
    tout->Branch("Z_mass_mmg", &Z_mass_mmg, "Z_mass_mmg/F");
    tout->Branch("AddPhoton_pt", &AddPhoton_pt, "AddPhoton_pt/F");
    tout->Branch("AddPhoton_eta", &AddPhoton_eta, "AddPhoton_eta/F");
    tout->Branch("dR_mg", &dR_mg, "dR_mg/F");
    tout->Branch("Photon_SCeta", &Photon_SCeta);
    tout->Branch("leadEleIdx", &leadEleIdx, "leadEleIdx/I");
    tout->Branch("subleadEleIdx", &subleadEleIdx, "subleadEleIdx/I");
    tout->Branch("leadMuonIdx", &leadMuonIdx, "leadMuonIdx/I");
    tout->Branch("subleadMuonIdx", &subleadMuonIdx, "subleadMuonIdx/I");
    tout->Branch("nGoodEle", &nGoodEle, "nGoodEle/I");
    tout->Branch("nGoodMuon", &nGoodMuon, "nGoodMuon/I");
    tout->Branch("nGoodPhoton", &nGoodPhoton, "nGoodPhoton/I");

    const double Z_MASS = 91.1876; // [GeV]
    // --- 5) 필요한 입력 브랜치 주소 ---
    static const int MAXP_64 = 64;
    static const int MAXP_128 = 128;    
    Bool_t HLT_Ele32_WPTight_Gsf;
    Bool_t HLT_Ele30_WPTight_Gsf;
    Bool_t HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL;
    Bool_t HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ;
    Bool_t HLT_IsoMu24;
    Bool_t HLT_IsoMu27;
    Bool_t HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL;
    Bool_t HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ;
    Bool_t HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8;
    Bool_t HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8; 

    Bool_t Flag_goodVertices;
    Bool_t Flag_globalSuperTightHalo2016Filter;
    Bool_t Flag_HBHENoiseFilter;     // Run2 Only? 22page Table 9.
    Bool_t Flag_HBHENoiseIsoFilter;  // Run2 Only? 22page Table 9.
    Bool_t Flag_EcalDeadCellTriggerPrimitiveFilter;
    Bool_t Flag_BadPFMuonFilter;
    Bool_t Flag_BadPFMuonDzFilter;
    Bool_t Flag_hfNoisyHitsFilter;
    Bool_t Flag_eeBadScFilter;
    Bool_t Flag_ecalBadCalibFilter;

    Int_t   nElectron = 0;
    Bool_t  Electron_mvaIso_WP80[MAXP_64];
    Int_t   Electron_charge[MAXP_64];
    Float_t Electron_pt[MAXP_64];
    Float_t Electron_eta[MAXP_64];
    Float_t Electron_phi[MAXP_64];
    Float_t Electron_mass[MAXP_64];
    Float_t Electron_dz[MAXP_64]; // New
    Float_t Electron_dzErr[MAXP_64]; // New
    Float_t Electron_dxy[MAXP_64]; // New
    Float_t Electron_dxyErr[MAXP_64]; // New
    Float_t Electron_mvaHZZIso[MAXP_64]; // New
    Float_t Electron_miniPFRelIso_all[MAXP_64]; // New
    Float_t Electron_deltaEtaSC[MAXP_64]; // New
    Short_t Electron_photonIdx[MAXP_64]; // New
    Short_t Electron_fsrPhotonIdx[MAXP_64]; // New

    Int_t nMuon = 0;
    Bool_t Muon_isTracker[MAXP_64];
    Bool_t Muon_looseId[MAXP_64];
    Float_t Muon_dz[MAXP_64];
    Float_t Muon_dxy[MAXP_64];
    Float_t Muon_sip3d[MAXP_64];
    Float_t Muon_pfRelIso03_all[MAXP_64];
    Float_t Muon_miniPFRelIso_all[MAXP_64];
    Float_t Muon_pt[MAXP_64];
    Float_t Muon_eta[MAXP_64];
    Float_t Muon_phi[MAXP_64];
    Float_t Muon_mass[MAXP_64]; 
    Int_t   Muon_charge[MAXP_64];

    Int_t   nPhoton = 0;
    Bool_t  Photon_mvaID_WP80[MAXP_128];
    Short_t Photon_electronIdx[MAXP_128];
    Float_t Photon_pt[MAXP_128];
    Float_t Photon_eta[MAXP_128];
    Float_t Photon_phi[MAXP_128];
    Float_t Photon_superclusterEta[MAXP_128]; // New
    Bool_t Photon_isScEtaEB[MAXP_128]; // New
    Bool_t Photon_isScEtaEE[MAXP_128]; // New
    UChar_t PV_npvsGood;
    Float_t PV_x = 0.0f, PV_y = 0.0f, PV_z = 0.0f; // NEW
   
    Int_t nFsrPhoton = 0;
    Short_t FsrPhoton_electronIdx[MAXP_128];
    Short_t FsrPhoton_muonIdx[MAXP_128];
    Float_t FsrPhoton_dROverEt2[MAXP_128];
    Float_t FsrPhoton_eta[MAXP_128];
    Float_t FsrPhoton_phi[MAXP_128];
    Float_t FsrPhoton_pt[MAXP_128];
    Float_t FsrPhoton_relIso03[MAXP_128];

    //tin->SetBranchAddress("", &);  // New  
    // HLT
    tin->SetBranchAddress("HLT_Ele32_WPTight_Gsf", &HLT_Ele32_WPTight_Gsf);
    tin->SetBranchAddress("HLT_Ele30_WPTight_Gsf", &HLT_Ele30_WPTight_Gsf);
    tin->SetBranchAddress("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL", &HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL);
    tin->SetBranchAddress("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ", &HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ);
    tin->SetBranchAddress("HLT_IsoMu24", &HLT_IsoMu24);
    tin->SetBranchAddress("HLT_IsoMu27", &HLT_IsoMu27);
    tin->SetBranchAddress("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL", &HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL);
    tin->SetBranchAddress("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ", &HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ);
    tin->SetBranchAddress("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8", &HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8);
    tin->SetBranchAddress("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8", &HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8);

    // Event Filter [JetMET] 23page, Section 3.2
    tin->SetBranchAddress("Flag_goodVertices", &Flag_goodVertices);
    tin->SetBranchAddress("Flag_globalSuperTightHalo2016Filter", &Flag_globalSuperTightHalo2016Filter);
    tin->SetBranchAddress("Flag_HBHENoiseFilter", &Flag_HBHENoiseFilter);
    tin->SetBranchAddress("Flag_HBHENoiseIsoFilter", &Flag_HBHENoiseIsoFilter);
    tin->SetBranchAddress("Flag_EcalDeadCellTriggerPrimitiveFilter", &Flag_EcalDeadCellTriggerPrimitiveFilter);
    tin->SetBranchAddress("Flag_BadPFMuonFilter", &Flag_BadPFMuonFilter);
    tin->SetBranchAddress("Flag_BadPFMuonDzFilter", &Flag_BadPFMuonDzFilter);
    tin->SetBranchAddress("Flag_hfNoisyHitsFilter", &Flag_hfNoisyHitsFilter);
    tin->SetBranchAddress("Flag_eeBadScFilter", &Flag_eeBadScFilter);
    tin->SetBranchAddress("Flag_ecalBadCalibFilter", &Flag_ecalBadCalibFilter);

    // Electron
    tin->SetBranchAddress("nElectron", &nElectron);
    tin->SetBranchAddress("Electron_mvaIso_WP80", &Electron_mvaIso_WP80);
    tin->SetBranchAddress("Electron_charge",       &Electron_charge);
    tin->SetBranchAddress("Electron_pt",          &Electron_pt);
    tin->SetBranchAddress("Electron_eta",         &Electron_eta);
    tin->SetBranchAddress("Electron_phi",         &Electron_phi);
    tin->SetBranchAddress("Electron_mass",        &Electron_mass);
    tin->SetBranchAddress("Electron_dz", &Electron_dz);    // New
    tin->SetBranchAddress("Electron_dzErr", &Electron_dzErr);   // New  
    tin->SetBranchAddress("Electron_dxy", &Electron_dxy);    // New
    tin->SetBranchAddress("Electron_dxyErr", &Electron_dxyErr);   // New 
    tin->SetBranchAddress("Electron_mvaHZZIso", &Electron_mvaHZZIso);  // New  
    tin->SetBranchAddress("Electron_miniPFRelIso_all", &Electron_miniPFRelIso_all);  // New  
    tin->SetBranchAddress("Electron_deltaEtaSC", &Electron_deltaEtaSC);  // New  
    tin->SetBranchAddress("Electron_photonIdx", &Electron_photonIdx);  // New  
    tin->SetBranchAddress("Electron_fsrPhotonIdx", &Electron_fsrPhotonIdx);  // New  

    // Muon
    tin->SetBranchAddress("nMuon", &nMuon);
    tin->SetBranchAddress("Muon_isTracker", &Muon_isTracker);
    tin->SetBranchAddress("Muon_looseId", &Muon_looseId);
    tin->SetBranchAddress("Muon_dz", &Muon_dz);
    tin->SetBranchAddress("Muon_dxy", &Muon_dxy);
    tin->SetBranchAddress("Muon_sip3d", &Muon_sip3d);
    tin->SetBranchAddress("Muon_pfRelIso03_all", &Muon_pfRelIso03_all);
    tin->SetBranchAddress("Muon_miniPFRelIso_all", &Muon_miniPFRelIso_all);
    tin->SetBranchAddress("Muon_pt", &Muon_pt);
    tin->SetBranchAddress("Muon_eta", &Muon_eta);
    tin->SetBranchAddress("Muon_phi", &Muon_phi);
    tin->SetBranchAddress("Muon_mass", &Muon_mass);
    tin->SetBranchAddress("Muon_charge", &Muon_charge);

    // Photon
    tin->SetBranchAddress("nPhoton",           &nPhoton);
    tin->SetBranchAddress("Photon_mvaID_WP80",&Photon_mvaID_WP80);
    tin->SetBranchAddress("Photon_electronIdx",&Photon_electronIdx);
    tin->SetBranchAddress("Photon_pt",        &Photon_pt);
    tin->SetBranchAddress("Photon_eta",       &Photon_eta);
    tin->SetBranchAddress("Photon_phi",       &Photon_phi);
    const bool has_Photon_superclusterEta = (tin->GetBranch("Photon_superclusterEta") != nullptr);
    if (has_Photon_superclusterEta){
      tin->SetBranchAddress("Photon_superclusterEta", &Photon_superclusterEta);
    }
    tin->SetBranchAddress("Photon_isScEtaEB", &Photon_isScEtaEB);  // New  
    tin->SetBranchAddress("Photon_isScEtaEE", &Photon_isScEtaEE);  // New  
    tin->SetBranchAddress("PV_npvsGood", &PV_npvsGood);
    tin->SetBranchAddress("PV_x", &PV_x);
    tin->SetBranchAddress("PV_y", &PV_y);
    tin->SetBranchAddress("PV_z", &PV_z);

    // FsrPhoton
    tin->SetBranchAddress("nFsrPhoton", &nFsrPhoton); // New
    tin->SetBranchAddress("FsrPhoton_pt", &FsrPhoton_pt); // New
    tin->SetBranchAddress("FsrPhoton_eta", &FsrPhoton_eta); // New
    tin->SetBranchAddress("FsrPhoton_phi", &FsrPhoton_phi); // New
    tin->SetBranchAddress("FsrPhoton_relIso03", &FsrPhoton_relIso03); // New
    tin->SetBranchAddress("FsrPhoton_dROverEt2", &FsrPhoton_dROverEt2); // New
    tin->SetBranchAddress("FsrPhoton_electronIdx", &FsrPhoton_electronIdx); // New
    tin->SetBranchAddress("FsrPhoton_muonIdx", &FsrPhoton_muonIdx); // New

    // GoldenJson
    ULong64_t event = 0;
    UInt_t run = 0;
    UInt_t luminosityBlock = 0;
    tin->SetBranchAddress("event", &event);
    tin->SetBranchAddress("run", &run);
    tin->SetBranchAddress("luminosityBlock", &luminosityBlock);
    bool isData = (std::string(sampleType) == "data");
    if (isData) {
        if (std::string(goldenJsonFile).empty()) {
            std::cerr << "[ERROR] sampleType=data but goldenJsonFile is empty." << std::endl;
            fout->Close();
            fin->Close();
            return;
        }
        goodLumiMap.clear(); // 같은 ROOT 세션에서 여러번 돌릴 때 중복 로딩 방지용
        if (!LoadGoldenJSON(goldenJsonFile)) {
            std::cerr << "[ERROR] Cannot open Golden JSON: " << goldenJsonFile << std::endl;
            fout->Close();
            fin->Close();
            return;
        }
        std::cout << "[INFO] Golden JSON loaded: " << goldenJsonFile << std::endl;
    }

    // --- 6) 이벤트 루프 ---
    Long64_t nPassJSON = 0;
    Long64_t nFailJSON = 0;
    for (Long64_t i = 0; i < nAll; ++i) {
        tin->GetEntry(i);

        if (isData) {
            if (!PassesGoodLumi((int)run, (int)luminosityBlock)){
                nFailJSON++;
                continue;
            }
            nPassJSON++;
        }

        // 기본값 초기화
        Z_mass_mm  = -1.0;
        Z_mass_mmg = -1.0;
        Photon_SCeta.clear();

        // -----------------------------
        // goodElectron index list
        // -----------------------------
        std::vector<int> goodEle;
        int nEleToCheck = std::min(nElectron, 16);

        for (int ie = 0; ie < nEleToCheck; ++ie) {

            // 1) pt cut
            if (Electron_pt[ie] <= 7.0) continue;

            // 2) |eta + deltaEtaSC| < 2.5
            float scEta = Electron_eta[ie] + Electron_deltaEtaSC[ie];
            if (std::fabs(scEta) >= 2.5) continue;

            // 3) HzzId_WP2022 apply to 2023_2024
            if (!HzzId_WP2022(Electron_pt[ie], scEta, Electron_mvaHZZIso[ie])) continue;

            // 4) |dz| < 1.0
            if (std::fabs(Electron_dz[ie]) >= 1.0) continue;

            // 5) |dxy| < 0.5
            if (std::fabs(Electron_dxy[ie]) >= 0.5) continue;

            // 모든 조건을 통과한 Electron
            goodEle.push_back(ie);
        }

        // lead/sublead 찾기 for HLT cut.
        leadEleIdx = -1;
        subleadEleIdx = -1;
        float leadElePt = -1.0;
        float subleadElePt = -1.0;

        nGoodEle = goodEle.size();
        for (int idx : goodEle) {
            float pt = Electron_pt[idx];

            if (pt > leadElePt) {
                // 기존 lead를 sublead로 밀어내기
                subleadElePt = leadElePt;
                subleadEleIdx = leadEleIdx;

                leadElePt = pt;
                leadEleIdx = idx;
            } else if (pt > subleadElePt) {
                subleadElePt = pt;
                subleadEleIdx = idx;
            }
        }
        // 이번엔 goodElectron 에 대응되는 Electron_photonIdx 찾기. 나중에 PhotonElectron veto 에 사용.
        std::set<int> photonVetoByElectron;
        for (int ie : goodEle) {
            int phoIdx = Electron_photonIdx[ie];
            if (phoIdx >= 0 && phoIdx < nPhoton) {
                photonVetoByElectron.insert(phoIdx);
            }
        }

        // -----------------------------
        // goodMuon index list
        // -----------------------------
        std::vector<int> goodMuon;
        int nMuonToCheck = std::min(nMuon, 16);

        for (int im = 0; im < nMuonToCheck; ++im) {

            // 1) pt cut
            if (Muon_pt[im] <= 5.0) continue;

            // 2) |eta| < 2.4
            float mu_eta = Muon_eta[im];
            if (std::fabs(mu_eta) >= 2.4) continue;

            // 3) Cut-based Loose ID
            if (Muon_looseId[im] != 1) continue;

            // 4) |dz| < 1.0
            if (std::fabs(Muon_dz[im]) >= 1.0) continue;

            // 5) |dxy| < 0.5
            if (std::fabs(Muon_dxy[im]) >= 0.5) continue;

            // 6) SIP_3D < 4
            if (Muon_sip3d[im] >= 4) continue;

            // 7) relIso < 0.35
            if (Muon_pfRelIso03_all[im] >= 0.35) continue;

            // 모든 조건을 통과한 Muon
            goodMuon.push_back(im);
        }

        leadMuonIdx = -1;
        subleadMuonIdx = -1;
        float leadMuonPt = -1.0;
        float subleadMuonPt = -1.0;

        nGoodMuon = goodMuon.size();
        for (int idx : goodMuon) {
            float pt = Muon_pt[idx];

            if (pt > leadMuonPt) {
                // 기존 lead를 sublead로 밀어내기
                subleadMuonPt = leadMuonPt;
                subleadMuonIdx = leadMuonIdx;

                leadMuonPt = pt;
                leadMuonIdx = idx;
            } else if (pt > subleadMuonPt) {
                subleadMuonPt = pt;
                subleadMuonIdx = idx;
            }
        }

        // -----------------------------
        // goodPhoton index list
        // -----------------------------
        std::vector<int> goodPhoton;
        int nPhoToCheck = std::min(nPhoton, 16);

        for (int ig = 0; ig < nPhoToCheck; ++ig) {
            // Calculate Photon_SCeta
            const bool isEB = (Photon_isScEtaEB[ig] == 1);
            float eta_for_sc = ComputeOriginEtaFromPV(isEB, Photon_eta[ig], Photon_phi[ig], PV_x, PV_y, PV_z); Photon_SCeta.push_back(eta_for_sc);
            float abs_eta_sc = std::fabs(eta_for_sc); 

            // 1) MVA WP80 cut
            if (Photon_mvaID_WP80[ig] != 1) continue;

            // 2) pt cut
            if (Photon_pt[ig] <= 15) continue;

            // 3) Always use PhotonSCeta.h. isScEtaEB, |eta_sc| < 1.4442 "or" isScEtaEE, 1.566 < |eta_sc| < 2.5
            bool inEB = (Photon_isScEtaEB[ig] == 1 && abs_eta_sc < 1.4442f);
            bool inEE = (Photon_isScEtaEE[ig] == 1 && abs_eta_sc > 1.566f && abs_eta_sc < 2.5f);
            if (!(inEB || inEE)) continue;

            // 4) dR_lg >= 0.3
            TLorentzVector g;
            g.SetPtEtaPhiM(Photon_pt[ig], Photon_eta[ig], Photon_phi[ig], 0);
            bool overlap = false;
            // dR_eg < 0.3 is overlap.
            for (int ie : goodEle) {
                TLorentzVector e;
                e.SetPtEtaPhiM(Electron_pt[ie], Electron_eta[ie], Electron_phi[ie], 0);
                if (g.DeltaR(e) < 0.3) { overlap = true; break; }
            }
            // dR_mg < 0.3 is overlap.
            if (!overlap) {
                for (int im : goodMuon) {
                    TLorentzVector m;
                    m.SetPtEtaPhiM(Muon_pt[im], Muon_eta[im], Muon_phi[im], 0);
                    if (g.DeltaR(m) < 0.3) { overlap = true; break; }
                }
            }
            if (overlap) continue;

            if (photonVetoByElectron.count(ig)) continue;

            // 모든 조건을 통과한 Photon
            goodPhoton.push_back(ig);
        }

        // -----------------------------
        // goodFsrPhoton index list
        // -----------------------------
        std::vector<int> goodFsr;
        int nFsrToCheck = std::min(nFsrPhoton, 16);

        // Pre-requitie for 5) : Find Leading-PT goodPhoton before FSR Loop.
        int leadPhoIdx = -1;
        float leadPho_pt = -1.f;
        for (int ig : goodPhoton){
            if (Photon_pt[ig] > leadPho_pt) {
                leadPho_pt = Photon_pt[ig];
                leadPhoIdx = ig; 
            }
        }
        bool hasLeadG = (leadPhoIdx >= 0); // There might be case : FSR exists, but no goodPhoton. (line436 : FSR photons selected from PF photon candidates)
        TLorentzVector lead_g;
        if (hasLeadG){
        lead_g.SetPtEtaPhiM(Photon_pt[leadPhoIdx], Photon_eta[leadPhoIdx], Photon_phi[leadPhoIdx], 0.0);
        }

        // FSR Loop begins.
        for (int i = 0; i < nFsrToCheck; ++i) {

            // 1) pt > 2
            if (FsrPhoton_pt[i] <= 2.0) continue;

            // 2) |eta| < 2.4
            float Fsr_eta = FsrPhoton_eta[i];
            if (std::fabs(Fsr_eta) >= 2.4) continue;

            // 3) Relative Iso < 1.8
            if (FsrPhoton_relIso03[i] >= 1.8) continue;

            // 4) dROverEt2 < 0.012
            if (FsrPhoton_dROverEt2[i] >= 0.012) continue;

            // 5) Veto goodPhoton : dR_goodFsr_goodPhoton > 0.2
            if (hasLeadG){
                TLorentzVector fsr;
                fsr.SetPtEtaPhiM(FsrPhoton_pt[i], FsrPhoton_eta[i], FsrPhoton_phi[i], 0.0);
                if (fsr.DeltaR(lead_g) <= 0.2) continue;
            }

            // 모든 조건을 통과한 FsrPhoton
            goodFsr.push_back(i);
        }
        nGoodPhoton = goodPhoton.size();

        // --------------------------------
        // Event Selection & Reconstruction
        // --------------------------------

        // S1. At least 2 opposite sign muons.
        bool hasOS = false;
        if ((int)goodMuon.size() < 2) continue;
        for (size_t a = 0; a < goodMuon.size() && !hasOS; ++a){
            for (size_t b = a+1; b < goodMuon.size(); ++b){
                int i1 = goodMuon[a], i2 = goodMuon[b];
                if (Muon_charge[i1] * Muon_charge[i2] == -1) {hasOS = true; break;}
            }
        }
        if (!hasOS) continue;

        // S2+S3. HLT with associated pT of leading objects.
        bool HLTpass =
            //(HLT_Ele32_WPTight_Gsf==1 && leadElePt > 35) ||
            (HLT_Ele30_WPTight_Gsf==1 && leadElePt > 35) ||
            (HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL==1 && leadElePt > 25 && subleadElePt > 15) ||
            //(HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ==1 && leadElePt > 25 && subleadElePt > 15) ||
            (HLT_IsoMu24==1 && leadMuonPt > 25) ||
            //(HLT_IsoMu27==1 && leadMuonPt > 28) ||
            //(HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL==1 && leadMuonPt > 20 && subleadMuonPt > 10) ||
            //(HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ==1 && leadMuonPt > 20 && subleadMuonPt > 10) ||
            //(HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8==1 && leadMuonPt > 20 && subleadMuonPt > 10) ||
            (HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8==1 && leadMuonPt > 20 && subleadMuonPt > 10);
        if (!HLTpass) continue;

        // S4. At least one photon
        if (goodPhoton.size() < 1) {
            continue;
        }

        // (1) OS pair 중에서 |M_ee - 91.19| 가 최소인 페어 선택 및 Z_ee mass 차이 계산. (쓰는건 아님.) 
        int best_i = -1;
        int best_j = -1;
        double best_diff = 1e9;
        double best_mass = -1.0;

        for (size_t a = 0; a < goodMuon.size(); ++a) {
            for (size_t b = a + 1; b < goodMuon.size(); ++b) {
                unsigned int i1 = goodMuon[a];
                unsigned int i2 = goodMuon[b];

                if (Muon_charge[i1] * Muon_charge[i2] != -1) continue; // OS 조건

                TLorentzVector tmp_mu1, tmp_mu2;
                tmp_mu1.SetPtEtaPhiM(Muon_pt[i1], Muon_eta[i1],
                                     Muon_phi[i1], Muon_mass[i1]);
                tmp_mu2.SetPtEtaPhiM(Muon_pt[i2], Muon_eta[i2],
                                     Muon_phi[i2], Muon_mass[i2]);
                TLorentzVector tmp_z = tmp_mu1 + tmp_mu2;
                double mass = tmp_z.M();
                double diff = std::fabs(mass - Z_MASS);

                if (diff < best_diff) {
                    best_diff = diff;
                    best_mass = mass;
                    best_i = i1;
                    best_j = i2;
                }
            }
        }

        // S5. (Part of) // Case 2. OS pair 자체가 없는 경우. 없는 경우 z_mm, z_mmg = -1.
        if (best_i < 0 || best_j < 0) {
            //std::cout << "Case 2" << endl;
            //std::cout << Z_mass_mm << endl << endl;
            tout->Fill();
            continue;
        }

        // Case 3. 선택된 OS pair 로 Z_ee 구성.
        // std::cout << "Case 3" << endl;
        TLorentzVector mu1, mu2, z_mm;
        mu1.SetPtEtaPhiM(Muon_pt[best_i], Muon_eta[best_i],
                        Muon_phi[best_i], Muon_mass[best_i]);
        mu2.SetPtEtaPhiM(Muon_pt[best_j], Muon_eta[best_j],
                        Muon_phi[best_j], Muon_mass[best_j]);
        z_mm = mu1 + mu2;
        Z_mass_mm  = z_mm.M();
        // std::cout << "Z_mm measured : " << Z_mass_mm << endl;
        Z_mass_mmg = Z_mass_mm;  // 기본값: photon 없으면 이 값 유지

        // S5. (Complete) Onshell Z.
        if (Z_mass_mm < 80 || Z_mass_mm > 100) { // Reduced Baseline Selection
            continue;
        }

        // leading goodPhoton adding.
        if (goodPhoton.size() >= 1) {
            int best_pho = -1;
            float best_pt = -999.0; // Metric is largest Photon_pt. AN-22-027_v4.pdf 676 line.
        
            for (int ig : goodPhoton) {
                float metric = Photon_pt[ig];
                if (metric > best_pt) {
                    best_pt = metric;
                    best_pho = ig;
                }
            }
        
            if (best_pho >= 0) {
                TLorentzVector g;
                g.SetPtEtaPhiM(Photon_pt[best_pho],
                               Photon_eta[best_pho],
                               Photon_phi[best_pho],
                               0.0);
        
                TLorentzVector z_mmg = z_mm + g;
                Z_mass_mmg = z_mmg.M();
                // std::cout << "Z_mmg updated : " << Z_mass_mmg << std::endl;
        
                AddPhoton_pt = Photon_pt[best_pho]; // To save.
                AddPhoton_eta = Photon_eta[best_pho]; // To save.
                double dr1 = g.DeltaR(mu1);
                double dr2 = g.DeltaR(mu2);
                dR_mg = std::min(dr1, dr2);
            }
        }

        // S6. g_pt / m_llg > 15./110
        if ((AddPhoton_pt/Z_mass_mmg) <= 15./110) continue;

        // S7. m_llg + m_ll > 185
        if (Z_mass_mmg + Z_mass_mm <= 185) continue;

        // S8. 95 < mllg < 180
        if (Z_mass_mmg <= 95 || Z_mass_mmg>= 180) continue;

        // S9. Event Filtering
        bool EventFilterPass =
                (Flag_goodVertices==1) &&
                (Flag_globalSuperTightHalo2016Filter==1) &&
                (Flag_EcalDeadCellTriggerPrimitiveFilter==1) &&
                (Flag_BadPFMuonFilter==1) &&
                (Flag_BadPFMuonDzFilter==1) &&
                (Flag_hfNoisyHitsFilter==1) &&
                (Flag_eeBadScFilter==1) &&
                (Flag_ecalBadCalibFilter==1);
        if (!EventFilterPass) continue;

        // S10. Control Region
        if (Z_mass_mmg > 120 && Z_mass_mmg < 130) { // Control Region
            continue;
        }

        // 이 이벤트 전체(원래 브랜치 + 새 브랜치) 저장
        // std::cout << Z_mass_mm << endl << endl;
        tout->Fill();
    } // Event Loop End.
    std::cout << "[INFO] JSON pass/fail = " << nPassJSON << " / " << nFailJSON << std::endl;

    // --- 7) 저장 ---
    fout->cd();
    tout->Write();
    fout->Close();
    fin->Close();

    std::cout << "[INFO] Done." << endl; //<< tout->GetEntries() << std::endl;
}
