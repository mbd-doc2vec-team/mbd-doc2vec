#!/usr/bin/env python

from gensim.models.doc2vec import Doc2Vec, TaggedLineDocument
from datetime import datetime

DOCUMENT_FILE = '/media/zac/ssd-ext/chromium-tokens.txt'

SIZE = 200
WINDOW = 8
ITERATIONS = 20
CONCATENATE = 0

docs = TaggedLineDocument(DOCUMENT_FILE)

print str(datetime.now())
model = Doc2Vec(documents=docs, size=SIZE, window=WINDOW,
                min_count=8, iter=ITERATIONS,
                dm_concat=CONCATENATE, workers=8)
print str(datetime.now())

model.save('/media/zac/ssd-ext/chromium-model-1/model')
