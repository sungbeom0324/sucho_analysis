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
#include "src/PlotComparePDF_3.C"

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
      "/pnfs/knu.ac.kr/data/cms/store/user/sucho/reco/data/list_2024TOT_Muon_PromptReco_reco.txt",
      "Events",
      "Run2024_PromptReco_Muon",
      "", // baseCut
      { // 1D plots
        //{ "Z_mass_mm",  "Z_mass_mm",  60,  60, 120, "m_{#mu#mu} [GeV]" },
        //{ "Z_mass_mmg", "Z_mass_mmg", 80,  80, 200, "m_{#mu#mug} [GeV]" },
        //{ "dR_mg",      "dR_mg",      40,   0,   4, "#DeltaR(#mu,#gamma)" }
      },
      { // 2D plots
        { "Zmmg_vs_Zmm",
          "Z_mass_mmg:Z_mass_mm",
          30, 75, 105, // x
          95, 90, 185, // y
          "m_{#mu#mu} [GeV]",
          "m_{#mu#mug} [GeV]",
          "" }
      }
    },

    {
      "/pnfs/knu.ac.kr/data/cms/store/user/sucho/reco/data/list_2024TOT_EGamma_PromptReco_reco.txt",
      "Events",
      "Run2024_PromptReco_EGamma",
      "", // baseCut
      { // 1D plots
        //{ "Z_mass_ee",  "Z_mass_ee",  60,  60, 120, "m_{ee} [GeV]" },
        //{ "Z_mass_eeg", "Z_mass_eeg", 80,  80, 200, "m_{ee#gamma} [GeV]" },
        //{ "dR_eg",      "dR_eg",      40,   0,   4, "#DeltaR(e,#gamma)" }
      },
      { // 2D plots
        { "Zeeg_vs_Zee",
          "Z_mass_eeg:Z_mass_ee",
          30, 75, 105, // x
          95, 90, 185, // y
          "m_{ee} [GeV]",
          "m_{ee#gamma} [GeV]",
          "" }
      }
    },
  };

  // ------------------------------------------------------------
  // 2) Compare_2Chain jobs (non-blind)
  // ------------------------------------------------------------
  std::vector<CompareJob> compareJobs = {

    { // NanoAOD version (EGamma)
      "/pnfs/knu.ac.kr/data/cms/store/user/sucho/reco_Feb03/data/list_2023TOT_EGamma_NanoAODv12_reco.txt",
      "/pnfs/knu.ac.kr/data/cms/store/user/sucho/reco_Feb03/data/list_2023TOT_EGamma_NanoAODv15_reco.txt",
      "Events",
      "23TOT_EGamma_v12_vs_v15",
      "23EG_v12",
      "23EG_v15",
      "", // baseCut
      "txt_baseline",
      false, "", 0, 0, // blind off

      1,         // Normalization mode 1 = Lumi, 0 = Event
      27.756,     // 2023TOT Lumi
      27.756,    //  Lumi

      {
        { "Z_mass_ee",  "Z_mass_ee",  60,  60, 120, "m_{ee} [GeV]" },
        { "Z_mass_eeg", "Z_mass_eeg", 80,  80, 200, "m_{ee#gamma} [GeV]" },
        { "dR_eg",      "dR_eg",      40,   0,   4, "#DeltaR(e,#gamma)" },
        { "PV_npvsGood", "PV_npvsGood", 100, 0, 100, "PV_npvsGood"},
        { "AddPhoton_pt/Z_mass_eeg", "AddPhoton_pt/Z_mass_eeg", 60, 0, 0.6, "p^{#gamma}_{T}/m_{ee#gamma}"},
        { "nGoodEle", "nGoodEle", 5,0,5},
        { "Electron1_pt", "Electron_pt[leadEleIdx]", 200, 0, 200, "Electron1_pt"},
        { "Electron2_pt", "Electron_pt[subleadEleIdx]", 100, 0, 100, "Electron2_pt"},
        { "Electron1_SCeta", "Electron_eta[leadEleIdx]+Electron_deltaEtaSC[leadEleIdx]", 60, -3, 3, "Electron1_SCeta"},
        { "Electron2_SCeta", "Electron_eta[subleadEleIdx]+Electron_deltaEtaSC[subleadEleIdx]", 60, -3, 3,"Electron2_SCEta"},
        { "Electron1_mvaHZZIso", "Electron_mvaHZZIso[leadEleIdx]", 20, -1, 1, "Electron1_mvaHZZIso" },
        { "Electron2_mvaHZZIso", "Electron_mvaHZZIso[subleadEleIdx]", 20, -1, 1, "Electron2_mvaHZZIso" },
        { "nGoodPhoton", "nGoodPhoton", 5, 0, 5 },
        { "AddPhoton_pt", "AddPhoton_pt", 100, 0, 100 },
        { "AddPhoton_eta", "AddPhoton_eta", 60, -3, 3 },
      }
    },

    { // NanoAOD version (Muon)
      "/pnfs/knu.ac.kr/data/cms/store/user/sucho/reco_Feb03/data/list_2023TOT_Muon_NanoAODv12_reco.txt",
      "/pnfs/knu.ac.kr/data/cms/store/user/sucho/reco_Feb03/data/list_2023TOT_Muon_NanoAODv15_reco.txt",
      "Events",
      "23TOT_Muon_v12_vs_v15",
      "23Muon_v12",
      "23Muon_v15",
      "", // baseCut
      "txt_baseline",
      false, "", 0, 0, // blind off

      1,         // Normalization mode 1 = Lumi, 0 = Event
      27.756,     // 2023TOT Lumi
      27.756,    //  Lumi

      {
        { "Z_mass_mm",  "Z_mass_mm",  60,  60, 120, "m_{#mu#mu} [GeV]" },
        { "Z_mass_mmg", "Z_mass_mmg", 80,  80, 200, "m_{#mu#mu#gamma} [GeV]" },
        { "dR_mg",      "dR_mg",      40,   0,   4, "#DeltaR(#mu,#gamma)" },
        { "PV_npvsGood", "PV_npvsGood", 100, 0, 100, "PV_npvsGood"},
        { "AddPhoton_pt/Z_mass_mmg", "AddPhoton_pt/Z_mass_mmg", 60, 0, 0.6, "p^{#gamma}_{T}/m_{#mu#mu#gamma}"},
        { "nGoodMuon", "nGoodMuon", 5,0,5},
        { "Muon1_pt", "Muon_pt[leadMuonIdx]", 200, 0, 200, "Muon1_pt"},
        { "Muon2_pt", "Muon_pt[subleadMuonIdx]", 100, 0, 100, "Muon2_pt"},
        { "Muon1_SCeta", "Muon_eta[leadMuonIdx]", 60, -3, 3, "Muon1_eta"},
        { "Muon2_SCeta", "Muon_eta[subleadMuonIdx]", 60, -3, 3,"Muon2_eta"},
        { "nGoodPhoton", "nGoodPhoton", 5, 0, 5 },
        { "AddPhoton_pt", "AddPhoton_pt", 100, 0, 100 },
        { "AddPhoton_eta", "AddPhoton_eta", 60, -3, 3 },
      }
    },

    { // 2023_vs_2024 (EGamma)
      "/pnfs/knu.ac.kr/data/cms/store/user/sucho/reco_Feb03/data/list_2023TOT_EGamma_NanoAODv12_reco.txt",
      "/pnfs/knu.ac.kr/data/cms/store/user/sucho/reco_Feb03/data/list_2024TOT_EGamma_PromptReco_reco.txt",
      "Events",
      "23EG_vs_v24EG",
      "23EG",
      "24EG",
      "", // baseCut
      "txt_baseline",
      false, "", 0, 0, // blind off

      1,         // Normalization mode 1 = Lumi, 0 = Event
      27.756,     // 2023CD
      108.95,    //  2024TOT

      {
        { "Z_mass_ee",  "Z_mass_ee",  60,  60, 120, "m_{ee} [GeV]" },
        { "Z_mass_eeg", "Z_mass_eeg", 80,  80, 200, "m_{ee#gamma} [GeV]" },
        { "dR_eg",      "dR_eg",      40,   0,   4, "#DeltaR(e,#gamma)" },
        { "PV_npvsGood", "PV_npvsGood", 100, 0, 100, "PV_npvsGood"},
        { "AddPhoton_pt/Z_mass_eeg", "AddPhoton_pt/Z_mass_eeg", 60, 0, 0.6, "p^{#gamma}_{T}/m_{ee#gamma}"},
        { "nGoodEle", "nGoodEle", 5,0,5},
        { "Electron1_pt", "Electron_pt[leadEleIdx]", 200, 0, 200, "Electron1_pt"},
        { "Electron2_pt", "Electron_pt[subleadEleIdx]", 100, 0, 100, "Electron2_pt"},
        { "Electron1_SCeta", "Electron_eta[leadEleIdx]+Electron_deltaEtaSC[leadEleIdx]", 60, -3, 3, "Electron1_SCeta"},
        { "Electron2_SCeta", "Electron_eta[subleadEleIdx]+Electron_deltaEtaSC[subleadEleIdx]", 60, -3, 3,"Electron2_SCEta"},
        { "Electron1_mvaHZZIso", "Electron_mvaHZZIso[leadEleIdx]", 20, -1, 1, "Electron1_mvaHZZIso" },
        { "Electron2_mvaHZZIso", "Electron_mvaHZZIso[subleadEleIdx]", 20, -1, 1, "Electron2_mvaHZZIso" },
        { "nGoodPhoton", "nGoodPhoton", 5, 0, 5 },
        { "AddPhoton_pt", "AddPhoton_pt", 100, 0, 100 },
        { "AddPhoton_eta", "AddPhoton_eta", 60, -3, 3 },
      }
    },

    { // 2023_vs_2024 (Muon)
      "/pnfs/knu.ac.kr/data/cms/store/user/sucho/reco_Feb03/data/list_2023TOT_Muon_NanoAODv12_reco.txt",
      "/pnfs/knu.ac.kr/data/cms/store/user/sucho/reco_Feb03/data/list_2024TOT_Muon_PromptReco_reco.txt",
      "Events",
      "23Muon_vs_24Muon",
      "23Muon",
      "24Muon",
      "", // baseCut
      "txt_baseline",
      false, "", 0, 0, // blind off

      1,         // Normalization mode 1 = Lumi, 0 = Event
      27.756,     // 2023CD
      108.95,     // 2024TOT

      {
        { "Z_mass_mm",  "Z_mass_mm",  60,  60, 120, "m_{#mu#mu} [GeV]" },
        { "Z_mass_mmg", "Z_mass_mmg", 80,  80, 200, "m_{#mu#mu#gamma} [GeV]" },
        { "dR_mg",      "dR_mg",      40,   0,   4, "#DeltaR(#mu,#gamma)" },
        { "PV_npvsGood", "PV_npvsGood", 100, 0, 100, "PV_npvsGood"},
        { "AddPhoton_pt/Z_mass_mmg", "AddPhoton_pt/Z_mass_mmg", 60, 0, 0.6, "p^{#gamma}_{T}/m_{#mu#mu#gamma}"},
        { "nGoodMuon", "nGoodMuon", 5,0,5},
        { "Muon1_pt", "Muon_pt[leadMuonIdx]", 200, 0, 200, "Muon1_pt"},
        { "Muon2_pt", "Muon_pt[subleadMuonIdx]", 100, 0, 100, "Muon2_pt"},
        { "Muon1_SCeta", "Muon_eta[leadMuonIdx]", 60, -3, 3, "Muon1_eta"},
        { "Muon2_SCeta", "Muon_eta[subleadMuonIdx]", 60, -3, 3,"Muon2_eta"},
        { "nGoodPhoton", "nGoodPhoton", 5, 0, 5 },
        { "AddPhoton_pt", "AddPhoton_pt", 100, 0, 100 },
        { "AddPhoton_eta", "AddPhoton_eta", 60, -3, 3 },
      }
    },
  };

  // ------------------------------------------------------------
  // 3) Compare_3Chain : jobs (non-blind)
  // ------------------------------------------------------------

  std::vector<CompareJob3> CompareJob3 = {
    { // EGamma 22EE_23_24
      "/pnfs/knu.ac.kr/data/cms/store/user/sucho/reco/data/list_2022EFG_EGamma_NanoAODv12_reco.txt",
      "/pnfs/knu.ac.kr/data/cms/store/user/sucho/reco/data/list_2023TOT_EGamma_NanoAODv12_reco.txt",
      "/pnfs/knu.ac.kr/data/cms/store/user/sucho/reco/data/list_2024TOT_EGamma_PromptReco_reco.txt",
      "Events",

      "EGamma_2022EFG_2023TOT_2024TOT",
      "22EE_EG", 
      "23EG",
      "24EG",

      "", // baseCut
      "txt_baseline", // Text key
      false, "", 0, 0, // blind off
      
      1,
      26.6717,
      27.756,
      108.95,

      { // Vector of 1D Histogramas.
        { "Z_mass_ee",  "Z_mass_ee",  60,  60, 120, "m_{ee} [GeV]" },
        { "Z_mass_eeg", "Z_mass_eeg", 80,  80, 200, "m_{ee#gamma} [GeV]" },
        { "dR_eg",      "dR_eg",      40,   0,   4, "#DeltaR(e,#gamma)" },
        { "PV_npvs", "PV_npvs", 100, 0, 100, "PV_npvs"},
        { "PV_npvsGood", "PV_npvsGood", 100, 0, 100, "PV_npvsGood"},
        { "AddPhoton_pt/Z_mass_eeg", "AddPhoton_pt/Z_mass_eeg", 60, 0, 0.6, "p^{#gamma}_{T}/m_{ee#gamma}"},
        { "nGoodEle", "nGoodEle", 5,0,5},
        { "Electron1_pt", "Electron_pt[leadEleIdx]", 200, 0, 200, "Electron1_pt"},
        { "Electron2_pt", "Electron_pt[subleadEleIdx]", 100, 0, 100, "Electron2_pt"},
        { "Electron1_SCeta", "Electron_eta[leadEleIdx]+Electron_deltaEtaSC[leadEleIdx]", 60, -3, 3, "Electron1_SCeta"},
        { "Electron2_SCeta", "Electron_eta[subleadEleIdx]+Electron_deltaEtaSC[subleadEleIdx]", 60, -3, 3,"Electron2_SCEta"},
        { "Electron1_mvaHZZIso", "Electron_mvaHZZIso[leadEleIdx]", 20, 0.8, 1, "Electron1_mvaHZZIso" },
        { "Electron2_mvaHZZIso", "Electron_mvaHZZIso[subleadEleIdx]", 20, 0.8, 1, "Electron2_mvaHZZIso" },
        { "nGoodPhoton", "nGoodPhoton", 5, 0, 5 },
        { "AddPhoton_pt", "AddPhoton_pt", 100, 0, 100 },
        { "AddPhoton_eta", "AddPhoton_eta", 60, -3, 3 },
      }
      },

    { // Muon 22EE_23_24
      "/pnfs/knu.ac.kr/data/cms/store/user/sucho/reco/data/list_2022EFG_Muon_NanoAODv12_reco.txt",
      "/pnfs/knu.ac.kr/data/cms/store/user/sucho/reco/data/list_2023TOT_Muon_NanoAODv12_reco.txt",
      "/pnfs/knu.ac.kr/data/cms/store/user/sucho/reco/data/list_2024TOT_Muon_PromptReco_reco.txt",
      "Events",

      "Muon_2022EFG_2023TOT_2024TOT",
      "22EE_Muon", 
      "23Muon",
      "24Muon",

      "", // baseCut
      "txt_baseline", // Text key
      false, "", 0, 0, // blind off
      
      1,
      26.6717,
      27.756,
      108.95,

      { // Vector of 1D Histogramas.
        { "Z_mass_mm",  "Z_mass_mm",  60,  60, 120, "m_{#mu#mu} [GeV]" },
        { "Z_mass_mmg", "Z_mass_mmg", 80,  80, 200, "m_{#mu#mu#gamma} [GeV]" },
        { "dR_mg",      "dR_mg",      40,   0,   4, "#DeltaR(#mu,#gamma)" },
        { "PV_npvs", "PV_npvs", 100, 0, 100, "PV_npvs"},
        { "PV_npvsGood", "PV_npvsGood", 100, 0, 100, "PV_npvsGood"},
        { "AddPhoton_pt/Z_mass_mmg", "AddPhoton_pt/Z_mass_mmg", 60, 0, 0.6, "p^{#gamma}_{T}/m_{#mu#mu#gamma}"},
        { "nGoodMuon", "nGoodMuon", 5,0,5},
        { "Muon1_pt", "Muon_pt[leadMuonIdx]", 200, 0, 200, "Muon1_pt"},
        { "Muon2_pt", "Muon_pt[subleadMuonIdx]", 100, 0, 100, "Muon2_pt"},
        { "Muon1_SCeta", "Muon_eta[leadMuonIdx]", 60, -3, 3, "Muon1_eta"},
        { "Muon2_SCeta", "Muon_eta[subleadMuonIdx]", 60, -3, 3,"Muon2_eta"},
        { "nGoodPhoton", "nGoodPhoton", 5, 0, 5 },
        { "AddPhoton_pt", "AddPhoton_pt", 100, 0, 100 },
        { "AddPhoton_eta", "AddPhoton_eta", 60, -3, 3 },
      }
      },

    };
  // ------------------------------------------------------------
  // Run
  // ------------------------------------------------------------

  // 필요하면 켜기
  // (Form 포인터를 직접 넘기지 말고, TString에 담아서 .Data()로 넘기기)
  TString onechainDir = Form("%s/onechain", outdir);
  //plot1ChainPDF(oneChainJobs, onechainDir.Data());
  
  // 필요하면 켜기
  TString compareDir = Form("%s/compare", outdir);
  //plotCompareRatioPDF(compareJobs, compareDir.Data());

  // 필요하면 켜기
  TString Compare3Dir = Form("%s/compare3", outdir);
  PlotComparePDF_3(CompareJob3, Compare3Dir.Data());
}

// alias (선택)
void runPalette(const char* outdir = "plots_out") { palette_all(outdir); }

