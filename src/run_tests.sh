#!/bin/bash

# Author: Aman Hogan-Bailey
# University of Texas at Arlington

# Define an array of thread counts to test
THREAD_COUNTS=(1 2 4 8 16 56)

# Define the output files
WRITE_RESULTS="./output/write_results.csv"
READ_RESULTS="./output/read_results.csv"

# Remove previous results if they exist
rm -f $WRITE_RESULTS $READ_RESULTS

# Loop through each thread count
for THREAD_COUNT in "${THREAD_COUNTS[@]}"; do

    echo "Running test with $THREAD_COUNT threads..."
    OMP_NUM_THREADS=$THREAD_COUNT ./bandwidth_test -v 1
    
done

echo "All tests completed. Results are saved in $WRITE_RESULTS and $READ_RESULTS."
