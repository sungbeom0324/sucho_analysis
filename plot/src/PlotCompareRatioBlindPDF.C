#include "include/PlotTypes.h"
#include "include/PlotUtils.h"

#include <TROOT.h>
#include <TSystem.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TLegend.h>
#include <TLine.h>
#include <TBox.h>
#include <TH1D.h>

#include <iostream>
#include <string>
#include <algorithm>

// normIncludeBlindRegion:
//   true  -> normalize는 baseCut만 기준(블라인드 구간 포함한 적분)
//   false -> normalize도 blindCut 적용(블라인드 구간 제외 적분)
void plotCompareRatioBlindPDF(const std::vector<CompareJob>& jobs,
                              const char* outdir,
                              bool normIncludeBlindRegion = true,
                              bool drawBlindBox = true)
{
  gROOT->SetBatch(kTRUE);
  gSystem->mkdir(outdir, kTRUE);

  for (const auto& job : jobs) {
    TChain chA(job.treename.c_str());
    TChain chB(job.treename.c_str());

    int nA = addFilesFromList(chA, job.listA);
    int nB = addFilesFromList(chB, job.listB);
    if (nA <= 0 || nB <= 0) {
      std::cerr << "[ERROR] files added A=" << nA << " B=" << nB << " (tag=" << job.tag << ")\n";
      continue;
    }

    const bool doBlind = job.useBlind && !job.blindVar.empty() && (job.blindMax > job.blindMin);
    const std::string bcut = doBlind ? blindCut(job.blindVar, job.blindMin, job.blindMax) : "";

    const std::string outpdf = std::string(outdir) + "/" + job.tag + ".pdf";
    TCanvas c("c", "c", 900, 800);
    c.Print((outpdf + "[").c_str());

    for (const auto& h : job.h1) {
      const bool applyBlindThis = doBlind && (h.expr == job.blindVar);

      // plot cut (그릴 때만 blind 제외)
      std::string plot_cut = job.baseCut;
      if (applyBlindThis) plot_cut = wrapAnd(plot_cut, bcut);

      // normalize cut (적분 기준)
      std::string norm_cut = job.baseCut;
      if (!normIncludeBlindRegion && applyBlindThis) norm_cut = wrapAnd(norm_cut, bcut);

      TH1D hAraw("hAraw", "", h.nbins, h.xmin, h.xmax);
      TH1D hBraw("hBraw", "", h.nbins, h.xmin, h.xmax);
      TH1D hA("hA", "", h.nbins, h.xmin, h.xmax);
      TH1D hB("hB", "", h.nbins, h.xmin, h.xmax);
      hAraw.Sumw2(); hBraw.Sumw2(); hA.Sumw2(); hB.Sumw2();

      chA.Draw(Form("%s>>hAraw", h.expr.c_str()), norm_cut.c_str(), "goff");
      chB.Draw(Form("%s>>hBraw", h.expr.c_str()), norm_cut.c_str(), "goff");
      chA.Draw(Form("%s>>hA",    h.expr.c_str()), plot_cut.c_str(), "goff");
      chB.Draw(Form("%s>>hB",    h.expr.c_str()), plot_cut.c_str(), "goff");

      // event-count normalize based on raw integrals
      double intA = integralNoUFOF(hAraw);
      double intB = integralNoUFOF(hBraw);
      if (intA > 0) hA.Scale(1.0 / intA);
      if (intB > 0) hB.Scale(1.0 / intB);

      hA.SetLineColor(kRed + 1);
      hB.SetLineColor(kBlue + 1);
      hA.SetLineWidth(2);
      hB.SetLineWidth(2);

      c.Clear();
      TPad p1("p1","p1",0,0.30,1,1);
      TPad p2("p2","p2",0,0.00,1,0.30);
      p1.SetBottomMargin(0.02);
      p2.SetTopMargin(0.02);
      p2.SetBottomMargin(0.30);
      p1.Draw(); p2.Draw();

      // top
      p1.cd();
      hA.GetXaxis()->SetLabelSize(0);
      hA.GetXaxis()->SetTitle("");
      hA.GetYaxis()->SetTitle("Normalized entries");

      double ymax = std::max(hA.GetMaximum(), hB.GetMaximum());
      if (ymax <= 0) ymax = 1.0;
      hA.SetMaximum(1.25 * ymax);

      hA.Draw("hist");
      hB.Draw("hist same");

      TBox* box = nullptr;
      if (drawBlindBox && applyBlindThis) {
        box = new TBox(job.blindMin, 0.0, job.blindMax, hA.GetMaximum());
        box->SetFillStyle(3004);
        box->SetFillColor(kGray + 2);
        box->SetLineColor(kGray + 2);
        box->Draw("same");
      }

      TLegend leg(0.60,0.75,0.88,0.88);
      leg.SetBorderSize(0);
      leg.AddEntry(&hA, job.labelA.c_str(), "l");
      leg.AddEntry(&hB, job.labelB.c_str(), "l");
      if (applyBlindThis) {
        leg.AddEntry((TObject*)0, Form("Blind: %.0f-%.0f", job.blindMin, job.blindMax), "");
      }
      leg.Draw();

      // ratio
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
      r->Draw("E1");

      TLine line(h.xmin, 1.0, h.xmax, 1.0);
      line.SetLineStyle(2);
      line.Draw();

      c.Print(outpdf.c_str());

      delete r;
      if (box) delete box;
    }

    c.Print((outpdf + "]").c_str());
    std::cout << "[INFO] wrote " << outpdf << "\n";
  }
}

