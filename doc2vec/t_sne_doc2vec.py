#!/usr/bin/env python
from gensim.models.doc2vec import Doc2Vec
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import offsetbox
from sklearn import (manifold, datasets, decomposition, ensemble,
                     discriminant_analysis, random_projection)
from string import printable
printable = set(printable)

def plot_embedding(X, y, c, f, sz):
  x_min, x_max = np.min(X, 0), np.max(X, 0)
  X = (X - x_min) / (x_max - x_min)
  plt.figure(figsize=sz)
  plt.xticks([]), plt.yticks([])
  for i in range(X.shape[0]):
      plt.text(X[i, 0], X[i, 1],
   filter(lambda x: x in printable, y[i]).split('.')[0],
               fontdict={'weight': 'bold', 'size': 9})
  plt.savefig(f)

def CreateSet(vectors, labels, concepts):
  X = []
  label = []
  cat = []
  for i, v in enumerate(labels):
    for j, c in enumerate(concepts):
      if c in v:
        cat.append(j)
        X.append(vectors[i])
        label.append(v)
  return (X, label, cat)

def FullPlot(categories, f, sz):
  (X,y,c) = CreateSet(model.docvecs, labels, categories)
  X_tsne = tsne.fit_transform(np.vstack(X))
  plot_embedding(X_tsne, y, c, f, sz)

MODEL_FILE = '/media/zac/ssd-ext/saved-model-3/model'

LABEL_FILE = '/media/zac/ssd-ext/document-labels.txt'

model = Doc2Vec.load(MODEL_FILE, mmap='r')

with open(LABEL_FILE) as f:
  labels = [x.strip() for x in f.readlines()]


tsne = manifold.TSNE(n_components=2, init='pca', random_state=0)

FullPlot(['science'], '/tmp/a.png', (150,150))
