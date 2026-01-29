// palette_all.C
#include "include/PlotTypes.h"

#include <vector>
#include <TSystem.h>
#include <TString.h>  // Form()

// ------------------------------------------------------------
// IMPORTANT:
// cling이 palette_all()을 링크할 때 plot1ChainPDF 심볼이 필요함.
// .L 로드는 런타임이라 늦어서 unresolved 발생.
// 그래서 구현 파일을 include로 끌어와서 "정의"를 미리 보이게 함.
// ------------------------------------------------------------
#include "src/Plot1ChainPDF.C"
#include "src/PlotCompareRatioPDF.C"
#include "src/PlotCompareRatioBlindPDF.C"

// ------------------------------------------------------------
// Entry function: 파일명과 동일하게 두는 게 가장 안전
// 실행: root -l -q 'palette_all.C("plots_out")'
// ------------------------------------------------------------
void palette_all(const char* outdir = "plots_out") {
  gSystem->mkdir(outdir, kTRUE);

  // ------------------------------------------------------------
  // 1) OneChain jobs (1 input list -> 여러 1D/2D)
  // ------------------------------------------------------------
  std::vector<OneChainJob> oneChainJobs = {
    {
      "/pnfs/knu.ac.kr/data/cms/store/user/sucho/reco/data/list_2023_Muon_NanoAODv15_reco.txt",
      "Events",
      "Run2024_Muon_onechain",
      "", // baseCut
      {
        { "Z_mass_mm",  "Z_mass_mm",  60,  60, 120, "m_{#mu#mu} [GeV]" },
        { "Z_mass_mmg", "Z_mass_mmg", 80,  80, 200, "m_{#mu#mug} [GeV]" },
        { "dR_mg",      "dR_mg",      40,   0,   4, "#DeltaR(#mu,#gamma)" }
      },
      {
        { "Zmmg_vs_Zmm",
          "Z_mass_mmg:Z_mass_mm",
          60, 60, 120,
          80, 80, 200,
          "m_{#mu#mu} [GeV]",
          "m_{#mu#mug} [GeV]",
          "" }
      }
    }
  };

  // ------------------------------------------------------------
  // 2) Compare jobs (non-blind)
  // ------------------------------------------------------------
  std::vector<CompareJob> compareJobs = {
/*
    {
      "/pnfs/knu.ac.kr/data/cms/store/user/sucho/reco/data/list_2023_Muon_NanoAODv15_reco.txt",
      "/pnfs/knu.ac.kr/data/cms/store/user/sucho/reco/data/list_2024_Muon_PromptReco_reco.txt",
      "Events",
      "Muon_23_vs_24_compare",
      "23_Muon",
      "24_Muon",
      "", // baseCut
      false, "", 0, 0, // blind off

      1,         // Normalization mode 1 = Lumi, 0 = Event
      18.063,    // 2023 Lumi
      108.95,    // 2024 Lumi

      {
        { "Z_mass_mm",  "Z_mass_mm",  40, 70, 110, "m_{#mu#mu} [GeV]" },
        { "Z_mass_mmg", "Z_mass_mmg", 80, 80, 200, "m_{#mu#mu#gamma} [GeV]" },
        { "dR_mg",      "dR_mg",      40,  0,   4, "#DeltaR(#mu,#gamma)" }
      }
    },

    {
      "/pnfs/knu.ac.kr/data/cms/store/user/sucho/reco/data/list_2023_EGamma_NanoAODv15_reco.txt",
      "/pnfs/knu.ac.kr/data/cms/store/user/sucho/reco/data/list_2024_EGamma_PromptReco_reco.txt",
      "Events",
      "EGamma_23_vs_24_compare",
      "23_EGamma",
      "24_EGamma",
      "", // baseCut
      false, "", 0, 0, // blind off

      1,         // Normalization mode 1 = Lumi, 0 = Event
      18.063,    // 2023 Lumi
      108.95,    // 2024 Lumi

      {
        { "Z_mass_ee",  "Z_mass_ee",  40, 70, 110, "m_{ee} [GeV]" },
        { "Z_mass_eeg", "Z_mass_eeg", 80, 80, 200, "m_{ee#gamma} [GeV]" },
        { "dR_eg",      "dR_eg",      40,  0,   4, "#DeltaR(e,#gamma)" }
      }
    },
*/


/*
    {
      "/pnfs/knu.ac.kr/data/cms/store/user/sucho/reco_eobject/data/list_2023_EGamma_NanoAODv15_reco.txt",
      "/pnfs/knu.ac.kr/data/cms/store/user/sucho/reco_eobject/data/list_2024B_EGamma_PromptReco_reco.txt",
      "Events",
      "EGamma_23_vs_24B",
      "23C_EG",
      "24B_EG",
      "nElectron>=2 && nPhoton>= 1", // baseCut
      false, "", 0, 0, // blind off

      0,         // Normalization mode 1 = Lumi, 0 = Event
      18.063,    // 2023 Lumi
      108.95,    // 2024 Lumi

      {
        { "nElectron",  "nElectron",  7, 0, 7, "N_{e}" },
        { "Electron1_pt", "Electron_pt[0]", 300, 0, 300, "Electron_pt1 [GeV]" },
        { "Electron2_pt", "Electron_pt[1]", 100, 0, 100, "Electron_pt2 [GeV]" },
        { "Electron1_SCeta", "Electron_eta[0] + Electron_deltaEtaSC[0]", 60, -3, 3, "Electron1_SCeta" },
        { "Electron2_SCeta", "Electron_eta[1] + Electron_deltaEtaSC[1]", 60, -3, 3, "Electron2_SCeta" },
        { "Electron1_mvaHZZIso", "Electron_mvaHZZIso[0]", 20, -1, 1, "Electron1_mvaHZZIso" },
        { "Electron2_mvaHZZIso", "Electron_mvaHZZIso[1]", 20, -1, 1, "Electron2_mvaHZZIso" },
        { "Electron1_HzzID_WP2022", "Electron_HzzID_WP2022[0]", 2, 0, 2, "Electron1_HzzID_WP2022" },
        { "Electron2_HzzID_WP2022", "Electron_HzzID_WP2022[1]", 2, 0, 2, "Electron2_HzzID_WP2022" },
        { "Electron_photonIdx_nMatch", "Electron_photonIdx_nMatch", 6, 0, 6, "Electron_photonIdx_nMatch" },
        { "Electron_photonIdx_nMatch / nElectron", "Electron_photonIdx_nMatch/nElectron", 10, 0, 1, "Electron_photonIdx_nMatch / N_{e}" },
        { "Photon_electronIdx_nMatch", "Photon_electronIdx_nMatch", 6, 0, 6, "Photon_electronIdx_nMatch" },
        { "nPhoton", "nPhoton", 6, 0, 6, "N_{#gamma}" },
        { "Photon_electronIdx_nMatch", "Photon_electronIdx_nMatch", 6, 0, 6, "Photon_electronIdx_nMatch" },
        { "Photon_electronIdx_nMatch / nPhoton", "Photon_electronIdx_nMatch/nPhoton", 10, 0, 1, "Photon_electronIdx_nMatch / N_{#gamma}" },
        { "Photon1_pt", "Photon_pt[0]", 200, 0, 200, "Photon1_pt"},
        { "Photon1_eta", "Photon_eta[0]", 60, -3, 3, "Photon1_eta"},
      }
    },

    {
      "/pnfs/knu.ac.kr/data/cms/store/user/sucho/reco_eobject/data/list_2023_EGamma_NanoAODv15_reco.txt",
      "/pnfs/knu.ac.kr/data/cms/store/user/sucho/reco_eobject/data/list_2024C_EGamma_PromptReco_reco.txt",
      "Events",
      "EGamma_23_vs_24C",
      "23C_EG",
      "24C_EG",
      "nElectron>=2 && nPhoton>= 1", // baseCut
      false, "", 0, 0, // blind off

      0,         // Normalization mode 1 = Lumi, 0 = Event
      18.063,    // 2023 Lumi
      108.95,    // 2024 Lumi

      {
        { "nElectron",  "nElectron",  7, 0, 7, "N_{e}" },
        { "Electron1_pt", "Electron_pt[0]", 300, 0, 300, "Electron_pt1 [GeV]" },
        { "Electron2_pt", "Electron_pt[1]", 100, 0, 100, "Electron_pt2 [GeV]" },
        { "Electron1_SCeta", "Electron_eta[0] + Electron_deltaEtaSC[0]", 60, -3, 3, "Electron1_SCeta" },
        { "Electron2_SCeta", "Electron_eta[1] + Electron_deltaEtaSC[1]", 60, -3, 3, "Electron2_SCeta" },
        { "Electron1_mvaHZZIso", "Electron_mvaHZZIso[0]", 20, -1, 1, "Electron1_mvaHZZIso" },
        { "Electron2_mvaHZZIso", "Electron_mvaHZZIso[1]", 20, -1, 1, "Electron2_mvaHZZIso" },
        { "Electron1_HzzID_WP2022", "Electron_HzzID_WP2022[0]", 2, 0, 2, "Electron1_HzzID_WP2022" },
        { "Electron2_HzzID_WP2022", "Electron_HzzID_WP2022[1]", 2, 0, 2, "Electron2_HzzID_WP2022" },
        { "Electron_photonIdx_nMatch", "Electron_photonIdx_nMatch", 6, 0, 6, "Electron_photonIdx_nMatch" },
        { "Electron_photonIdx_nMatch / nElectron", "Electron_photonIdx_nMatch/nElectron", 10, 0, 1, "Electron_photonIdx_nMatch / N_{e}" },
        { "Photon_electronIdx_nMatch", "Photon_electronIdx_nMatch", 6, 0, 6, "Photon_electronIdx_nMatch" },
        { "nPhoton", "nPhoton", 6, 0, 6, "N_{#gamma}" },
        { "Photon_electronIdx_nMatch", "Photon_electronIdx_nMatch", 6, 0, 6, "Photon_electronIdx_nMatch" },
        { "Photon_electronIdx_nMatch / nPhoton", "Photon_electronIdx_nMatch/nPhoton", 10, 0, 1, "Photon_electronIdx_nMatch / N_{#gamma}" },
        { "Photon1_pt", "Photon_pt[0]", 200, 0, 200, "Photon1_pt"},
        { "Photon1_eta", "Photon_eta[0]", 60, -3, 3, "Photon1_eta"},
      }
    },

    {
      "/pnfs/knu.ac.kr/data/cms/store/user/sucho/reco_eobject/data/list_2023_EGamma_NanoAODv15_reco.txt",
      "/pnfs/knu.ac.kr/data/cms/store/user/sucho/reco_eobject/data/list_2024D_EGamma_PromptReco_reco.txt",
      "Events",
      "EGamma_23_vs_24D",
      "23C_EG",
      "24D_EG",
      "nElectron>=2 && nPhoton>= 1", // baseCut
      false, "", 0, 0, // blind off

      0,         // Normalization mode 1 = Lumi, 0 = Event
      18.063,    // 2023 Lumi
      108.95,    // 2024 Lumi

      {
        { "nElectron",  "nElectron",  7, 0, 7, "N_{e}" },
        { "Electron1_pt", "Electron_pt[0]", 300, 0, 300, "Electron_pt1 [GeV]" },
        { "Electron2_pt", "Electron_pt[1]", 100, 0, 100, "Electron_pt2 [GeV]" },
        { "Electron1_SCeta", "Electron_eta[0] + Electron_deltaEtaSC[0]", 60, -3, 3, "Electron1_SCeta" },
        { "Electron2_SCeta", "Electron_eta[1] + Electron_deltaEtaSC[1]", 60, -3, 3, "Electron2_SCeta" },
        { "Electron1_mvaHZZIso", "Electron_mvaHZZIso[0]", 20, -1, 1, "Electron1_mvaHZZIso" },
        { "Electron2_mvaHZZIso", "Electron_mvaHZZIso[1]", 20, -1, 1, "Electron2_mvaHZZIso" },
        { "Electron1_HzzID_WP2022", "Electron_HzzID_WP2022[0]", 2, 0, 2, "Electron1_HzzID_WP2022" },
        { "Electron2_HzzID_WP2022", "Electron_HzzID_WP2022[1]", 2, 0, 2, "Electron2_HzzID_WP2022" },
        { "Electron_photonIdx_nMatch", "Electron_photonIdx_nMatch", 6, 0, 6, "Electron_photonIdx_nMatch" },
        { "Electron_photonIdx_nMatch / nElectron", "Electron_photonIdx_nMatch/nElectron", 10, 0, 1, "Electron_photonIdx_nMatch / N_{e}" },
        { "Photon_electronIdx_nMatch", "Photon_electronIdx_nMatch", 6, 0, 6, "Photon_electronIdx_nMatch" },
        { "nPhoton", "nPhoton", 6, 0, 6, "N_{#gamma}" },
        { "Photon_electronIdx_nMatch", "Photon_electronIdx_nMatch", 6, 0, 6, "Photon_electronIdx_nMatch" },
        { "Photon_electronIdx_nMatch / nPhoton", "Photon_electronIdx_nMatch/nPhoton", 10, 0, 1, "Photon_electronIdx_nMatch / N_{#gamma}" },
        { "Photon1_pt", "Photon_pt[0]", 200, 0, 200, "Photon1_pt"},
        { "Photon1_eta", "Photon_eta[0]", 60, -3, 3, "Photon1_eta"},
      }
    },

    {
      "/pnfs/knu.ac.kr/data/cms/store/user/sucho/reco_eobject/data/list_2023_EGamma_NanoAODv15_reco.txt",
      "/pnfs/knu.ac.kr/data/cms/store/user/sucho/reco_eobject/data/list_2024E_EGamma_PromptReco_reco.txt",
      "Events",
      "EGamma_23_vs_24E",
      "23C_EG",
      "24E_EG",
      "nElectron>=2 && nPhoton>= 1", // baseCut
      false, "", 0, 0, // blind off

      0,         // Normalization mode 1 = Lumi, 0 = Event
      18.063,    // 2023 Lumi
      108.95,    // 2024 Lumi

      {
        { "nElectron",  "nElectron",  7, 0, 7, "N_{e}" },
        { "Electron1_pt", "Electron_pt[0]", 300, 0, 300, "Electron_pt1 [GeV]" },
        { "Electron2_pt", "Electron_pt[1]", 100, 0, 100, "Electron_pt2 [GeV]" },
        { "Electron1_SCeta", "Electron_eta[0] + Electron_deltaEtaSC[0]", 60, -3, 3, "Electron1_SCeta" },
        { "Electron2_SCeta", "Electron_eta[1] + Electron_deltaEtaSC[1]", 60, -3, 3, "Electron2_SCeta" },
        { "Electron1_mvaHZZIso", "Electron_mvaHZZIso[0]", 20, -1, 1, "Electron1_mvaHZZIso" },
        { "Electron2_mvaHZZIso", "Electron_mvaHZZIso[1]", 20, -1, 1, "Electron2_mvaHZZIso" },
        { "Electron1_HzzID_WP2022", "Electron_HzzID_WP2022[0]", 2, 0, 2, "Electron1_HzzID_WP2022" },
        { "Electron2_HzzID_WP2022", "Electron_HzzID_WP2022[1]", 2, 0, 2, "Electron2_HzzID_WP2022" },
        { "Electron_photonIdx_nMatch", "Electron_photonIdx_nMatch", 6, 0, 6, "Electron_photonIdx_nMatch" },
        { "Electron_photonIdx_nMatch / nElectron", "Electron_photonIdx_nMatch/nElectron", 10, 0, 1, "Electron_photonIdx_nMatch / N_{e}" },
        { "Photon_electronIdx_nMatch", "Photon_electronIdx_nMatch", 6, 0, 6, "Photon_electronIdx_nMatch" },
        { "nPhoton", "nPhoton", 6, 0, 6, "N_{#gamma}" },
        { "Photon_electronIdx_nMatch", "Photon_electronIdx_nMatch", 6, 0, 6, "Photon_electronIdx_nMatch" },
        { "Photon_electronIdx_nMatch / nPhoton", "Photon_electronIdx_nMatch/nPhoton", 10, 0, 1, "Photon_electronIdx_nMatch / N_{#gamma}" },
        { "Photon1_pt", "Photon_pt[0]", 200, 0, 200, "Photon1_pt"},
        { "Photon1_eta", "Photon_eta[0]", 60, -3, 3, "Photon1_eta"},
      }
    },

    {
      "/pnfs/knu.ac.kr/data/cms/store/user/sucho/reco_eobject/data/list_2023_EGamma_NanoAODv15_reco.txt",
      "/pnfs/knu.ac.kr/data/cms/store/user/sucho/reco_eobject/data/list_2024F_EGamma_PromptReco_reco.txt",
      "Events",
      "EGamma_23_vs_24F",
      "23C_EG",
      "24F_EG",
      "nElectron>=2 && nPhoton>= 1", // baseCut
      false, "", 0, 0, // blind off

      0,         // Normalization mode 1 = Lumi, 0 = Event
      18.063,    // 2023 Lumi
      108.95,    // 2024 Lumi

      {
        { "nElectron",  "nElectron",  7, 0, 7, "N_{e}" },
        { "Electron1_pt", "Electron_pt[0]", 300, 0, 300, "Electron_pt1 [GeV]" },
        { "Electron2_pt", "Electron_pt[1]", 100, 0, 100, "Electron_pt2 [GeV]" },
        { "Electron1_SCeta", "Electron_eta[0] + Electron_deltaEtaSC[0]", 60, -3, 3, "Electron1_SCeta" },
        { "Electron2_SCeta", "Electron_eta[1] + Electron_deltaEtaSC[1]", 60, -3, 3, "Electron2_SCeta" },
        { "Electron1_mvaHZZIso", "Electron_mvaHZZIso[0]", 20, -1, 1, "Electron1_mvaHZZIso" },
        { "Electron2_mvaHZZIso", "Electron_mvaHZZIso[1]", 20, -1, 1, "Electron2_mvaHZZIso" },
        { "Electron1_HzzID_WP2022", "Electron_HzzID_WP2022[0]", 2, 0, 2, "Electron1_HzzID_WP2022" },
        { "Electron2_HzzID_WP2022", "Electron_HzzID_WP2022[1]", 2, 0, 2, "Electron2_HzzID_WP2022" },
        { "Electron_photonIdx_nMatch", "Electron_photonIdx_nMatch", 6, 0, 6, "Electron_photonIdx_nMatch" },
        { "Electron_photonIdx_nMatch / nElectron", "Electron_photonIdx_nMatch/nElectron", 10, 0, 1, "Electron_photonIdx_nMatch / N_{e}" },
        { "Photon_electronIdx_nMatch", "Photon_electronIdx_nMatch", 6, 0, 6, "Photon_electronIdx_nMatch" },
        { "nPhoton", "nPhoton", 6, 0, 6, "N_{#gamma}" },
        { "Photon_electronIdx_nMatch", "Photon_electronIdx_nMatch", 6, 0, 6, "Photon_electronIdx_nMatch" },
        { "Photon_electronIdx_nMatch / nPhoton", "Photon_electronIdx_nMatch/nPhoton", 10, 0, 1, "Photon_electronIdx_nMatch / N_{#gamma}" },
        { "Photon1_pt", "Photon_pt[0]", 200, 0, 200, "Photon1_pt"},
        { "Photon1_eta", "Photon_eta[0]", 60, -3, 3, "Photon1_eta"},
      }
    },

    {
      "/pnfs/knu.ac.kr/data/cms/store/user/sucho/reco_eobject/data/list_2023_EGamma_NanoAODv15_reco.txt",
      "/pnfs/knu.ac.kr/data/cms/store/user/sucho/reco_eobject/data/list_2024G_EGamma_PromptReco_reco.txt",
      "Events",
      "EGamma_23_vs_24G",
      "23C_EG",
      "24G_EG",
      "nElectron>=2 && nPhoton>= 1", // baseCut
      false, "", 0, 0, // blind off

      0,         // Normalization mode 1 = Lumi, 0 = Event
      18.063,    // 2023 Lumi
      108.95,    // 2024 Lumi

      {
        { "nElectron",  "nElectron",  7, 0, 7, "N_{e}" },
        { "Electron1_pt", "Electron_pt[0]", 300, 0, 300, "Electron_pt1 [GeV]" },
        { "Electron2_pt", "Electron_pt[1]", 100, 0, 100, "Electron_pt2 [GeV]" },
        { "Electron1_SCeta", "Electron_eta[0] + Electron_deltaEtaSC[0]", 60, -3, 3, "Electron1_SCeta" },
        { "Electron2_SCeta", "Electron_eta[1] + Electron_deltaEtaSC[1]", 60, -3, 3, "Electron2_SCeta" },
        { "Electron1_mvaHZZIso", "Electron_mvaHZZIso[0]", 20, -1, 1, "Electron1_mvaHZZIso" },
        { "Electron2_mvaHZZIso", "Electron_mvaHZZIso[1]", 20, -1, 1, "Electron2_mvaHZZIso" },
        { "Electron1_HzzID_WP2022", "Electron_HzzID_WP2022[0]", 2, 0, 2, "Electron1_HzzID_WP2022" },
        { "Electron2_HzzID_WP2022", "Electron_HzzID_WP2022[1]", 2, 0, 2, "Electron2_HzzID_WP2022" },
        { "Electron_photonIdx_nMatch", "Electron_photonIdx_nMatch", 6, 0, 6, "Electron_photonIdx_nMatch" },
        { "Electron_photonIdx_nMatch / nElectron", "Electron_photonIdx_nMatch/nElectron", 10, 0, 1, "Electron_photonIdx_nMatch / N_{e}" },
        { "Photon_electronIdx_nMatch", "Photon_electronIdx_nMatch", 6, 0, 6, "Photon_electronIdx_nMatch" },
        { "nPhoton", "nPhoton", 6, 0, 6, "N_{#gamma}" },
        { "Photon_electronIdx_nMatch", "Photon_electronIdx_nMatch", 6, 0, 6, "Photon_electronIdx_nMatch" },
        { "Photon_electronIdx_nMatch / nPhoton", "Photon_electronIdx_nMatch/nPhoton", 10, 0, 1, "Photon_electronIdx_nMatch / N_{#gamma}" },
        { "Photon1_pt", "Photon_pt[0]", 200, 0, 200, "Photon1_pt"},
        { "Photon1_eta", "Photon_eta[0]", 60, -3, 3, "Photon1_eta"},
      }
    },

    {
      "/pnfs/knu.ac.kr/data/cms/store/user/sucho/reco_eobject/data/list_2023_EGamma_NanoAODv15_reco.txt",
      "/pnfs/knu.ac.kr/data/cms/store/user/sucho/reco_eobject/data/list_2024H_EGamma_PromptReco_reco.txt",
      "Events",
      "EGamma_23_vs_24H",
      "23C_EG",
      "24H_EG",
      "nElectron>=2 && nPhoton>= 1", // baseCut
      false, "", 0, 0, // blind off

      0,         // Normalization mode 1 = Lumi, 0 = Event
      18.063,    // 2023 Lumi
      108.95,    // 2024 Lumi

      {
        { "nElectron",  "nElectron",  7, 0, 7, "N_{e}" },
        { "Electron1_pt", "Electron_pt[0]", 300, 0, 300, "Electron_pt1 [GeV]" },
        { "Electron2_pt", "Electron_pt[1]", 100, 0, 100, "Electron_pt2 [GeV]" },
        { "Electron1_SCeta", "Electron_eta[0] + Electron_deltaEtaSC[0]", 60, -3, 3, "Electron1_SCeta" },
        { "Electron2_SCeta", "Electron_eta[1] + Electron_deltaEtaSC[1]", 60, -3, 3, "Electron2_SCeta" },
        { "Electron1_mvaHZZIso", "Electron_mvaHZZIso[0]", 20, -1, 1, "Electron1_mvaHZZIso" },
        { "Electron2_mvaHZZIso", "Electron_mvaHZZIso[1]", 20, -1, 1, "Electron2_mvaHZZIso" },
        { "Electron1_HzzID_WP2022", "Electron_HzzID_WP2022[0]", 2, 0, 2, "Electron1_HzzID_WP2022" },
        { "Electron2_HzzID_WP2022", "Electron_HzzID_WP2022[1]", 2, 0, 2, "Electron2_HzzID_WP2022" },
        { "Electron_photonIdx_nMatch", "Electron_photonIdx_nMatch", 6, 0, 6, "Electron_photonIdx_nMatch" },
        { "Electron_photonIdx_nMatch / nElectron", "Electron_photonIdx_nMatch/nElectron", 10, 0, 1, "Electron_photonIdx_nMatch / N_{e}" },
        { "Photon_electronIdx_nMatch", "Photon_electronIdx_nMatch", 6, 0, 6, "Photon_electronIdx_nMatch" },
        { "nPhoton", "nPhoton", 6, 0, 6, "N_{#gamma}" },
        { "Photon_electronIdx_nMatch", "Photon_electronIdx_nMatch", 6, 0, 6, "Photon_electronIdx_nMatch" },
        { "Photon_electronIdx_nMatch / nPhoton", "Photon_electronIdx_nMatch/nPhoton", 10, 0, 1, "Photon_electronIdx_nMatch / N_{#gamma}" },
        { "Photon1_pt", "Photon_pt[0]", 200, 0, 200, "Photon1_pt"},
        { "Photon1_eta", "Photon_eta[0]", 60, -3, 3, "Photon1_eta"},
      }
    },

    {
      "/pnfs/knu.ac.kr/data/cms/store/user/sucho/reco_eobject/data/list_2023_EGamma_NanoAODv15_reco.txt",
      "/pnfs/knu.ac.kr/data/cms/store/user/sucho/reco_eobject/data/list_2024I_EGamma_PromptReco_reco.txt",
      "Events",
      "EGamma_23_vs_24I",
      "23C_EG",
      "24I_EG",
      "nElectron>=2 && nPhoton>= 1", // baseCut
      false, "", 0, 0, // blind off

      0,         // Normalization mode 1 = Lumi, 0 = Event
      18.063,    // 2023 Lumi
      108.95,    // 2024 Lumi

      {
        { "nElectron",  "nElectron",  7, 0, 7, "N_{e}" },
        { "Electron1_pt", "Electron_pt[0]", 300, 0, 300, "Electron_pt1 [GeV]" },
        { "Electron2_pt", "Electron_pt[1]", 100, 0, 100, "Electron_pt2 [GeV]" },
        { "Electron1_SCeta", "Electron_eta[0] + Electron_deltaEtaSC[0]", 60, -3, 3, "Electron1_SCeta" },
        { "Electron2_SCeta", "Electron_eta[1] + Electron_deltaEtaSC[1]", 60, -3, 3, "Electron2_SCeta" },
        { "Electron1_mvaHZZIso", "Electron_mvaHZZIso[0]", 20, -1, 1, "Electron1_mvaHZZIso" },
        { "Electron2_mvaHZZIso", "Electron_mvaHZZIso[1]", 20, -1, 1, "Electron2_mvaHZZIso" },
        { "Electron1_HzzID_WP2022", "Electron_HzzID_WP2022[0]", 2, 0, 2, "Electron1_HzzID_WP2022" },
        { "Electron2_HzzID_WP2022", "Electron_HzzID_WP2022[1]", 2, 0, 2, "Electron2_HzzID_WP2022" },
        { "Electron_photonIdx_nMatch", "Electron_photonIdx_nMatch", 6, 0, 6, "Electron_photonIdx_nMatch" },
        { "Electron_photonIdx_nMatch / nElectron", "Electron_photonIdx_nMatch/nElectron", 10, 0, 1, "Electron_photonIdx_nMatch / N_{e}" },
        { "Photon_electronIdx_nMatch", "Photon_electronIdx_nMatch", 6, 0, 6, "Photon_electronIdx_nMatch" },
        { "nPhoton", "nPhoton", 6, 0, 6, "N_{#gamma}" },
        { "Photon_electronIdx_nMatch", "Photon_electronIdx_nMatch", 6, 0, 6, "Photon_electronIdx_nMatch" },
        { "Photon_electronIdx_nMatch / nPhoton", "Photon_electronIdx_nMatch/nPhoton", 10, 0, 1, "Photon_electronIdx_nMatch / N_{#gamma}" },
        { "Photon1_pt", "Photon_pt[0]", 200, 0, 200, "Photon1_pt"},
        { "Photon1_eta", "Photon_eta[0]", 60, -3, 3, "Photon1_eta"},
      }
    },
*/
    
    { // 2023C vs 2024TOT
      "/pnfs/knu.ac.kr/data/cms/store/user/sucho/reco_eobject/data/list_2023_EGamma_NanoAODv15_reco.txt",
      "/pnfs/knu.ac.kr/data/cms/store/user/sucho/reco_eobject/data/list_2024TOT_EGamma_PromptReco_reco.txt",
      "Events",
      "EGamma_23_vs_24TOT",
      "23C_EG",
      "24TOT_EG",
      "nElectron>=2 && nPhoton>= 1", // baseCut
      false, "", 0, 0, // blind off

      0,         // Normalization mode 1 = Lumi, 0 = Event
      18.063,    // 2023 Lumi
      108.95,    // 2024 Lumi

      {
        { "nElectron",  "nElectron",  7, 0, 7, "N_{e}" },
        { "Electron1_pt", "Electron_pt[0]", 300, 0, 300, "Electron_pt1 [GeV]" },
        { "Electron2_pt", "Electron_pt[1]", 100, 0, 100, "Electron_pt2 [GeV]" },
        { "Electron1_SCeta", "Electron_eta[0] + Electron_deltaEtaSC[0]", 60, -3, 3, "Electron1_SCeta" },
        { "Electron2_SCeta", "Electron_eta[1] + Electron_deltaEtaSC[1]", 60, -3, 3, "Electron2_SCeta" },
        { "Electron1_mvaHZZIso", "Electron_mvaHZZIso[0]", 20, -1, 1, "Electron1_mvaHZZIso" },
        { "Electron2_mvaHZZIso", "Electron_mvaHZZIso[1]", 20, -1, 1, "Electron2_mvaHZZIso" },
        { "Electron1_HzzID_WP2022", "Electron_HzzID_WP2022[0]", 2, 0, 2, "Electron1_HzzID_WP2022" },
        { "Electron2_HzzID_WP2022", "Electron_HzzID_WP2022[1]", 2, 0, 2, "Electron2_HzzID_WP2022" },
        { "Electron_photonIdx_nMatch", "Electron_photonIdx_nMatch", 6, 0, 6, "Electron_photonIdx_nMatch" },
        { "Electron_photonIdx_nMatch / nElectron", "Electron_photonIdx_nMatch/nElectron", 10, 0, 1, "Electron_photonIdx_nMatch / N_{e}" },
        { "Photon_electronIdx_nMatch", "Photon_electronIdx_nMatch", 6, 0, 6, "Photon_electronIdx_nMatch" },
        { "nPhoton", "nPhoton", 6, 0, 6, "N_{#gamma}" },
        { "Photon_electronIdx_nMatch", "Photon_electronIdx_nMatch", 6, 0, 6, "Photon_electronIdx_nMatch" },
        { "Photon_electronIdx_nMatch / nPhoton", "Photon_electronIdx_nMatch/nPhoton", 10, 0, 1, "Photon_electronIdx_nMatch / N_{#gamma}" },
        { "Photon1_pt", "Photon_pt[0]", 200, 0, 200, "Photon1_pt"},
        { "Photon1_eta", "Photon_eta[0]", 60, -3, 3, "Photon1_eta"},
      }
    },
  };

  // ------------------------------------------------------------
  // 3) Compare jobs (blind)
  //   - blind는 expr == blindVar 인 plot에만 적용
  // ------------------------------------------------------------
  std::vector<CompareJob> blindCompareJobs = {
    {
      "/pnfs/knu.ac.kr/data/cms/store/user/sucho/reco/data/list_2023_Muon_reco.txt",
      "/pnfs/knu.ac.kr/data/cms/store/user/sucho/reco/data/list_2024_Muon_reco.txt",
      "Events",
      "Muon_23_vs_24_blind",
      "2023 Muon",
      "2024 Muon",
      "Z_mass_mm + Z_mass_mmg > 185", // baseCut
      true,          // useBlind
      "Z_mass_mmg",  // blindVar (expr과 정확히 같아야 적용)
      120, 130,      // blind range

      1,         // Normalization mode 1 = Lumi, 0 = Event
      18.063,    // 2023 Lumi
      108.95,    // 2024 Lumi

      {
        { "Z_mass_mm",  "Z_mass_mm",  40, 70, 110, "m_{#mu#mu} [GeV]" },
        { "Z_mass_mmg", "Z_mass_mmg", 80, 80, 200, "m_{#mu#mug} [GeV]" }, // <- blind box
        { "dR_mg",      "dR_mg",      40,  0,   4, "#DeltaR(#mu,#gamma)" }
      }
    }
  };

  // ------------------------------------------------------------
  // Run
  // ------------------------------------------------------------

  // 필요하면 켜기
  // (Form 포인터를 직접 넘기지 말고, TString에 담아서 .Data()로 넘기기)
  TString onechainDir = Form("%s/onechain", outdir);
  // plot1ChainPDF(oneChainJobs, onechainDir.Data());
  
  // 필요하면 켜기
  TString compareDir = Form("%s/compare", outdir);
  plotCompareRatioPDF(compareJobs, compareDir.Data());
  
  // 필요하면 켜기
  TString blindDir = Form("%s/blind", outdir);
  // plotCompareRatioBlindPDF(blindCompareJobs, blindDir.Data(),
  //                          true,  // normIncludeBlindRegion
  //                          true); // drawBlindBox

}

// alias (선택)
void runPalette(const char* outdir = "plots_out") { palette_all(outdir); }

