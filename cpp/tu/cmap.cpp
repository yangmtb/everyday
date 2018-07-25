#include "cmap.hpp"
#include <iostream>
#include <cstdio>
#include <cstring>

using std::cout;
using std::endl;

CMap::CMap(int capacity)
{
  mCapacity = capacity;
  mNodeCount = 0;
  mpNodeArray = new Node[mCapacity];
  mpMatrix = new int[mCapacity * mCapacity];
  memset(mpMatrix, 0, mCapacity * mCapacity * sizeof(int));
}

CMap::~CMap()
{
  delete []mpNodeArray;
  delete []mpMatrix;
}

bool CMap::addNode(Node *pNode)
{
  if (nullptr == pNode) {
    return false;
  }
  mpNodeArray[mNodeCount++].mData = pNode->mData;
  return true;
}

void CMap::resetNode()
{
  for (int i = 0; i < mNodeCount; ++i) {
    mpNodeArray[i].mIsVisited = false;
  }
}

bool CMap::setValue2MatrixForDirectedGraph(int row, int col, int val)
{
  if (row < 0 || col < 0 || row > mCapacity || col > mCapacity) {
    return false;
  }
  mpMatrix[row*mCapacity + col] = val;
  return true;
}

bool CMap::setValue2MatrixForUndirectedGraph(int row, int col, int val)
{
  if (row < 0 || col < 0 || row > mCapacity || col > mCapacity) {
    return false;
  }
  mpMatrix[row*mCapacity + col] = val;
  mpMatrix[col*mCapacity + row] = val;
  return true;
}

void CMap::printData()
{
  for (int i = 0; i < mCapacity; ++i) {
    cout << mpNodeArray[i].mData << " ";
  }
  cout << endl;
}

void CMap::printMatrix()
{
  for (int i = 0; i < mCapacity; ++i) {
    for (int j = 0; j < mCapacity; ++j) {
      cout << mpMatrix[i*mCapacity + j] << " ";
    }
    cout << endl;
  }
}

bool CMap::getValueFromMatrix(int row, int col, int &val)
{
  if (row < 0 || col < 0 || row > mCapacity || col > mCapacity) {
    return false;
  }
  val = mpMatrix[row*mCapacity + col];
  return true;
}

void CMap::depthFirstTraverse(int nodeIndex)
{
  int value = 0;
  cout << mpNodeArray[nodeIndex].mData << " ";
  mpNodeArray[nodeIndex].mIsVisited = true;
  for (int i = 0; i < mCapacity; ++i) {
    getValueFromMatrix(nodeIndex, i, value);
    if (1 == value) {
      if (mpNodeArray[i].mIsVisited) {
        continue;
      } else {
        depthFirstTraverse(i);
      }
    } else {
      continue;
    }
  }
}

void CMap::breadthFirstTraverse(int nodeIndex)
{
  cout << mpNodeArray[nodeIndex].mData << " ";
  mpNodeArray[nodeIndex].mIsVisited = true;

  vector<int> curVec;
  curVec.push_back(nodeIndex);
  breadthFirstTraverseImpl(curVec);
}

void CMap::breadthFirstTraverseImpl(vector<int> preVec)
{
  int value = 0;
  vector<int> curVec;
  for (int j = 0; j < preVec.size(); ++j) {
    for (int i = 0; i < mCapacity; ++i) {
      getValueFromMatrix(preVec[j], i, value);
      if (1 == value) {
        if (mpNodeArray[i].mIsVisited) {
          continue;
        } else {
          cout << mpNodeArray[i].mData << " ";
          mpNodeArray[i].mIsVisited = true;
          curVec.push_back(i);
        }
      } else {
        continue;
      }
    }
  }
  if (curVec.empty()) {
    return;
  } else {
    breadthFirstTraverseImpl(curVec);
  }
}
