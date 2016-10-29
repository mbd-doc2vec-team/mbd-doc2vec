#!/bin/bash
# Point at IMDB dump directory

rm -r $1/processed/
mkdir -p $1/processed/test
mkdir -p $1/processed/train


declare -a type=("test" "train")
declare -a label=("pos" "neg")

for i in "${type[@]}"; do
    for j in "${label[@]}"; do
        cp -r $1/$i/$j $1/processed/$i
        perl -C -pe 's/<br \/>/ /g' -i $1/processed/$i/$j/*
        perl -C -pe '$_=lc' -i $1/processed/$i/$j/*
        perl -C -pe "s/([\.,\"()\!?;:])/ \$1 /g" -i $1/processed/$i/$j/*
        perl -C -pe "s/'//g" -i $1/processed/$i/$j/*
    done
done

cp -r $1/train/unsup $1/processed/train
find $1/processed/train/unsup -name "*.txt" -exec perl -C -pe 's/<br \/>/ /g' -i '{}' +
find $1/processed/train/unsup -name "*.txt" -exec perl -C -pe '$_=lc' -i '{}' +
find $1/processed/train/unsup -name "*.txt" -exec perl -C -pe "s/([\.,\"()\!?;:])/ \$1 /g" -i '{}' +
find $1/processed/train/unsup -name "*.txt" -exec perl -C -pe "s/'//g" -i '{}' +