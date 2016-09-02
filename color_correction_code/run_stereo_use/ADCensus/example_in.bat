SET pathfile=H:\RQing_code\Run

SET execfile=%pathfile%\run_stereo\ADCensus\ADCensus_Build\bin\Release\stereoDense.exe

SET img=Art
SET img1=%pathfile%\data_cc\%img%\view1.png
SET img2=%pathfile%\data_cc\%img%\view5E2-n_RW.png

SET dmin=-200
SET dmax=0

SET dmap=%pathfile%\data_cc\%img%\disp_adcensus_view5E2-n_RW.png

SET cmd=%execfile% -O r %img1% %img2% %dmin% %dmax% %dmap% %img%

start %cmd%

pause