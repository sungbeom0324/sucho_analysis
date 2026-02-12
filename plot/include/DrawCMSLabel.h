#ifndef DRAW_CMS_LABEL_H
#define DRAW_CMS_LABEL_H

#include <TLatex.h>
#include <cmath>

// CMS Run 3 (Preliminary) in ONE LINE
inline void DrawCMSLabel(double x = 0.11,
                         double y = 0.96,
                         const char* extra = "Run3",
                         bool preliminary = false)
{
  TLatex latex;
  latex.SetNDC();
  latex.SetTextAlign(13); // left-top
  latex.SetTextColor(kBlack);

  // --- CMS (bold) ---
  latex.SetTextFont(62);
  latex.SetTextSize(0.050);
  latex.DrawLatex(x, y, "CMS");

  // estimate horizontal offset for the following text
  // (tuned for text size 0.05)
  const double dx = 0.075;

  // --- extra text (italic) ---
  latex.SetTextFont(52);
  latex.SetTextSize(0.040);

  if (preliminary)
    latex.DrawLatex(x + dx, y -0.008, Form("%s Preliminary", extra));
  else
    latex.DrawLatex(x + dx, y -0.008, extra);
}

#endif
