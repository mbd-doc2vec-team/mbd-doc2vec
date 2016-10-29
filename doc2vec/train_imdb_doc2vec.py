#!/usr/bin/env python

from gensim.models.doc2vec import Doc2Vec, TaggedLineDocument
from datetime import datetime
from random import shuffle
from collections import namedtuple

Document = namedtuple('Document', 'words tags')


DOCUMENT_FILE = '/media/zac/ssd-ext/imdb-dataset/documents.txt'

SIZE = 200
WINDOW = 8
ITERATIONS = 20
CONCATENATE = 0

docs = []

with open(DOCUMENT_FILE) as alldata:
  for line_no, line in enumerate(alldata):
    words = line.split()
    tags = [line_no]
    docs.append(Document(words, tags))

print str(datetime.now())
model = Doc2Vec(dm=0, size=SIZE, negative=5, hs=0, min_count=2, workers=8)
model.build_vocab(docs)
for i in range(ITERATIONS):
  shuffle(docs)
  model.train(docs)
print str(datetime.now())

model.save('/media/zac/ssd-ext/imdb-model-3/model')
