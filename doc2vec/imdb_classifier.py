#!/usr/bin/env python

from gensim.models.doc2vec import Doc2Vec, TaggedLineDocument
from keras.models import Sequential
from keras.layers import Dense, Activation
from keras.optimizers import RMSprop
from datetime import datetime
import numpy as np

TRAINING_MODEL_FILE = '/home/jcad/Downloads/aclImdb/training-model'
TESTING_DATA_FILE = '/home/jcad/Downloads/aclImdb/testing-data.txt'

TESTING_INFERED_FILE = '/home/jcad/Downloads/aclImdb/testing-infered_vecs.npy'

SIZE=200

training_model = Doc2Vec.load(TRAINING_MODEL_FILE)
infered_test_vecs = np.load(TESTING_INFERED_FILE)

# Generate labels from files
labels = np.append(np.ones(12500), np.zeros(12500))

# Train NN
nn = Sequential([
 Dense(50, input_dim=SIZE, activation="sigmoid"),
 Dense(1, activation="sigmoid")
])

rmsprop = RMSprop(lr=0.001)
nn.compile(
    optimizer=rmsprop,
    loss='binary_crossentropy',
    metrics=['accuracy']
)

# Fit and test NN
print "Fitting and testing"
print str(datetime.now())

final_training_docvecs = np.ndarray(shape=(25000, SIZE))
for index, vec in enumerate(training_model.docvecs):
    if (index < 50000):
        continue
    final_training_docvecs[index-50000] = vec

nn.fit(final_training_docvecs, labels, validation_data=(infered_test_vecs, labels), nb_epoch=100, batch_size=32, verbose=2)

print str(datetime.now())
