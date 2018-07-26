#include "sudoku.hpp"
#include <iostream>

using std::cout;
using std::endl;

int main(int argc, char *argv[])
{
  auto v = genRandQueue();
  for (auto x : v) {
    cout << x << " ";
  }
  cout << endl;
  //cout << "hello" << endl;
  Grid g(8, 7, 1);
  //g.Show();
  //cout << g << endl;
  Sudoku s;
  //cout << s << endl;
  s.Show();
  cout << "solve " << s.Solve() << endl;
  s.Show();
  return 0;
}
