#ifndef PLOT_UTILS_H
#define PLOT_UTILS_H

#include <TChain.h>
#include <TH1D.h>

#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>

inline int addFilesFromList(TChain& ch, const std::string& listfile) {
  std::ifstream fin(listfile);
  if (!fin) {
    std::cerr << "[ERROR] cannot open listfile: " << listfile << "\n";
    return 0;
  }
  std::string line;
  int nAdded = 0;

  while (std::getline(fin, line)) {
    line.erase(std::remove_if(line.begin(), line.end(),
                              [](unsigned char c){ return c=='\r' || c=='\n'; }),
               line.end());
    while (!line.empty() && (line[0]==' ' || line[0]=='\t')) line.erase(line.begin());
    while (!line.empty() && (line.back()==' ' || line.back()=='\t')) line.pop_back();

    if (line.empty() || line[0]=='#') continue;

    int added = ch.Add(line.c_str());
    if (added <= 0) {
      std::cerr << "[WARN] failed to add: " << line << "\n";
      continue;
    }
    nAdded += added;
  }
  return nAdded;
}

inline std::string wrapAnd(const std::string& a, const std::string& b) {
  if (a.empty()) return b;
  if (b.empty()) return a;
  return "(" + a + ")&&(" + b + ")";
}

// !(var > min && var < max)
inline std::string blindCut(const std::string& var, double mn, double mx) {
  return "!(" + var + ">" + std::to_string(mn) + "&&" + var + "<" + std::to_string(mx) + ")";
}

inline double integralNoUFOF(const TH1D& h) {
  return h.Integral(1, h.GetNbinsX());
}

// “Event 수로 normalize” = selection 적용 후 histogram 적분(under/overflow 제외)으로 나눔
inline void normalizeByIntegral(TH1D& h) {
  double integ = integralNoUFOF(h);
  if (integ > 0) h.Scale(1.0 / integ);
}

// -------------------------
// NEW: norm policy
// normMode:
//   0 = shape (integral=1)
//   1 = per-lumi (Events per 1 fb^-1): scale by 1/lumi
// -------------------------
inline void applyNorm(TH1D& h, int normMode, double lumi_fb = 1.0) {
  if (normMode == 0) {
    normalizeByIntegral(h);
  } else if (normMode == 1) {
    if (lumi_fb > 0) h.Scale(1.0 / lumi_fb);
  }
}

inline const char* yTitleFromNorm(int normMode) {
  if (normMode == 1) return "Events / (1 fb^{-1})";
  return "Normalized entries";
}

inline TH1D* makeRatio(const TH1D& num, const TH1D& den, const char* name) {
  TH1D* r = (TH1D*)num.Clone(name);
  r->SetDirectory(nullptr);
  r->Reset("ICE");
  r->Divide(&num, &den, 1.0, 1.0, "");
  return r;
}

// exprYX="Y:X" 에서 X/Y 라벨 자동 추출 (title 비어있을 때만)
inline void splitYX(const std::string& exprYX, std::string& y, std::string& x) {
  auto pos = exprYX.find(':');
  if (pos == std::string::npos) { y = exprYX; x = ""; return; }
  y = exprYX.substr(0, pos);
  x = exprYX.substr(pos+1);
}

#endif

