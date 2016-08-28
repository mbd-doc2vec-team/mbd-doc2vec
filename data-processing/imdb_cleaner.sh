#!/bin/bash
# Point at IMDB dump directory

mkdir -p $1/processed/test/
mkdir -p $1/processed/train/

declare -a arr=("/test/pos/" "/test/neg/" "/train/pos/" "/train/neg/" "/train/unsup/")

for i in "${arr[@]}"
do
    cp -r "$1/$i" "$1/processed/$i"
    sed -i 's/<br \/>//g' "$1/processed/$i/"*
    sed -i 's/[^a-zA-Z0-9 \-]//g' "$1/processed/$i/"*
done
