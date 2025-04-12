#!/bin/bash

# Description: This script processes a set of zip files located in the 
# "Chimera/" directory. For each zip file, it extracts the contents using 
# zcat, pipes the output to the "deChimera" executable, saves the result 
# to a text file, compresses the text file into a new zip file in the 
# "de_chimera0/" directory, and then removes the temporary text file.
#
# Workflow:
# 1. Iterates through each zip file in the "Chimera/" directory.
# 2. Extracts the content of the current zip file using `zcat`.
# 3. Pipes the extracted content to the `../build/deChimera` executable.
# 4. Redirects the output of `deChimera` to a text file named after the 
#    original zip file (with .zip replaced by .txt).
# 5. Compresses the newly created text file into a zip file with the same
#    name in the "../de_chimera0/" directory.
# 6. Removes the temporary text file.
#
# Input:
#   - Zip files located in the "Chimera/" directory.
#
# Output:
#   - Zip files in the "../de_chimera0/" directory, each containing the 
#     output of the "deChimera" executable for the corresponding input zip
#     file.
#
# Dependencies:
#   - zcat: A command-line utility for decompressing zip files.
#   - ../build/deChimera: An executable that processes the data from the 
#     input zip files.
#   - zip: A command-line utility for compressing files into zip archives.
#
# Usage:
#   - Place zip files to be processed in the "Chimera/" directory.
#   - Run the script: `bash de_chim.sh`
#
# Example:
#   - If there is a file "Chimera/data1.zip", the script will:
#     - Extract the content of "data1.zip".
#     - Process it with "../build/deChimera".
#     - Save the output to "data1.txt".
#     - Compress "data1.txt" into "../de_chimera0/data1.zip".
#     - Delete "data1.txt".

for file in Chimera/*.zip
do
    zcat $file | ../build/deChimera > de_chimera0/$(basename $file .zip).txt
    zip de_chimera0/$(basename $file .zip).zip de_chimera0/$(basename $file .zip).txt
    rm -f de_chimera0/$(basename $file .zip).txt
done
