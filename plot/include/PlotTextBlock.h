#ifndef PLOT_TEXT_BLOCK_H
#define PLOT_TEXT_BLOCK_H

#include <string>
#include <vector>
#include <TLatex.h>

// --------------------------------------------------
// 1) key → predefined text lines
// --------------------------------------------------
inline std::vector<std::string>
GetPlotTextPreset(const std::string& key)
{
  if (key == "txt_baseline") {
    return {
      "#bf{S1.} N_{e/#mu} #geq 2",
      "#bf{S2.} HLT triggers with p_{T} cuts",
      "#bf{S3.} N_{#gamma} #geq 1",
      "#bf{S4.} 80 #leq m_{ll} #leq 100 GeV",
      "#bf{S5.} p_{T}(#gamma)/m_{ll#gamma} #geq 15./110",
      "#bf{S6.} m_{ll} + m_{ll#gamma} #geq 185 GeV",
      "#bf{S7.} 95 < m_{ll#gamma} < 180 GeV",
      "#bf{CR.} m_{ll#gamma} < 120 || m_{ll#gamma} > 130 GeV"
    };
  }

  if (key.empty() || key == "txt_none")
    return {};

  // fallback: key 자체 출력 (디버그용)
  return { key };
}

// --------------------------------------------------
// 2) Draw text block (Top-Right)
// --------------------------------------------------
inline void
DrawTextBlockTR(const std::vector<std::string>& lines,
                double x = 0.88,
                double y = 0.88,
                double lineStep = 0.040,
                int font = 42,
                double size = 0.032)
{
  if (lines.empty()) return;

  TLatex latex;
  latex.SetNDC();
  latex.SetTextAlign(33);   // right-top
  latex.SetTextFont(font);
  latex.SetTextSize(size);

  for (size_t i = 0; i < lines.size(); ++i) {
    latex.DrawLatex(x, y - i * lineStep,
                    lines[i].c_str());
  }
}

#endif
