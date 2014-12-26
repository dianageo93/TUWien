echo "Testing Cilk ............................................................"
cd cilk/
make > /dev/null
cd ..
echo "Compiled ................................................................"
echo "Num elements,Range,Num Cores,Seq Sort Time,Partition Time,Prefixsum Time,Sort Time,Par Sort Time" > results_cilk.csv

echo "Testing ................................................................."
NUM_ELEMS=( 5 50 500 5000 50000 500000 5000000 50000000 500000000 )
RANGE=( 100 200 500 1000 5000 )
NUM_CORES=( 1 2 4 10 20 40 48 50 )

for E in "${NUM_ELEMS[@]}"
do
    for R in "${RANGE[@]}"
    do
        for C in "${NUM_CORES[@]}"
        do
            echo -n $E $R $C
            echo -n ": "
            for I in {1..5}
            do
                echo -n $I 
                echo -n " "
                ./cilk/bin/exec $E $R $C >> results_cilk.csv
            done
            echo
            echo -n $((2 * $E)) $R $C
            echo -n ": "
            for I in {1..5}
            do
                echo -n $I
                echo -n " "
                ./cilk/bin/exec $((2 * $E)) $R $C >> results_cilk.csv
            done
            echo
        done
    done
done

echo "Done ...................................................................."


echo "Testing Pthreads ........................................................"
cd pthreads/
make > /dev/null
cd ..
echo "Compiled ................................................................"
echo "Num elements,Range,Num Cores,Seq Sort Time,Partition Time,Prefixsum Time,Sort Time,Par Sort Time" > results_pthreads.csv

echo "Testing ................................................................."
NUM_ELEMS=( 5 50 500 5000 50000 500000 5000000 50000000 500000000 )
RANGE=( 100 200 500 1000 5000 )
NUM_CORES=( 1 2 4 10 20 40 48 50 )

for E in "${NUM_ELEMS[@]}"
do
    for R in "${RANGE[@]}"
    do
        for C in "${NUM_CORES[@]}"
        do
            echo -n $E $R $C
            echo -n ": "
            for I in {1..5}
            do
                echo -n $I 
                echo -n " "
                ./pthreads/bin/exec $E $R $C >> results_pthreads.csv
            done
            echo
            echo -n $((2 * $E)) $R $C
            echo -n ": "
            for I in {1..5}
            do
                echo -n $I
                echo -n " "
                ./cilk/bin/exec $((2 * $E)) $R $C >> results_pthreads.csv
            done
            echo
        done
    done
done

echo "Done ...................................................................."
