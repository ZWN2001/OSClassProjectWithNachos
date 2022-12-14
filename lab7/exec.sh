make clean
make
./nachos -x ../test/sort.noff  2>&1 | tee temp.log


#FIFO
#     ./n7 -pra -1 -x ../test/sort.noff
#CLOCK
#    ./n7 -pra -2 -x ../test/sort.noff