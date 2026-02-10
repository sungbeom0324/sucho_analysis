#include "include/PlotTypes.h"
#include "include/PlotUtils.h"
#include "include/DrawCMSLabel.h"
#include "include/PlotTextBlock.h"

#include <TROOT.h>
#include <TSystem.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TH1D.h>
#include <TH2D.h>

#include <iostream>
#include <string>

void plot1ChainPDF(const std::vector<OneChainJob>& jobs, const char* outdir) {
  gROOT->SetBatch(kTRUE);
  gSystem->mkdir(outdir, kTRUE);
  
  gStyle->SetTitleY(0.97);
  gStyle->SetOptStat(1110);
  gStyle->SetStatX(0.88); // Stat right edge. 1 is closer to colour bar. 
  gStyle->SetStatY(0.88);
  gStyle->SetStatColor(0);

  for (const auto& job : jobs) {
    TChain ch(job.treename.c_str());
    int nAdded = addFilesFromList(ch, job.listfile);
    if (nAdded <= 0) {
      std::cerr << "[ERROR] no files added for job: " << job.tag << "\n";
      continue;
    }

    const std::string outpdf = std::string(outdir) + "/" + job.tag + ".pdf";
    TCanvas c("c", "c", 900, 800);
    c.Print((outpdf + "[").c_str());

    // ----- 1D -----
    for (const auto& h : job.h1) {
      TH1D hist("h1_tmp", "", h.nbins, h.xmin, h.xmax);
      hist.Sumw2();

      const std::string cut = job.baseCut; // 1chain은 baseCut만 사용 (필요하면 expr별 cut은 Hist2D처럼 확장 가능)
      ch.Draw(Form("%s>>h1_tmp", h.expr.c_str()), cut.c_str(), "goff");

      c.Clear();
      hist.SetLineWidth(2);
      hist.GetYaxis()->SetTitle("Events");
      hist.GetXaxis()->SetTitle((h.xTitle.empty() ? h.expr : h.xTitle).c_str());
      hist.Draw("hist");
      c.Print(outpdf.c_str());
    }

    // ----- 2D -----
    for (const auto& h : job.h2) {
      TH2D hist("h2_tmp", "", h.nbx, h.xmin, h.xmax, h.nby, h.ymin, h.ymax);
      hist.Sumw2();

      std::string cut = wrapAnd(job.baseCut, h.cut);

      ch.Draw(Form("%s>>h2_tmp", h.exprYX.c_str()), cut.c_str(), "goff");

      std::string autoY, autoX;
      splitYX(h.exprYX, autoY, autoX);

      c.Clear();
      hist.SetTitle(job.tag.c_str());
      hist.GetXaxis()->SetTitle((h.xTitle.empty() ? autoX : h.xTitle).c_str());
      hist.GetYaxis()->SetTitle((h.yTitle.empty() ? autoY : h.yTitle).c_str());
      //hist.SetStats(0); // Do not show Stats.
      hist.Draw("COLZ");
        
      // DrawCMSLabel(0.12, 0.96, "Run3", true);

      c.Print(outpdf.c_str());
    }

    c.Print((outpdf + "]").c_str());
    std::cout << "[INFO] wrote " << outpdf << "\n";
  }
}

