#!/usr/bin/env python

from gensim.models.doc2vec import Doc2Vec, TaggedDocument
from datetime import datetime
from random import shuffle
from collections import namedtuple

Document = namedtuple('Document', 'words tags')


DOCUMENT_FILE = '/home/jcad/Downloads/aclImdb/alldata.txt'

SIZE = 100
ITERATIONS = 20

docs = []

with open(DOCUMENT_FILE) as alldata:
  for line_no, line in enumerate(alldata):
    docs.append(TaggedDocument(line.split(), [line_no]))

training_start_time = datetime.now()
print "Training started - " + str(training_start_time)

model = Doc2Vec(dm=0, size=SIZE, negative=5, hs=0, min_count=2, workers=4)
model.build_vocab(docs)
for i in range(ITERATIONS):
  epoch_start_time = datetime.now()
  shuffle(docs)
  model.train(docs)
  print("Epoch {}: Completed in {}s".format(i, datetime.now() - epoch_start_time))

print "Training complete - Elapsed time: " + str(datetime.now() - training_start_time)

model.save('/home/jcad/Downloads/aclImdb/training-model')
