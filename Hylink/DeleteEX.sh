#!/bin/sh
echo "Running script to delete existing verfication files to prevent reuse."
cd ../wd
rm -f bloatedSimGI.cpp
rm -f hybridSimGI.cpp
rm -f libbloatedsim.so
rm -f libhybridsim.so
rm -f simu
