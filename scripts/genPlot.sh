#!/bin/bash

# Description: This script generates data files for plotting performance 
# metrics of different algorithms. It reads configuration from a file, 
# iteratively runs the 'tablegenerator' executable with varying scaling 
# factors, and extracts specific metrics to create individual data files
# for each algorithm. These data files can then be used for plotting 
# performance curves.
#
# Workflow:
# 1. Cleans up temporary files (__tmp, *.dat).
# 2. Reads the first 3 lines from the input file ($1) and stores them in 
#    the temporary file __tmp.
# 3. Appends "__tmp.csv" to __tmp.
# 4. Iterates from 1 to $2 (inclusive), representing different scaling 
#    factors.
# 5. For each iteration:
#    a. Calculates a scaling factor 't' as i/$2.
#    b. Runs the 'tablegenerator' executable with the configuration in 
#       __tmp and the scaling factor 't'.
#    c. Reads the output of 'tablegenerator' from '__tmp.csv'.
#    d. Extracts the algorithm name and a specific metric (column $3) 
#       from each line of '__tmp.csv'.
#    e. Appends the current iteration number 'i' and the extracted metric 
#       value to a data file named after the algorithm (e.g., 
#       algorithm1.dat).
# 6. Cleans up temporary files (__tmp, __tmp.csv).
#
# Input:
#   - $1: The path to a configuration file.
#   - $2: The number of iterations (scaling factors).
#   - $3: The index of the column in the tablegenerator output that 
#         contains the metric to be extracted.
#
# Output:
#   - Multiple .dat files, each named after an algorithm, containing pairs 
#     of (iteration number, metric value).
#
# Dependencies:
#   - ../build/tablegenerator: An executable that generates a table of 
#     performance metrics based on a configuration file and scaling factor.
#   - bc: A command-line calculator for performing arithmetic operations.
#
# Usage:
#   - bash genPlot.sh <config_file> <num_iterations> <metric_column_index>
#
# Example:
#   - bash genPlot.sh my_config.txt 10 4  (runs 10 iterations, extracts 
#     the 4th metric from column 4)

rm -f results/__tmp
rm -f results/*.dat

nline=0
while IFS= read -r line; do
    if [[ $nline = 3 ]]; then
	break;
    fi
    echo $line >> results/__tmp
    nline=$(($nline+1))
done < $1

echo "results/__tmp.csv" >> results/__tmp

for ((i=1; i<=$2; i++))
do
    t=$(echo "scale=1;$i/$2"|bc)
    build/tablegenerator -p results/__tmp -s $t

    filename='results/__tmp.csv'
    n=0
    while read line0; do
	# reading each line
	if [[ $n -eq 0 ]]; then
	    n=$((n+1))
	    continue;
	fi
	line=(${line0//,/ })
	name=${line[0]}
	echo $i ${line[$3]} >> results/${name}.dat 
	n=$((n+1))
    done < $filename
done
rm -f results/__tmp
rm -f results/__tmp.csv
