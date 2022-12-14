make clean
make
./nachos -x ../test/exec.noff 2>&1 | tee temp.log
