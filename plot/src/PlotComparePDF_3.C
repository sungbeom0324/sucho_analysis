#include "include/PlotTypes.h"
#include "include/PlotUtils.h"
#include "include/DrawCMSLabel.h"
#include "include/PlotTextBlock.h"
#include "include/DrawLumiTopRight.h"

#include <TROOT.h>
#include <TSystem.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TLegend.h>
#include <TLine.h>
#include <TH1D.h>

#include <iostream>
#include <string>
#include <algorithm>

void PlotComparePDF_3(const std::vector<CompareJob3>& jobs, const char* outdir) {
  gROOT->SetBatch(kTRUE);
  gSystem->mkdir(outdir, kTRUE);

  for (const auto& job : jobs) {
    TChain chA(job.treename.c_str());
    TChain chB(job.treename.c_str());
    TChain chC(job.treename.c_str());

    int nA = addFilesFromList(chA, job.listA);
    int nB = addFilesFromList(chB, job.listB);
    int nC = addFilesFromList(chC, job.listC);
    if (nA <= 0 || nB <= 0 || nC <=0) {
      std::cerr << "[ERROR] files added A=" << nA << " B=" << nB << "C=" << nC << " (tag=" << job.tag << ")\n";
      continue;
    }

    const std::string outpdf = std::string(outdir) + "/" + job.tag + ".pdf";
    TCanvas c("c", "c", 900, 800);
    c.Print((outpdf + "[").c_str());

    for (const auto& h : job.h1) {
      TH1D hA("hA", "", h.nbins, h.xmin, h.xmax);
      TH1D hB("hB", "", h.nbins, h.xmin, h.xmax);
      TH1D hC("hC", "", h.nbins, h.xmin, h.xmax);
      hA.Sumw2(); hB.Sumw2(); hC.Sumw2();

      chA.Draw(Form("%s>>hA", h.expr.c_str()), job.baseCut.c_str(), "goff");
      chB.Draw(Form("%s>>hB", h.expr.c_str()), job.baseCut.c_str(), "goff");
      chC.Draw(Form("%s>>hC", h.expr.c_str()), job.baseCut.c_str(), "goff");

      // event-count normalize
      applyNorm(hA, job.normMode, job.lumiA_fb);
      applyNorm(hB, job.normMode, job.lumiB_fb);
      applyNorm(hC, job.normMode, job.lumiC_fb);

      hA.SetLineColor(kRed + 1);
      hB.SetLineColor(kBlue + 1);
      hC.SetLineColor(kGreen + 1);
      hA.SetLineWidth(2);
      hB.SetLineWidth(2);
      hC.SetLineWidth(2);
      hA.SetMinimum(0.0);
      hB.SetMinimum(0.0);
      hC.SetMinimum(0.0);

      c.Clear();
      TPad p1("p1","p1",0,0.30,1,1);
      TPad p2("p2","p2",0,0.00,1,0.30);
      p1.SetBottomMargin(0.02);
      p2.SetTopMargin(0.02);
      p2.SetBottomMargin(0.30);
      p1.Draw(); p2.Draw();

      // Main plot
      p1.cd();
      hA.GetXaxis()->SetLabelSize(0);
      hA.GetXaxis()->SetTitle("");
      hA.GetYaxis()->SetTitle(yTitleFromNorm(job.normMode));
      hA.SetStats(0);
      hB.SetStats(0);
      hC.SetStats(0);

      double ymax = std::max(hA.GetMaximum(), hB.GetMaximum());
      if (ymax <= 0) ymax = 1.0;
      hA.SetMaximum(1.25 * ymax);

      hA.Draw("hist");
      hB.Draw("hist same");
      hC.Draw("hist same");

      TLegend leg(0.12, 0.76, 0.42, 0.88);
      leg.SetBorderSize(0);
      leg.SetTextSize(0.032);
      leg.AddEntry(&hA, job.labelA.c_str(), "l");
      leg.AddEntry(&hB, job.labelB.c_str(), "l");
      leg.AddEntry(&hB, job.labelC.c_str(), "l");
      leg.Draw();

      DrawCMSLabel(0.12, 0.96, "Run3", true); 
      DrawLumiTopRight(job.lumiA_fb, job.lumiB_fb, job.labelA.c_str(), job.labelB.c_str(), job.labelC.c_str());

      auto lines = GetPlotTextPreset(job.textKey);
      DrawTextBlockTR(lines);

      // Ratio sub-plot
      p2.cd();
      TH1D* r = makeRatio(hB, hA, "ratio");
      r->SetMarkerStyle(20);
      r->SetMarkerSize(0.6);
      r->SetLineWidth(2);

      r->GetYaxis()->SetTitle((job.labelB + " / " + job.labelA).c_str());
      r->GetYaxis()->SetNdivisions(505);
      r->GetYaxis()->SetTitleSize(0.10);
      r->GetYaxis()->SetTitleOffset(0.45);
      r->GetYaxis()->SetLabelSize(0.08);

      r->GetXaxis()->SetTitle((h.xTitle.empty() ? h.expr : h.xTitle).c_str());
      r->GetXaxis()->SetTitleSize(0.12);
      r->GetXaxis()->SetLabelSize(0.10);

      r->SetMinimum(0.5);
      r->SetMaximum(1.5);
      r->SetStats(0);
      r->Draw("E1");

      TLine line(h.xmin, 1.0, h.xmax, 1.0);
      line.SetLineStyle(2);
      line.Draw();


      c.Print(outpdf.c_str());
      delete r;
    }

    c.Print((outpdf + "]").c_str());
    std::cout << "[INFO] wrote " << outpdf << "\n";
  }
}

