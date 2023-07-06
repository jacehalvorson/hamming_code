#!/bin/bash

test_folder="test_files"

if [ -z "$(ls -A "$test_folder")" ]; then
   echo "Directory $test_folder is empty."
   exit 1
fi

make

# Encode and decode all files in test folder and check if the decoded files are the same as the original ones
for file in "$test_folder"/*;
do
   ./hamming e $file
   ./hamming d $file.ham
   diff_output=$(diff $file $file.ham.dec)
   if [ -n "$diff_output" ];
   then
      echo "Test passed for file $file"
   else
      echo "Test failed for file $file"
      echo "$diff_output"
   fi
done

rm "$test_folder"/*.ham "$test_folder"/*.dec
make clean