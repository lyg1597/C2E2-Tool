#!/bin/sh
echo "Running script to delete existing verfication files to prevent reuse."
cd ../wd
#rm guardGen.cpp
#rm simulator.cpp
#rm Invcheck.cpp
#rm guards
#rm invariants
rm bloatedSimGI.cpp
rm hybridSimGI.cpp
rm libbloatedsim.so
rm libhybridsim.so
rm simu
