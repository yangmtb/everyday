#include "sudoku.hpp"
#include <iostream>

using std::cout;
using std::endl;

int main(int argc, char *argv[])
{
  cout << "hello" << endl;
  Grid g(8, 7, 1);
  //g.Show();
  cout << g << endl;
  return 0;
}
