#!/bin/bash

# Description: This script analyzes the "metrics_interesting.csv" file
# to determine the most significant fields for sorting.
# It identifies fields that, when used for sorting, result in the
# largest difference in the "NN-SSS" metric.

# This helps in understanding which fields have the most impact on the
# data's structure when sorted.
#
# Workflow:
# 1. Reads the header of "metrics_interesting.csv" to get the field names.
# 2. For each field (excluding the first one):
#    a. Sorts the data in ascending order based on the current field
#       using 'sorting.sh'.
#    b. Processes the sorted data using 'tablegenerator' (which likely
#        calculates metrics).
#    c. Extracts the "NN-SSS" value from the 'Stat_tmp.csv' output.
#    d. Sorts the data in descending order based on the current field.
#    e. Processes the sorted data again using 'tablegenerator'.
#    f. Extracts the "NN-SSS" value again.
#    g. Calculates the absolute difference between the two "NN-SSS" values.
#    h. Stores the field name and the absolute difference in a temporary file.
# 3. Sorts the temporary file by the absolute difference in descending order.
# 4. Saves the sorted results to "results/Big_stat_<subset_size>.csv".
#
# Input:
#   - metrics_interesting.csv: The main data file (CSV format).
#   - subset_size: An integer representing the number of lines to
#     consider during each sort operation.
#
# Output:
#   - Big_stat_<subset_size>.csv: A CSV file containing the field
#     names and their corresponding absolute differences, sorted in
#     descending order of difference.
#
# Dependencies:
#   - sorting.sh: A script for sorting data based on a specified field
#     and direction.
#   - tablegenerator: An executable for processing data and generating
#     metrics (including "NN-SSS").
#
# Usage: bash pre_sort.sh <subset_size>
# Example: bash pre_sort.sh 100

# Define the input file name.
file="data/metrics_interesting.csv"

# Get the subset size from the command-line argument.
subset_size="$1"

# Check if the subset_size argument is provided.
if [ -z "$subset_size" ]; then
  echo "Error: subset_size argument is required."
  echo "Usage: bash pre_sort.sh <subset_size>"
  exit 1 # Exit with an error code.
fi

# Check if the metrics_interesting.csv file exists.
if [ ! -f "$file" ]; then
  echo "Error: metrics_interesting.csv not found."
  exit 1 # Exit with an error code.
fi

# Create P_tmp, the temporary configurtion file for tablegenertor
echo "data/RawResults/Res_DGS_all_NN.csv" > results/P_tmp
echo "some_instances results/tmp.txt" >> results/P_tmp
echo "some_algorithms data/Algorithms_all_NN" >> results/P_tmp
echo "results/Stat_tmp.csv" >> results/P_tmp

# Read the first line (header) of the CSV file.
read -r first_line < "$file"

# Split the header line into an array of field names.
fields=($first_line)

# Define the name of the temporary file for storing results.
tmp_file="results/Big_stat.tmp"

# Clear the temporary file if it exists, ensuring it's empty.
> "$tmp_file"

# Loop through each field in the header (excluding the first one).
for ((index = 1; index < ${#fields[@]}; index++)); do
  # Get the current field name.
  field="${fields[$index]}"

  # Print a message indicating which field is being processed.
  echo "Processing field: $field"

  # Sort in ascending order using sorting.sh, limiting to subset_size lines.
  # Redirect the output to tmp.txt.
  bash scripts/sorting.sh "$field" a "$subset_size" > results/tmp.txt

  # Process the sorted data using tablegenerator.
  # Redirect the output to /dev/null to suppress it.
  build/tablegenerator -p results/P_tmp -a > /dev/null

  # Extract the "NN-SSS" value from Stat_tmp.csv (output of tablegenerator).
  res1=$(grep "NN-SSS" results/Stat_tmp.csv | cut -f 2 -d',')

  # Sort in descending order using sorting.sh, limiting to subset_size lines.
  # Redirect the output to tmp.txt.
  bash scripts/sorting.sh "$field" d "$subset_size" > results/tmp.txt

  # Process the sorted data using tablegenerator.
  # Redirect the output to /dev/null to suppress it.
  build/tablegenerator -p results/P_tmp -a > /dev/null

  # Extract the "NN-SSS" value from Stat_tmp.csv.
  res2=$(grep "NN-SSS" results/Stat_tmp.csv | cut -f 2 -d',')

  # Calculate the difference between the two "NN-SSS" values.
  diff=$(echo $res1-$res2 | bc)

  # Calculate the absolute value of the difference.
  abs_diff=${diff#-}

  # Append the field name and the absolute difference to the temporary file.
  echo "$field,$abs_diff" >> "$tmp_file"
done

# Sort the results in the temporary file by the absolute difference
# (second column) in descending order.
# Use LC_NUMERIC to ensure correct numerical sorting.
LC_NUMERIC="en_US.UTF-8" sort -t',' -rn -k 2,2 "$tmp_file" > "results/Big_stat_$subset_size.csv"

# Clean up the temporary files.
rm -f results/Big_stat.tmp
rm -f results/tmp.txt
rm -f results/P_tmp
rm -f results/Stat_tmp.csv

# Print a message indicating where the results are saved.
echo "Results saved to results/Big_stat_$subset_size.csv"
