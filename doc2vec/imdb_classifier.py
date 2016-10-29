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

SIZE=100

training_model = Doc2Vec.load(TRAINING_MODEL_FILE)
infered_test_vecs = np.load(TESTING_INFERED_FILE)


#################################################
def error_rate_for_model(test_model, train_set, test_set):
    """Report error rate on test_doc sentiments, using supplied model and train_docs"""

    train_targets, train_regressors = zip(*[(doc.sentiment, test_model.docvecs[doc.tags[0]]) for doc in train_set])
    train_regressors = sm.add_constant(train_regressors)
    predictor = logistic_predictor_from_data(train_targets, train_regressors)

    test_data = test_set
    test_regressors = [test_model.docvecs[doc.tags[0]] for doc in test_docs]
    test_regressors = sm.add_constant(test_regressors)
    
    # predict & evaluate
    test_predictions = predictor.predict(test_regressors)
    corrects = sum(np.rint(test_predictions) == [doc.sentiment for doc in test_data])
    errors = len(test_predictions) - corrects
    error_rate = float(errors) / len(test_predictions)
    return (error_rate, errors, len(test_predictions), predictor)

from collections import defaultdict
best_error = 100
import statsmodels.api as sm

for epoch in range(20):
    err, err_count, test_count, predictor = error_rate_for_model(train_model, train_docs, test_docs)
    if err <= best_error:
        best_error = err
    print("%f : %i passes" % (err, epoch + 1))

#################################################

import sys
sys.exit(0)

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
