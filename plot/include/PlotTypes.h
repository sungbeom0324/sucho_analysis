#ifndef PLOT_TYPES_H
#define PLOT_TYPES_H

#include <string>
#include <vector>

// ---------- 1D histogram definition ----------
struct Hist1D {
  std::string name;   // unique within a job
  std::string expr;   // branch/expression
  int nbins = 0;
  double xmin = 0.0;
  double xmax = 0.0;
  std::string xTitle; // optional (""이면 expr 사용)
};

// ---------- 2D histogram definition ----------
struct Hist2D {
  std::string name;
  std::string exprYX;  // "Y:X"
  int nbx = 0; double xmin = 0.0; double xmax = 0.0;
  int nby = 0; double ymin = 0.0; double ymax = 0.0;
  std::string xTitle;  // optional
  std::string yTitle;  // optional
  std::string cut;     // optional cut (추가로 적용)
};

// ---------- 1 chain -> plots ----------
struct OneChainJob {
  std::string listfile;
  std::string treename = "Events";
  std::string tag;
  std::string baseCut;           // job 공통 cut
  std::vector<Hist1D> h1;
  std::vector<Hist2D> h2;
};

// ---------- 2 chains -> compare 1D ----------
struct CompareJob {
  std::string listA;
  std::string listB;
  std::string treename = "Events";

  std::string tag;
  std::string labelA = "A";
  std::string labelB = "B";

  std::string baseCut; // 공통 selection
  std::string textKey = "";  // e.g. "txt_baseline" 

  // blind 옵션: expr == blindVar 인 plot에만 blind 적용
  bool useBlind = false;
  std::string blindVar;
  double blindMin = 0.0;
  double blindMax = 0.0;

  // -------------------------
  // NEW: normalization control
  // normMode:
  //   0 = shape (기존) : integral=1
  //   1 = per-lumi     : scale by 1/lumi (Events per 1 fb^-1)
  // -------------------------
  int normMode = 0;
  double lumiA_fb = 1.0;
  double lumiB_fb = 1.0;

  std::vector<Hist1D> h1;
};

#endif

