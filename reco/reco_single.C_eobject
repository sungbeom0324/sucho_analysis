#include "MakeSkimOutputPath.h"
#include "GoodLumi.h"   // LoadGoldenJSON, PassesGoodLumi
#include "TFile.h"
#include "TTree.h"
#include <iostream>
#include <string>
#include <cmath>
#include <algorithm>

// --- HZZ MVA WP helper (same as reco_single_muon) ---
static inline float ConvertMVA(float mva_mini) {
  return 2.0f / (1.0f + std::exp(-2.0f * mva_mini)) - 1.0f;
}

static inline bool HzzId_WP2022(float pt, float etasc, float hzzmvaid) {
  const float aeta = std::fabs(etasc);
  if (pt < 10.0f) {
    if (aeta < 0.8f)        return (hzzmvaid > ConvertMVA(1.6339f));
    else if (aeta < 1.479f) return (hzzmvaid > ConvertMVA(1.5499f));
    else                    return (hzzmvaid > ConvertMVA(2.0629f));
  } else {
    if (aeta < 0.8f)        return (hzzmvaid > ConvertMVA(0.3685f));
    else if (aeta < 1.479f) return (hzzmvaid > ConvertMVA(0.2662f));
    else                    return (hzzmvaid > ConvertMVA(-0.5444f));
  }
}

// THIS IS ONLY FOR OBJECT COMPARISION
void reco_single(const char* sampleType,
                         const char* inFileName,
                         const char* baseOutDir = "/u/user/sucho/SE_UserHome/reco_eobject",
                         const char* goldenJsonFile = "")
{
  // output path
  std::string outFileName = MakeSkimOutputPath(sampleType, inFileName, baseOutDir);

  std::cout << "[INFO] Input  file = " << inFileName  << "\n";
  std::cout << "[INFO] Output file = " << outFileName << "\n";

  // open input
  TFile* fin = TFile::Open(inFileName, "READ");
  if (!fin || fin->IsZombie()) {
    std::cerr << "[ERROR] Cannot open input file: " << inFileName << "\n";
    return;
  }

  TTree* tin = (TTree*)fin->Get("Events");
  if (!tin) {
    std::cerr << "[ERROR] Cannot find TTree 'Events'\n";
    fin->Close();
    return;
  }

  const Long64_t nAll = tin->GetEntries();
  std::cout << "[INFO] Input entries : " << nAll << "\n";

  // Data? then need Golden JSON
  const bool isData = (std::string(sampleType) == "data");
  if (isData) {
    if (std::string(goldenJsonFile).empty()) {
      std::cerr << "[ERROR] sampleType=data but goldenJsonFile is empty.\n";
      fin->Close();
      return;
    }
    goodLumiMap.clear();
    if (!LoadGoldenJSON(goldenJsonFile)) {
      std::cerr << "[ERROR] Cannot open Golden JSON: " << goldenJsonFile << "\n";
      fin->Close();
      return;
    }
    std::cout << "[INFO] Golden JSON loaded: " << goldenJsonFile << "\n";
  }

  // Keep only the branches we want
  tin->SetBranchStatus("*", 0);

  // event id (for JSON & debugging)
  tin->SetBranchStatus("run", 1);
  tin->SetBranchStatus("luminosityBlock", 1);
  tin->SetBranchStatus("event", 1);

  // electron branches
  tin->SetBranchStatus("nElectron", 1);
  tin->SetBranchStatus("Electron_pt", 1);
  tin->SetBranchStatus("Electron_eta", 1);
  tin->SetBranchStatus("Electron_deltaEtaSC", 1);
  tin->SetBranchStatus("Electron_photonIdx", 1);
  tin->SetBranchStatus("Photon_electronIdx", 1);
  // photon branches
  tin->SetBranchStatus("nPhoton", 1);
  tin->SetBranchStatus("Photon_mvaID_WP80", 1);
  tin->SetBranchStatus("Photon_pt", 1);
  tin->SetBranchStatus("Photon_eta", 1);
  tin->SetBranchStatus("Photon_phi", 1);

  // these may or may not exist depending on NanoAOD content
  const bool has_dz  = (tin->GetBranch("Electron_dz") != nullptr);
  const bool has_dxy = (tin->GetBranch("Electron_dxy") != nullptr);
  const bool has_mva = (tin->GetBranch("Electron_mvaHZZIso") != nullptr);

  if (has_dz)  tin->SetBranchStatus("Electron_dz", 1);
  if (has_dxy) tin->SetBranchStatus("Electron_dxy", 1);
  if (has_mva) tin->SetBranchStatus("Electron_mvaHZZIso", 1);

  std::cout << "[INFO] Branch existence: "
            << "dz=" << has_dz << ", dxy=" << has_dxy << ", mvaHZZIso=" << has_mva << "\n";

  // ----- Bind branches needed to compute HzzId_WP2022 and Electron_photonIdx_nMatch -----
  Int_t   nElectron = 0;
  Float_t Electron_pt[16];
  Float_t Electron_eta[16];
  Float_t Electron_deltaEtaSC[16];
  Float_t Electron_mvaHZZIso[16]; // only valid if has_mva
  Short_t Electron_photonIdx[16];
  Short_t Photon_electronIdx[16];
  Int_t   Electron_photonIdx_nMatch;
  Int_t   Photon_electronIdx_nMatch;
  // photon input branches
  Int_t   nPhoton = 0;
  Bool_t  Photon_mvaID_WP80[16];
  Float_t Photon_pt[16];
  Float_t Photon_eta[16];
  Float_t Photon_phi[16];
  // bind brances.
  tin->SetBranchAddress("nElectron", &nElectron);
  tin->SetBranchAddress("Electron_pt", &Electron_pt);
  tin->SetBranchAddress("Electron_eta", &Electron_eta);
  tin->SetBranchAddress("Electron_deltaEtaSC", &Electron_deltaEtaSC);
  if (has_mva) tin->SetBranchAddress("Electron_mvaHZZIso", &Electron_mvaHZZIso);
  tin->SetBranchAddress("Electron_photonIdx", &Electron_photonIdx);
  tin->SetBranchAddress("Photon_electronIdx", &Photon_electronIdx);
  tin->SetBranchAddress("nPhoton", &nPhoton);
  tin->SetBranchAddress("Photon_mvaID_WP80", &Photon_mvaID_WP80);
  tin->SetBranchAddress("Photon_pt", &Photon_pt);
  tin->SetBranchAddress("Photon_eta", &Photon_eta);
  tin->SetBranchAddress("Photon_phi", &Photon_phi);
  

  // open output
  TFile* fout = TFile::Open(outFileName.c_str(), "RECREATE");
  if (!fout || fout->IsZombie()) {
    std::cerr << "[ERROR] Cannot create output file: " << outFileName << "\n";
    fin->Close();
    return;
  }

  // Create output tree with same selected-branch structure
  // (CloneTree(0): make an empty clone containing only enabled branches)
  TTree* tout = tin->CloneTree(0);
  tout->SetName("Events");
  tout->SetTitle("Events (GoldenJSON only + selected electron branches)");

  // ----- NEW derived branch: WP2022 pass/fail per electron slot -----
  Bool_t Electron_HzzID_WP2022[16];
  tout->Branch("Electron_HzzID_WP2022", Electron_HzzID_WP2022, "Electron_HzzID_WP2022[16]/O");
  tout->Branch("Electron_photonIdx_nMatch", &Electron_photonIdx_nMatch, "Electron_photonIdx_nMatch/I");
  tout->Branch("Photon_electronIdx_nMatch", &Photon_electronIdx_nMatch, "Photon_electronIdx_nMatch/I");
  
  Long64_t nPassJSON = 0, nFailJSON = 0;

  // Need run/lumi values to check JSON:
  UInt_t run = 0, luminosityBlock = 0;
  tin->SetBranchAddress("run", &run);
  tin->SetBranchAddress("luminosityBlock", &luminosityBlock);

  // EVENT LOOP BEGIN
  for (Long64_t i = 0; i < nAll; ++i) {
    if (i % 1000 != 0) continue;
    tin->GetEntry(i);
    Electron_photonIdx_nMatch = 0;
    Photon_electronIdx_nMatch = 0;

    if (isData) {
      if (!PassesGoodLumi((int)run, (int)luminosityBlock)) {
        nFailJSON++;
        continue;
      }
      nPassJSON++;
    }

    // ----- compute derived branch values for this event -----
    for (int k = 0; k < 16; ++k) Electron_HzzID_WP2022[k] = 0;

    // Electron Loop for Hzz pass/fail. Leave separately for readability.
    if (has_mva) {
      const int nEleToCheck = std::min(nElectron, 16);
      for (int ie = 0; ie < nEleToCheck; ++ie) {
        const float scEta = Electron_eta[ie] + Electron_deltaEtaSC[ie];
        Electron_HzzID_WP2022[ie] =
          (Bool_t)HzzId_WP2022(Electron_pt[ie], scEta, Electron_mvaHZZIso[ie]);
      }
    }

    // Electron Loop for matching idx.
    const int nEleToCheck = std::min(nElectron, 16);
    for (int ie = 0; ie < nEleToCheck; ++ie) {
      if (Electron_photonIdx[ie] != -1)
        Electron_photonIdx_nMatch++;
    }

    // Photon Loop for matching idx.
    const int nPhoToCheck = std::min(nPhoton, 16);
    for (int ig = 0; ig < nPhoToCheck; ++ig) {
      if (Photon_electronIdx[ig] != -1)
        Photon_electronIdx_nMatch++;
    }

    tout->Fill();
  }

  std::cout << "[INFO] JSON pass/fail = " << nPassJSON << " / " << nFailJSON << "\n";
  std::cout << "[INFO] Saved events = " << tout->GetEntries() << "\n";

  fout->cd();
  tout->Write();
  fout->Close();
  fin->Close();

  std::cout << "[INFO] Done.\n";
}
