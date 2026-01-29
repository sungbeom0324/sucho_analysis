
plot/
  include/
    PlotTypes.h              # 공통 struct (1chain job, compare job)
    PlotUtils.h              # 공통 helper (list->TChain, cut 결합, normalize, blind cut 등)
  src/
    Plot1ChainPDF.C          # 1개의 체인 input -> 1D/2D PDF 생성
    PlotCompareRatioPDF.C    # 2개의 체인 input -> 1D 비교+ratio (event 수로 normalize)
    PlotCompareRatioBlindPDF.C # 위와 동일 + blind 포함
  palette_all.C              # ⭐ 유일한 palette: bin/range/expr/cut/listfile 여기서만 수정

# Execution
cd plot
root -l -q 'palette_all.C("plots_out")'

