#ifndef __CMAP_HPP__
#define __CMAP_HPP__

#include <vector>
#include "node.hpp"

using std::vector;

class CMap
{
public:
  CMap(int capacity);
  ~CMap();
  bool addNode(Node *pNode);
  void resetNode();
  bool setValue2MatrixForDirectedGraph(int row, int col, int val = 1);
  bool setValue2MatrixForUndirectedGraph(int row, int col, int val = 1);

  void printData();
  void printMatrix();

  void depthFirstTraverse(int nodeIndex);
  void breadthFirstTraverse(int nodeIndex);

  void breadthFirstTraverseImpl(vector<int> preVec);

private:
  int mCapacity;
  int mNodeCount;
  Node *mpNodeArray;
  int *mpMatrix;
  bool getValueFromMatrix(int row, int col, int &val);
};

#endif//__CMAP_HPP__
