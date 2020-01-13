set zeroaxis
r = 2
set xrange[-r:r]
set yrange[-r:r]
set grid

plot "scatter.tmp" using 1:2:(sprintf("(%0.4f, %0.4f)", $1, $2)) with labels point  pt 7 offset char 1,1 notitle
