#!/bin/bash
set -e
make clean
make
# This is the decision tree as a CSV file 
dt_csv_file=../../DT/spam/spam_dt.csv
test -f $dt_csv_file
# This is an optional test file 
# testfile=../data/spam_data/test_1_10.csv
# test -f $testfile
# These are 3 binary files created by the process
dtfile=dt.bin
rffile=rf.bin
mdlfile=mdl.bin
VG=""
VG="valgrind --leak-check=full"
$VG ./test_dt $dt_csv_file $dtfile $rffile $mdlfile $testfile
echo "Successfully completed $0 in $PWD"
