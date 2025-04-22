#!/bin/bash

# Description: This script sorts a CSV file
# ("metrics_interesting.csv") based on a specified field and direction
# (ascending/descending).  It also supports limiting the number of
# output lines and displaying available field names.
#
# Input:
#   - $1: The field name to sort by (or "?" to list available fields).
#   - $2: The sorting direction ("a" for ascending, "d" for descending).
#   - $3: The maximum number of lines to output (optional, defaults to all).
#
# Output:
#   - If $1 is "?": Lists the available field names from the header of
#     "metrics_interesting.csv".
#   - Otherwise: Outputs the first column of the sorted data (limited
#     by $3) to standard output.
#   - Error messages to standard error if invalid parameters are provided.
#
# Dependencies:
#   - metrics_interesting.csv: The CSV file to sort.
#
# Usage:
#   - bash sorting.sh <field_name> <direction> <max_lines>
#   - bash sorting.sh ? (to list available fields)
#
# Example:
#   - bash sorting.sh field3 a 100  (sort by field3 ascending, output 100 lines)
#   - bash sorting.sh field5 d (sort by field5 descending, output all lines)
#   - bash sorting.sh ? (list available fields)

# Define the input file name.
file="data/metrics_interesting.csv"

# Set a flag for random permutation column.
# This is used to break ties when sorting.
random_perm_offset=1

# Initialize an error flag.
error=0

# Read the header line of the CSV file.
read -r first_line < "$file"

# Count the number of fields in the header.
# Using tr to replace spaces with newlines and then counting lines is
# more robust than grep -o.

fields_count=$(echo "$first_line" | tr ' ' '\n' | wc -l)

# Calculate the index of the random permutation column.
# It's the last column -2 + the offset.
random_perm=$((fields_count - 3 + random_perm_offset))

# Find the index of the field to sort by.
index=1
IFS=' ' read -r -a fields <<< "$first_line" # Read the header into an array
for ((i=0; i<${#fields[@]}; i++)); do
    if [[ "${fields[$i]}" == "$1" ]]; then
        index=$((i+1))
        break
    fi
done

# Handle the case of empty arguments
if [[ "$1" == "" ]]; then
    echo
    echo "Usage:"
    echo 
    echo "  $0 <field_name> <direction> <number of output lines>"
    echo
    exit
fi


# Validate the field name.
if [[ "$1" != "?" ]]; then
    if (( index == 1 )) || (( index > fields_count )); then
        echo "*** Field name \"$1\" does not exist! Use field name ?"
	" to see them all." >&2
        error=1
    fi
else
    error=1
fi

# Validate the sorting direction.
if [[ "$2" == "a" ]]; then
    dir="" # Ascending
elif [[ "$2" == "d" ]]; then
    dir="r" # Descending
elif (( error == 0 )); then
    error=2
    echo "*** Wrong parameter #2 ($2). Allowed \"a\" or \"d\"." >&2
fi

# Validate the maximum number of lines.
if [[ -z "$3" ]] || (( "$3" < 0 )) || (( "$3" > 2635 )); then
    if (( error == 0 )); then
        error=3
        echo "*** Wrong parameter #3 ($3). Allowed interval (0,2635]." >&2
    fi
else
    lines="$3"
fi

# Perform the sorting if no errors occurred.
if (( error == 0 )); then
    # Sort the data:
    # - tail +2: Skip the header line.
    # - sort: Sort numerically (-n) based on the specified field and direction.
    # -k $index,$index: Sort by the specified field.
    # -k $random_perm,$random_perm: Sort by the random permutation
    #    column to break ties.
    # - cut -d" " -f 1: Extract the first column (assuming space as delimiter).
    # - head -$lines: Limit the output to the specified number of lines.
    tail +2 "$file" | sort -"$dir"n -k "$index","$index" -k "$random_perm","$random_perm" | cut -d" " -f 1 | head -"$lines"
fi

# Handle the case where the field name is "?" or an error occurred.
if [[ "$1" == "?" ]] || (( error == 1 )); then
    # Print the available field names.
    index=0
    for field in $first_line; do
        ((index++))
        if (( index > 1 )) && (( index <= fields_count )); then
            echo -n "$field "
        fi
    done
    echo " "
    exit # Exit after listing the fields.
fi

# Exit with an error code if there was an error.
if (( error > 0 )); then
    exit 1
fi
