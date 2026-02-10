// cutflow_single.C
// Usage (local):
//   root -l -q 'cutflow_single.C+("data","/path/to/input.root","ele","Cert_Collisions2023_366442_370790_Golden.json","results")'
//   root -l -q 'cutflow_single.C+("data","/path/to/input.root","mu","Cert_Collisions2023_366442_370790_Golden.json","results")'

#include "GoodLumi.h" // GoldenJson (LoadGoldenJSON, PassesGoodLumi, goodLumiMap)
#include "PhotonSCeta.h" // Calculate PhotonSCeta

#include "TSystem.h"
#include "TFile.h"
#include "TTree.h"
#include "TLorentzVector.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <string>
#include <cmath>
#include <algorithm>

// --- helpers (same as your cutflow.C) ---
static inline float ConvertMVA(float mva_mini) {
  return 2.0f / (1.0f + std::exp(-2.0f * mva_mini)) - 1.0f;
}

static inline bool HzzId_WP2022(float pt, float etasc, float hzzmvaid) {
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

static void PrintCutflow(const std::vector<std::string>& names,
                         const std::vector<long long>& cnt) {
  std::cout << "\n================ Cutflow ================\n";
  for (size_t i = 0; i < names.size(); ++i) {
    std::cout << Form("[%2zu] %-35s : %lld\n", i, names[i].c_str(), cnt[i]);
  }
  std::cout << "=========================================\n\n";
}

static std::string SanitizeFileName(std::string s) {
  // replace '/' -> '_'
  for (char &c : s) if (c == '/') c = '_';
  // remove dangerous chars
  for (char &c : s) if (c == ' ') c = '_';
  return s;
}

void cutflow_single(const char* sampleType,
                    const char* inFileName,
                    const char* channel = "ele",
                    const char* goldenJson = "",
                    const char* outDir = "results")
{
  // ---------------------------
  // 0) Open input ROOT file
  // ---------------------------
  TFile *fin = TFile::Open(inFileName, "READ");
  if (!fin || fin->IsZombie()) {
    std::cerr << "[ERROR] Cannot open input file: " << inFileName << "\n";
    return;
  }

  TTree *tin = dynamic_cast<TTree*>(fin->Get("Events"));
  if (!tin) {
    std::cerr << "[ERROR] Cannot find TTree 'Events' in file: " << inFileName << "\n";
    fin->Close();
    return;
  }

  const bool isData = (std::string(sampleType) == "data");

  // Golden JSON (data only)
  if (isData) {
    goodLumiMap.clear();
    if (!LoadGoldenJSON(goldenJson)) {
      std::cerr << "[ERROR] Cannot open Golden JSON: " << goldenJson << "\n";
      fin->Close();
      return;
    }
    std::cout << "[INFO] Golden JSON loaded: " << goldenJson << "\n";
  }

  const std::string chn = channel;
  const bool doEle = (chn == "ele");
  const bool doMu  = (chn == "mu");
  if (!doEle && !doMu) {
    std::cerr << "[ERROR] channel must be \"ele\" or \"mu\" (given: " << channel << ")\n";
    fin->Close();
    return;
  }

  std::cout << "[INFO] Input file: " << inFileName << "\n";
  std::cout << "[INFO] Entries   : " << tin->GetEntries() << "\n";
  std::cout << "[INFO] Channel   : " << (doEle ? "electron" : "muon") << "\n";

  // ---------------------------
  // 1) Set branches (same as your cutflow.C)
  // ---------------------------
  // HLT
  Bool_t HLT_Ele32_WPTight_Gsf, HLT_Ele30_WPTight_Gsf;
  Bool_t HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL, HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ;
  Bool_t HLT_IsoMu24, HLT_IsoMu27;
  Bool_t HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL, HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ;
  Bool_t HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8, HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8;

  // Event filters
  Bool_t Flag_goodVertices, Flag_globalSuperTightHalo2016Filter;
  Bool_t Flag_EcalDeadCellTriggerPrimitiveFilter, Flag_BadPFMuonFilter, Flag_BadPFMuonDzFilter;
  Bool_t Flag_hfNoisyHitsFilter, Flag_eeBadScFilter, Flag_ecalBadCalibFilter;

  // Leptons/photons
  static const int MAXP_64 = 64;
  static const int MAXP_128 = 128;
  Int_t nElectron = 0;
  Int_t Electron_charge[MAXP_64];
  Short_t Electron_photonIdx[MAXP_64];
  Float_t Electron_pt[MAXP_64], Electron_eta[MAXP_64], Electron_phi[MAXP_64], Electron_mass[MAXP_64];
  Float_t Electron_dz[MAXP_64], Electron_dxy[MAXP_64];
  Float_t Electron_mvaHZZIso[MAXP_64];
  Float_t Electron_deltaEtaSC[MAXP_64];

  Int_t nMuon = 0;
  Bool_t Muon_isTracker[MAXP_64], Muon_looseId[MAXP_64];
  Float_t Muon_dz[MAXP_64], Muon_dxy[MAXP_64], Muon_sip3d[MAXP_64], Muon_pfRelIso03_all[MAXP_64];
  Float_t Muon_pt[MAXP_64], Muon_eta[MAXP_64], Muon_phi[MAXP_64], Muon_mass[MAXP_64];
  Int_t   Muon_charge[MAXP_64];

  Int_t nPhoton = 0;
  Bool_t Photon_mvaID_WP80[MAXP_128];
  Short_t Photon_electronIdx[MAXP_128];
  Float_t Photon_pt[MAXP_128], Photon_eta[MAXP_128], Photon_phi[MAXP_128];
  Float_t Photon_superclusterEta[MAXP_128];
  Bool_t Photon_isScEtaEB[MAXP_128], Photon_isScEtaEE[MAXP_128];
  UChar_t PV_npvsGood;
  Float_t PV_x = 0.0f, PV_y = 0.0f, PV_z = 0.0f;

  // Golden JSON
  UInt_t run = 0, luminosityBlock = 0;

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

  tin->SetBranchAddress("Flag_goodVertices", &Flag_goodVertices);
  tin->SetBranchAddress("Flag_globalSuperTightHalo2016Filter", &Flag_globalSuperTightHalo2016Filter);
  tin->SetBranchAddress("Flag_EcalDeadCellTriggerPrimitiveFilter", &Flag_EcalDeadCellTriggerPrimitiveFilter);
  tin->SetBranchAddress("Flag_BadPFMuonFilter", &Flag_BadPFMuonFilter);
  tin->SetBranchAddress("Flag_BadPFMuonDzFilter", &Flag_BadPFMuonDzFilter);
  tin->SetBranchAddress("Flag_hfNoisyHitsFilter", &Flag_hfNoisyHitsFilter);
  tin->SetBranchAddress("Flag_eeBadScFilter", &Flag_eeBadScFilter);
  tin->SetBranchAddress("Flag_ecalBadCalibFilter", &Flag_ecalBadCalibFilter);

  tin->SetBranchAddress("nElectron", &nElectron);
  tin->SetBranchAddress("Electron_photonIdx", &Electron_photonIdx);
  tin->SetBranchAddress("Electron_charge", &Electron_charge);
  tin->SetBranchAddress("Electron_pt", &Electron_pt);
  tin->SetBranchAddress("Electron_eta", &Electron_eta);
  tin->SetBranchAddress("Electron_phi", &Electron_phi);
  tin->SetBranchAddress("Electron_mass", &Electron_mass);
  tin->SetBranchAddress("Electron_dz", &Electron_dz);
  tin->SetBranchAddress("Electron_dxy", &Electron_dxy);
  tin->SetBranchAddress("Electron_mvaHZZIso", &Electron_mvaHZZIso);
  tin->SetBranchAddress("Electron_deltaEtaSC", &Electron_deltaEtaSC);

  tin->SetBranchAddress("nMuon", &nMuon);
  tin->SetBranchAddress("Muon_isTracker", &Muon_isTracker);
  tin->SetBranchAddress("Muon_looseId", &Muon_looseId);
  tin->SetBranchAddress("Muon_dz", &Muon_dz);
  tin->SetBranchAddress("Muon_dxy", &Muon_dxy);
  tin->SetBranchAddress("Muon_sip3d", &Muon_sip3d);
  tin->SetBranchAddress("Muon_pfRelIso03_all", &Muon_pfRelIso03_all);
  tin->SetBranchAddress("Muon_pt", &Muon_pt);
  tin->SetBranchAddress("Muon_eta", &Muon_eta);
  tin->SetBranchAddress("Muon_phi", &Muon_phi);
  tin->SetBranchAddress("Muon_mass", &Muon_mass);
  tin->SetBranchAddress("Muon_charge", &Muon_charge);

  tin->SetBranchAddress("nPhoton", &nPhoton);
  tin->SetBranchAddress("Photon_mvaID_WP80", &Photon_mvaID_WP80);
  tin->SetBranchAddress("Photon_electronIdx", &Photon_electronIdx);
  tin->SetBranchAddress("Photon_pt", &Photon_pt);
  tin->SetBranchAddress("Photon_eta", &Photon_eta);
  tin->SetBranchAddress("Photon_phi", &Photon_phi);
  tin->SetBranchAddress("PV_x", &PV_x);
  tin->SetBranchAddress("PV_y", &PV_y);
  tin->SetBranchAddress("PV_z", &PV_z);

  const bool has_Photon_superclusterEta = (tin->GetBranch("Photon_superclusterEta") != nullptr);
  if (has_Photon_superclusterEta){
    tin->SetBranchAddress("Photon_superclusterEta", &Photon_superclusterEta);
  }
  tin->SetBranchAddress("Photon_isScEtaEB", &Photon_isScEtaEB);
  tin->SetBranchAddress("Photon_isScEtaEE", &Photon_isScEtaEE);

  tin->SetBranchAddress("run", &run);
  tin->SetBranchAddress("luminosityBlock", &luminosityBlock);

  // ---------------------------
  // 2) Cutflow counters (same names/order as your cutflow.C)
  // ---------------------------
  std::vector<std::string> cutNames = {
    "All events",
    "Pass Golden JSON (data only)",
    "S1: HLT + leading/subleading pT",
    "S2: >=2 good OS same flavour leptons (e/mu. not combined)",
    "S3: >=1 good photon",
    "S4: Onshell Z (80-100)",
    "S5: g_pt/m_llg > 15/110",
    "S6: (m_llg + m_ll) > 185",
    "S7: 95 < m_llg < 180",
    "S8: Event filters",
    "S9 : Control region (veto 120 < llg < 130)"
  };
  std::vector<long long> cnt(cutNames.size(), 0);

  const double Z_MASS = 91.19;

  // ---------------------------
  // 3) Event loop (same logic as your cutflow.C)
  // ---------------------------
  const Long64_t nAll = tin->GetEntries();
  for (Long64_t iev = 0; iev < nAll; ++iev) {
    tin->GetEntry(iev);
    cnt[0]++;

    // JSON
    if (isData) {
      if (!PassesGoodLumi((int)run, (int)luminosityBlock)) continue;
      cnt[1]++;
    } else {
      cnt[1]++;
    }

    // build good object lists
    std::vector<int> goodEle, goodMu, goodPho;

    // electrons
    for (int ie = 0; ie < std::min(nElectron, 16); ++ie) {
      if (Electron_pt[ie] <= 7.0) continue;
      float scEta = Electron_eta[ie] + Electron_deltaEtaSC[ie];
      if (std::fabs(scEta) >= 2.5) continue;
      if (!HzzId_WP2022(Electron_pt[ie], scEta, Electron_mvaHZZIso[ie])) continue;
      if (std::fabs(Electron_dz[ie]) >= 1.0) continue;
      if (std::fabs(Electron_dxy[ie]) >= 0.5) continue;
      goodEle.push_back(ie);
    }
    float leadElePt = -1.0, subleadElePt = -1.0;
    for (int idx : goodEle) {
      float pt = Electron_pt[idx];
      if (pt > leadElePt) { subleadElePt = leadElePt; leadElePt = pt; }
      else if (pt > subleadElePt) { subleadElePt = pt; }
    }
    std::set<int> photonVetoByElectron;
    for (int ie : goodEle) {
        int phoIdx = Electron_photonIdx[ie];
        if (phoIdx >= 0 && phoIdx < nPhoton) {
            photonVetoByElectron.insert(phoIdx);
        }
    }

    // muons
    for (int im = 0; im < std::min(nMuon, 16); ++im) {
      if (Muon_pt[im] <= 5.0) continue; // ScaReKIT (Run3) not applied.
      if (std::fabs(Muon_eta[im]) >= 2.4) continue;
      if (Muon_looseId[im] != 1) continue;
      if (std::fabs(Muon_dz[im]) >= 1.0) continue;
      if (std::fabs(Muon_dxy[im]) >= 0.5) continue;
      if (Muon_sip3d[im] >= 4.0) continue; // In AN it is < 4, thus >= 4.0
      if (Muon_pfRelIso03_all[im] >= 0.35) continue;
      goodMu.push_back(im);
    }
    float leadMuPt = -1.0, subleadMuPt = -1.0;
    for (int idx : goodMu) {
      float pt = Muon_pt[idx];
      if (pt > leadMuPt) { subleadMuPt = leadMuPt; leadMuPt = pt; }
      else if (pt > subleadMuPt) { subleadMuPt = pt; }
    }

    // photons (overlap veto)
    for (int ig = 0; ig < std::min(nPhoton, 16); ++ig) {
      if (Photon_mvaID_WP80[ig] != 1) continue;
      if (Photon_pt[ig] <= 15.0) continue;

      // 항상 PV 기반 origin-eta 계산 값을 사용 (Photon_superclusterEta는 사용 안 함)
      const bool isEB = (Photon_isScEtaEB[ig] == 1);
      float eta_for_sc = ComputeOriginEtaFromPV(isEB,
                                                Photon_eta[ig], Photon_phi[ig],
                                                PV_x, PV_y, PV_z);      

      float abs_eta_sc = std::fabs(eta_for_sc);
      bool inEB = (Photon_isScEtaEB[ig] == 1 && abs_eta_sc < 1.4442f);
      bool inEE = (Photon_isScEtaEE[ig] == 1 && abs_eta_sc > 1.566f && abs_eta_sc < 2.5f);
      if (!(inEB || inEE)) continue;


      TLorentzVector g; g.SetPtEtaPhiM(Photon_pt[ig], Photon_eta[ig], Photon_phi[ig], 0.0);
      bool overlap = false;

      for (int ie : goodEle) {
        TLorentzVector e; e.SetPtEtaPhiM(Electron_pt[ie], Electron_eta[ie], Electron_phi[ie], 0.0);
        if (g.DeltaR(e) < 0.3) { overlap = true; break; }
      }
      if (!overlap) {
        for (int im : goodMu) {
          TLorentzVector m; m.SetPtEtaPhiM(Muon_pt[im], Muon_eta[im], Muon_phi[im], 0.0);
          if (g.DeltaR(m) < 0.3) { overlap = true; break; }
        }
      }
      if (overlap) continue;

      //if (Photon_electronIdx[ig] != -1) continue;
      if (photonVetoByElectron.count(ig)) continue;

      goodPho.push_back(ig);
    }

    // S1
    bool HLTpass =
      //(HLT_Ele32_WPTight_Gsf==1 && leadElePt > 35) ||
      (HLT_Ele30_WPTight_Gsf==1 && leadElePt > 35) ||
      (HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL==1 && leadElePt > 25 && subleadElePt > 15) ||
      //(HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ==1 && leadElePt > 25 && subleadElePt > 15) ||
      (HLT_IsoMu24==1 && leadMuPt > 25) ||  // 25 in AN ; 20 in draw_pico
      //(HLT_IsoMu27==1 && leadMuPt > 28) ||  // 28 in AN ; 20 in draw_pico
      //(HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL==1 && leadMuPt > 20 && subleadMuPt > 10) ||
      //(HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ==1 && leadMuPt > 20 && subleadMuPt > 10) ||
      //(HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8==1 && leadMuPt > 20 && subleadMuPt > 10) ||
      (HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8==1 && leadMuPt > 20 && subleadMuPt > 10);
    if (!HLTpass) continue;
    cnt[2]++;

    // S2 : Require at least one OPPOSITE-SIGN same flavour pair among good leptons
    bool hasOS = false;
    
    if (doEle) {
      if ((int)goodEle.size() < 2) continue;
      for (size_t a = 0; a < goodEle.size() && !hasOS; ++a) {
        for (size_t b = a + 1; b < goodEle.size(); ++b) {
          int i1 = goodEle[a], i2 = goodEle[b];
          if (Electron_charge[i1] * Electron_charge[i2] == -1) { hasOS = true; break; }
        }
      }
    } else {
      if ((int)goodMu.size() < 2) continue;
      for (size_t a = 0; a < goodMu.size() && !hasOS; ++a) {
        for (size_t b = a + 1; b < goodMu.size(); ++b) {
          int i1 = goodMu[a], i2 = goodMu[b];
          if (Muon_charge[i1] * Muon_charge[i2] == -1) { hasOS = true; break; }
        }
      }
    }
    
    if (!hasOS) continue;
    cnt[3]++;


    // S3
    if ((int)goodPho.size() < 1) continue;
    cnt[4]++;

    // S5: best OS pair closest to Z
    int best_i = -1, best_j = -1;
    double best_diff = 1e18;
    double best_mass = -1.0;

    if (doEle) {
      for (size_t a = 0; a < goodEle.size(); ++a) {
        for (size_t b = a + 1; b < goodEle.size(); ++b) {
          int i1 = goodEle[a], i2 = goodEle[b];
          if (Electron_charge[i1] * Electron_charge[i2] != -1) continue;
          TLorentzVector e1, e2;
          e1.SetPtEtaPhiM(Electron_pt[i1], Electron_eta[i1], Electron_phi[i1], Electron_mass[i1]);
          e2.SetPtEtaPhiM(Electron_pt[i2], Electron_eta[i2], Electron_phi[i2], Electron_mass[i2]);
          double mass = (e1 + e2).M();
          double diff = std::fabs(mass - Z_MASS);
          if (diff < best_diff) { best_diff = diff; best_mass = mass; best_i = i1; best_j = i2; }
        }
      }
    } else {
      for (size_t a = 0; a < goodMu.size(); ++a) {
        for (size_t b = a + 1; b < goodMu.size(); ++b) {
          int i1 = goodMu[a], i2 = goodMu[b];
          if (Muon_charge[i1] * Muon_charge[i2] != -1) continue;
          TLorentzVector m1, m2;
          m1.SetPtEtaPhiM(Muon_pt[i1], Muon_eta[i1], Muon_phi[i1], Muon_mass[i1]);
          m2.SetPtEtaPhiM(Muon_pt[i2], Muon_eta[i2], Muon_phi[i2], Muon_mass[i2]);
          double mass = (m1 + m2).M();
          double diff = std::fabs(mass - Z_MASS);
          if (diff < best_diff) { best_diff = diff; best_mass = mass; best_i = i1; best_j = i2; }
        }
      }
    }

    double mll = best_mass;
    if (mll < 80 || mll > 100) continue;
    cnt[5]++;

    // leading photon (highest pT)
    int best_pho = -1;
    float best_pt = -1.0;
    for (int ig : goodPho) {
      if (Photon_pt[ig] > best_pt) { best_pt = Photon_pt[ig]; best_pho = ig; }
    }
    if (best_pho < 0) continue;

    TLorentzVector g; g.SetPtEtaPhiM(Photon_pt[best_pho], Photon_eta[best_pho], Photon_phi[best_pho], 0.0);

    TLorentzVector l1, l2;
    if (doEle) {
      l1.SetPtEtaPhiM(Electron_pt[best_i], Electron_eta[best_i], Electron_phi[best_i], Electron_mass[best_i]);
      l2.SetPtEtaPhiM(Electron_pt[best_j], Electron_eta[best_j], Electron_phi[best_j], Electron_mass[best_j]);
    } else {
      l1.SetPtEtaPhiM(Muon_pt[best_i], Muon_eta[best_i], Muon_phi[best_i], Muon_mass[best_i]);
      l2.SetPtEtaPhiM(Muon_pt[best_j], Muon_eta[best_j], Muon_phi[best_j], Muon_mass[best_j]);
    }
    TLorentzVector zll = l1 + l2;
    double mllg = (zll + g).M();
    double gpt  = Photon_pt[best_pho];

    if ((gpt / mllg) <= (15.0/110.0)) continue;
    cnt[6]++;

    if ((mllg + mll) <= 185.0) continue;
    cnt[7]++;

    if (mllg <= 95 || mllg >= 180) continue; // Draw_pico is 100, AN is 95
    cnt[8]++;

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
    cnt[9]++;

    // control region
    if (mllg > 120.0 && mllg < 130.0) continue;
    cnt[10]++;
  }

  // print to stdout
  PrintCutflow(cutNames, cnt);

  // ---------------------------
  // 4) Write a per-file result txt (merge-friendly)
  // ---------------------------
  gSystem->mkdir(outDir, /*recursive=*/kTRUE);
  
  std::string outName = std::string(outDir)
    + "/cutflow_" + SanitizeFileName(std::string(inFileName)) + ".txt";
  
  std::ofstream fout(outName);
  if (!fout.is_open()) {
    std::cerr << "[ERROR] Cannot write output txt: " << outName << "\n";
    fin->Close();
    return;
  }
  
  // NOTE:
  //  - one line = one cut
  //  - format: "<event_count> <description>"
  //  - line number == cut index
  for (size_t i = 0; i < cutNames.size(); ++i) {
    fout << cnt[i] << " " << cutNames[i] << "\n";
  }
  
  fout.close();
  
  std::cout << "[INFO] Wrote cutflow (simple format): " << outName << "\n";
  std::cout << "        INPUT=" << inFileName
            << "  CHANNEL=" << channel
            << "  SAMPLETYPE=" << sampleType << "\n";
  
  fin->Close();
}
