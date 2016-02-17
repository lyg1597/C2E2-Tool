#!/bin/bash

MAKE=0
TIMEFORMAT="%3R"

systems=(bruss sys7d power_train)

#Make files
if [ $MAKE -eq 1 ]; then
  make -C capd
  make -C odeint
fi

#Compare execution times
for mdl in ${systems[@]}; do
  c_path=capd/$mdl
  o_path=odeint/$mdl

  #Getting CAPD time
  c_time=$({ time ./$c_path/$mdl < $c_path/Config > $c_path/trace.txt; } 2>&1 )

  #Getting ODEINT time
  o_time=$({ time ./$o_path/$mdl < $o_path/Config > $o_path/trace.txt; } 2>&1 )

  #echo $o_time
  speedup=$(bc -l <<< "scale=2;$c_time/$o_time")

  #Printing times
  printf "%-15s | %-5s (CAPD) | %-5s (ODEINT) | %-7s (Speedup)\n" $mdl $c_time $o_time $speedup
done
