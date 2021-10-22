#!/usr/bin/env python3
from collections import defaultdict
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
import scipy.stats as ss
import math
W = 31
L = 2**W

def estimateProbs(xs):
  n = len(xs)
  p = defaultdict(lambda : 0)
  for x in xs:
    p[x] += 1
  for i in p.keys():
    p[i] /= n
  return p

def JacobsonBitmap(n):
  logn = math.log2(n)
  loglogn = math.log2(logn)
  return 2*n*loglogn/logn + n/logn + math.sqrt(n)*loglogn

def mapToPos(xs):
  px = estimateProbs(xs)
  nvals = len(px)
  sortedpx = dict(sorted(px.items(), key=lambda i: i[1]))
  sortedpx = [(k,v) for k,v in sortedpx.items()]
  xToPos = dict()
  for idx, (x, v) in enumerate(sortedpx):
    xToPos[x] = nvals - idx - 1
  ys = [xToPos[x] for x in xs]
  return ys 
  
def DAC(xs):
  n = len(xs)
  xs = mapToPos(xs)
  N0 = sum(int(math.floor(math.log2(x) if x != 0 else 0)+1) for x in xs)
  b = int(math.ceil(math.sqrt(N0/n)))
  N = int(math.ceil(N0 + 2*n*math.sqrt(N0/n)))
  BitmapRanks = JacobsonBitmap(N//(b+1))
  S = N + BitmapRanks + 3*(math.log2(L)/b*math.log2(N))
  print("Binary N0 encoding: %d bits" % N0)
  print("Splitting into blocks of %d bits" % b)
  print("Vbyte encoding: %d bits" % N)
  print("BitmapRanks: %d bits" % (int(BitmapRanks)))
  print("Result space: %d bits\n" % (int(S)))
  return int(math.ceil(S))

def OkanoharaBitmap(n):
  logn = math.log2(n)
  loglogn = math.log2(logn)
  return n/logn + 2*n*loglogn/logn + n/(logn**2) + math.sqrt(n)*loglogn

def EliasH(xs):
  n = len(xs)
  xs = mapToPos(xs)
  Y = sum(int(math.floor(math.log2(x))) if x > 1 else 1 for x in xs)
  bitmapEntropy = n*math.log2(Y/n)
  bitmapDictionary = OkanoharaBitmap(Y+n)
  S = Y + bitmapEntropy + bitmapDictionary 
  print("Binary Y encoding: %d bits" % Y)
  print("Original bitmap len: %d bits" % (Y + n))
  print("0-order bitmap entropy: %d bits" % int(bitmapEntropy))
  print("Bitmap encoding: %d bits" % int(bitmapDictionary))
  print("Result space: %d bits\n" % (int(S)))
  return S

def RiceH(xs, k):
  xs = mapToPos(xs)
  n = len(xs)
  w = sum(xs)
  remainders = n*k
  bitmapLen = int((w/(2**k) + n))
  bitmapDictionary = OkanoharaBitmap(bitmapLen)
  bitmapEntropy = int(n*math.log2(w/n/(2**k)))
  S = remainders + bitmapEntropy + bitmapDictionary
  print("Remainders encoding: %d bits" % remainders)
  print("Original bitmap len: %d bits" % bitmapLen)
  print("0-order bitmap entropy: %d bits" % bitmapEntropy)
  print("Result space: %d bits\n" % (int(S)))
  return S

def EliasW(xs):
  xs = mapToPos(xs)
  n = len(xs)
  L = set([int(math.floor(math.log2(x) if x!=0 else 0)) for x in xs])
  r = len(L)
  Y = sum(int(math.floor(math.log2(x))) if x > 1 else 1 for x in xs)
  tree = n*math.ceil(math.log2(r))
  treeDicts = JacobsonBitmap(tree)
  S = tree + Y + treeDicts
  print("r: %d" % r)
  print("Binary Y encoding: %d bits" % int(Y))
  print("Tree: %d bits" % tree)
  print("Additional o(nlogr) bits: %d" % int(treeDicts))
  print("Result space: %d bits\n" % (int(S)))
  return S

def RiceW(xs, k):
  xs = mapToPos(xs)
  n = len(xs)
  L = set([int(math.floor((x/(2**k)))) for x in xs])
  r = len(L)
  remainders = n*k
  tree = n*math.ceil(math.log2(r))
  treeDicts = JacobsonBitmap(tree)
  S = tree + remainders + treeDicts
  print("Remainders encoding: %d bits" % remainders)
  print("Tree: %d bits" % tree)
  print("Additional o(nlogr) bits: %d" % int(treeDicts))
  print("Result space: %d bits\n" % (int(S)))
  return S


def FDA(xs):
  n = len(xs)
  px = estimateProbs(xs)
  hp = sum(-p*math.log2(p) for p in px.values())
  entropy = n*hp
  sum_storage = n*(math.log2(math.log2(L+2)) + 4)
  S = entropy + sum_storage
  print("Entropy: %.2f" % entropy)
  print("Sum_storage: %d" % sum_storage)
  print("Result space: %d bits\n" % (int(S)))
  return S



data = dict()

rng = np.random.default_rng(int(np.random.randint(0, 100)))
data['uni'] = np.random.randint(low=200, high=L, size = 1024, dtype=np.int32)

print("Original: %d bits\n" % (len(data['uni']) * 32))
DAC(data['uni'])
EliasH(data['uni'])
EliasW(data['uni'])
FDA(data['uni'])



  



  
  
