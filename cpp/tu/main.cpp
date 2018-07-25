#include <iostream>
#include "cmap.hpp"

using std::cout;
using std::endl;

int main()
{
  CMap *pMap = new CMap(8);
  Node *pNodeA = new Node('A');
  Node *pNodeB = new Node('B');
  Node *pNodeC = new Node('C');
  Node *pNodeD = new Node('D');
  Node *pNodeE = new Node('E');
  Node *pNodeF = new Node('F');
  Node *pNodeG = new Node('G');
  Node *pNodeH = new Node('H');

  pMap->addNode(pNodeA);
  pMap->addNode(pNodeB);
  pMap->addNode(pNodeC);
  pMap->addNode(pNodeD);
  pMap->addNode(pNodeE);
  pMap->addNode(pNodeF);
  pMap->addNode(pNodeG);
  pMap->addNode(pNodeH);

  delete pNodeA;
  delete pNodeB;
  delete pNodeC;
  delete pNodeD;
  delete pNodeE;
  delete pNodeF;
  delete pNodeG;
  delete pNodeH;

  pMap->setValue2MatrixForUndirectedGraph(0,1);
  pMap->setValue2MatrixForUndirectedGraph(0,3);
  pMap->setValue2MatrixForUndirectedGraph(1,2);
  pMap->setValue2MatrixForUndirectedGraph(1,5);
  pMap->setValue2MatrixForUndirectedGraph(3,6);
  pMap->setValue2MatrixForUndirectedGraph(3,7);
  pMap->setValue2MatrixForUndirectedGraph(6,7);
  pMap->setValue2MatrixForUndirectedGraph(2,4);
  pMap->setValue2MatrixForUndirectedGraph(4,5);

  pMap->printData();
  cout << endl;

  pMap->printMatrix();
  cout << endl;

  pMap->resetNode();

  pMap->depthFirstTraverse(0);
  pMap->resetNode();
  cout << endl;

  pMap->breadthFirstTraverse(0);
  pMap->resetNode();
  cout << endl;

  return 0;
}
