set autoscale
unset log
unset label
set xtic auto


set terminal pdf enhanced color font "Times"


#set key top left
set key top right
set key font ",22"

set ylabel "Probability"

set xlabel font ",22"
set ylabel font ",22"

#set xtics (1, 2, 5, 10)
set xtic auto

set xtic font ",18"
set ytic font ",18"

fitSigmoidDBM(x) = 1.0 / (1.0 + exp((ss1-x)*ss2))
fit fitSigmoidDBM(x) 'data.dat' using 1:2 via ss1, ss2

fitSigmoidWatt(x) = 1.0 / (1.0 + exp((sl1-( 10**((x-30.0)/10.0) ))*sl2))
fit fitSigmoidWatt(x) 'data.dat' using 1:2 via sl1, sl2

fitSigmoidWattBIS(x) = 1.0 / (1.0 + exp((slb1-( x ))*slb2))
fit fitSigmoidWattBIS(x) 'data.dat' using (10**(($1-30.0)/10.0)):2 via slb1, slb2


#fitBig(x) = 100/((x+1)**krBig)
#fit fitBig(x) 'dataAllDim.dat' using ($1/3.6):2 via krBig

#fitMed(x) = 100/((x+1)**krMed)
#fit fitMed(x) 'dataAllDim.dat' using ($1/3.6):3 via krMed

#fitSmall(x) = 100/((x+1)**krSmall)
#fit fitSmall(x) 'dataAllDim.dat' using ($1/3.6):4 via krSmall

#set bmargin 3.4

#set xrange [-5 : 25]
unset xrange
set xlabel "SINR (dBm)"
set output "fitall_dbm.pdf"

plot \
fitSigmoidDBM(x) with lines t "dBm" lw 5 lc 2, \
"data.dat" using 1:2 t "" with points ps 1.1 pt 5 lc 2


#set xrange [-5 : 25]
unset xrange
set xlabel "SINR (W)"
set output "fitall_watt.pdf"

plot \
fitSigmoidWatt(x) with lines t "Watt" lw 5 lc 2, \
"data.dat" using (10**(($1-30.0)/10.0)):2 t "" with points ps 1.1 pt 5 lc 2

#set xrange [-5 : 25]
unset xrange
set xlabel "SINR (W)"
set output "fitall_watt_bis.pdf"

plot \
fitSigmoidWattBIS(x) with lines t "Watt" lw 5 lc 2, \
"data.dat" using (10**(($1-30.0)/10.0)):2 t "" with points ps 1.1 pt 5 lc 2








