
plot/
  include/
    PlotTypes.h              # Common struct (1chain job, compare job)
    PlotUtils.h              # Common helper (list->TChain, cut, normalize, blind cut, etc..)
    DrawCMSLabel.h           # TEXT : CMS Preliminary outer canvas.
    DrawLumiTopRight.h       # TEXT : Luminosity outer canvas.
    PlotTextBlock.h          # TEXT : Selections in canvas.
  src/
    Plot1ChainPDF.C          # 1개의 체인 input -> 1D/2D PDF 생성
    PlotCompareRatioPDF.C    # 2개의 체인 input -> 1D 비교+ratio (event 수로 normalize)
    PlotCompareRatioBlindPDF.C # 위와 동일 + blind 포함
  palette_all.C              # 유일한 palette: bin/range/expr/cut/listfile 여기서만 수정

# Execution
cd plot
root -l -q 'palette_all.C("plots_out")'

