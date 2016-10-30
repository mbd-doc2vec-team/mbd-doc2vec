import statsmodels.api as sm
from gensim.models.doc2vec import Doc2Vec

FILE_LIST = '/home/jcad/Downloads/aclImdb/allfiles.txt'
MODEL = '/home/jcad/Downloads/aclImdb/training-model'

with open(FILE_LIST) as f:
  labels = [x.strip() for x in f.readlines()]

# Document sets
train_indices = [i for (i,v) in enumerate(labels)
                 if 'train' in v and 'unsup' not in v]
test_indices = [i for (i,v) in enumerate(labels)
                if 'test' in v and 'unsup' not in v]

# Document labels
test_targets = [(1 if 'pos' in labels[i] else 0) for i in train_indices]
train_targets = [(1 if 'pos' in labels[i] else 0) for i in train_indices]

model = Doc2Vec.load(MODEL)

# Document vectors
train_vecs = [model.docvecs[i] for i in train_indices]
train_vecs = sm.add_constant(train_vecs)

test_vecs = [model.docvecs[i] for i in test_indices]
test_vecs = sm.add_constant(test_vecs)

# Compute regression
logit = sm.Logit(train_targets, train_vecs)
pred = logit.fit(disp=0)
test_preds = pred.predict(test_vecs)

# Evaluate results
correct = 0

for i in range(len(test_preds)):
  if test_preds[i] <= 0.5 and test_targets[i] == 0:
    correct += 1
  elif test_preds[i] > 0.5 and test_targets[i] == 1:
    correct += 1

error = correct / float(len(test_preds))
print "Accuracy: " + str(error)
