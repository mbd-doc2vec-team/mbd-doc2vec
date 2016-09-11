#!/bin/bash
# Script to concatenate IMDB documents into a single file, delimited by newlines
# arg1: file with filenames to concat
# arg2: file to output to

for line in `cat $1`; do
	cat $line >> $2;
	printf "\n" >> $2;
done;
