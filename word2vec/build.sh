#!/bin/bash
TF_INC=$(python -c 'import tensorflow as tf; print(tf.sysconfig.get_include())')
DIR=$(dirname $0)
g++ -std=c++11 -shared $DIR/word2vec*.cc -o $DIR/word2vec.so \
    -fPIC -D_GLIBCXX_USE_CXX11_ABI=0 -I $TF_INC -I ./tensorflow
