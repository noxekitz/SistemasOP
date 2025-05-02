set terminal png
set output 'scatter_plot.png'
set title "Resources Extracted vs Medium and Low Limits (High Limit = 100)"
set xlabel "Medium Limit"
set ylabel "Low Limit"
set zlabel "Resources Extracted"
set grid
set view 60, 30
splot 'datos_gnuplot.txt' using 1:2:3 with points pointtype 7 pointsize 1 title "Resources"
set output