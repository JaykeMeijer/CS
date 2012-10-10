#!/bin/bash
RUN="sh bin/test_data_structures"

#THREADS="1 `seq 2 2 10`"
nthreads=4
worktime=10

for dtype in {cg,fg,lf}l; do
    for n in 100 1000 5000 `seq 10000 10000 50000`; do
        echo -n "$dtype $nthreads $n $worktime "
        MEM=1024 $RUN $dtype $nthreads $n $worktime | egrep -o "[0-9]+"
    done
done

for dtype in {cg,fg,lf}t; do
    for n in 100 1000 5000 `seq 10000 10000 50000` `seq 100000 100000 1000000`; do
        echo -n "$dtype $nthreads $n $worktime "
        MEM=1024 $RUN $dtype $nthreads $n $worktime | egrep -o "[0-9]+"
    done
done
