#!/bin/bash
RUN="sh bin/test_data_structures"

worktime=10

for dtype in {cg,fg,lf}{l,t}; do
    for nelements in 1000 5000 10000 20000; do
        for nthreads in 1 2 4 6 8 10; do
            echo -n "$dtype $nthreads $nelements $worktime "
            runtime="`MEM=1024 $RUN $dtype $nthreads $nelements $worktime \
                | grep -E -o \"[[:digit:]]+\"`"

            if [ $runtime ]; then
                echo $runtime;
            else
                echo "ERROR"
            fi
        done
    done
done
