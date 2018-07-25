#include <iostream>

using std::cout;
using std::endl;

int data[9][9] = {0};

unsigned long long gnum = 0;

int main()
{
  
  for (int i = 0; i < 9 ; ++i) {
    for (int j = 0; j < 9; ++j) {
      for (int k = 0; k < 9; ++k) {
        data[i][j] = k+1;
        gnum++;
      }
    }
  }
  cout << "gnum:" << gnum << endl;
  return 0;
}
