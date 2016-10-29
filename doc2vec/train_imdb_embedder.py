#!/usr/bin/env python

from datetime import datetime, timedelta
from gensim.models.doc2vec import Doc2Vec, TaggedDocument
from os import path
from random import shuffle
import numpy as np


ALL_DATA_FILE = '/home/jcad/Downloads/aclImdb/alldata.txt'

TRAINING_MODEL_DEST = '/home/jcad/Downloads/aclImdb/training-model'
TESTING_INFERED_DEST = '/home/jcad/Downloads/aclImdb/testing-infered_vecs'

SIZE = 100
ITERATIONS = 20

assert path.isfile(ALL_DATA_FILE), "alldata.txt must exist"

testing_docs = []
training_docs = []
all_docs = []

with open(ALL_DATA_FILE) as alldata:
    for line_no, line in enumerate(alldata):
    	document = TaggedDocument(line.split(), str(line_no))
    	all_docs.append(document)
    	if line_no < 25000:
    		testing_docs.append(document)
		if line_no >= 75000:
			training_docs.append(document)

# Training
print "Training"
training_start_time = datetime.now()
training_model = Doc2Vec(dm=0, size=SIZE, negative=5, hs=0, min_count=2, workers=4)
training_model.build_vocab(all_docs)

for epoch in range(ITERATIONS):
	epoch_start_time = datetime.now()
	shuffle(training_docs)
	training_model.train(training_docs)
	print("Epoch {}: Completed in {}s".format(epoch, datetime.now() - epoch_start_time))

print "Training completed in {}s".format(datetime.now() - training_start_time)

training_model.save(TRAINING_MODEL_DEST)

# Testing
print "Infering testing set"
print str(datetime.now())
infered_test_vecs = np.ndarray(shape=(25000, SIZE))
for idx, doc in enumerate(testing_docs):
    # infered_test_vecs[idx] = training_model.infer_vector(doc.words, steps=ITERATIONS)
    infered_test_vecs[idx] = doc.tags[0]

print str(datetime.now())

np.save(TESTING_INFERED_DEST, infered_test_vecs)
