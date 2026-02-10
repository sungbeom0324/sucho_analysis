#ifndef DRAW_LUMI_TOPRIGHT_H
#define DRAW_LUMI_TOPRIGHT_H

#include "TLatex.h"
#include "TString.h"
#include "TStyle.h"

inline void DrawLumiTopRight(double lumiA_fb,
                             double lumiB_fb,
                             const char* labelA,
                             const char* labelB,
                             double y = 0.925,
                             double x = 0.90)
{
  // y=0.965는 보통 "top margin 영역"에 해당 (너 마진에 맞춰 조정 가능)
  TLatex latex;
  latex.SetNDC();
  latex.SetTextAlign(31);      // right-top
  latex.SetTextFont(42);
  latex.SetTextSize(0.035);

  TString line;
  // 예: "23C: 18.1 fb^{-1}, 24TOT: 109 fb^{-1} (13.6 TeV)"
  line = Form("%s: %.1f fb^{-1}, %s: %.1f fb^{-1}",
              labelA, lumiA_fb, labelB, lumiB_fb);

  latex.DrawLatex(x, y, line);
}

#endif
