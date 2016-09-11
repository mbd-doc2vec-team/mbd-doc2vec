#!/usr/bin/env python

from gensim.models.doc2vec import Doc2Vec, TaggedLineDocument
from datetime import datetime
import numpy as np

TRAINING_DOCUMENT_FILE = '/home/jcad/Downloads/aclImdb/training-data.txt'
TESTING_DOCUMENT_FILE = '/home/jcad/Downloads/aclImdb/testing-data.txt'

TRAINING_MODEL_DEST = '/home/jcad/Downloads/aclImdb/training-model'
TESTING_INFERED_DEST = '/home/jcad/Downloads/aclImdb/testing-infered_vecs'

SIZE = 200
WINDOW = 5 
ITERATIONS = 20
CONCATENATE = 1

# Training
training_docs = TaggedLineDocument(TRAINING_DOCUMENT_FILE)

print "Training"
print str(datetime.now())
training_model = Doc2Vec(documents=training_docs, size=SIZE, window=WINDOW,
                 min_count=8, iter=ITERATIONS,
                 dm_concat=CONCATENATE, workers=4)
print str(datetime.now())

training_model.save(TRAINING_MODEL_DEST)

# Testing
testing_docs = TaggedLineDocument(TESTING_DOCUMENT_FILE)

print "Infering testing set"
print str(datetime.now())
infered_test_vecs = np.ndarray(shape=(25000, SIZE))
for idx, doc in enumerate(testing_docs):
    infered_test_vecs[idx] = training_model.infer_vector(doc.words, steps=ITERATIONS)

print str(datetime.now())

np.save(TESTING_INFERED_DEST, infered_test_vecs)


