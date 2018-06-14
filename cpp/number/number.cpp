#include <fstream>
#include <string>
#include <iostream>

using namespace std;

int makenum(string inname, string outname, int64_t len = 10000000)
{
  ifstream in(inname, ifstream::binary);
  ofstream out(outname, ofstream::binary);
  int64_t res = 0;
  int64_t step = len / 100;
  int p = 0;
  for (int i = 0; i < len; ++i) {
    if (0 == i % step) {
      p++;
      cout << p << "%(" << i << ")" << endl;
    }
    in.read((char *)&res, sizeof(res));
    out.write((char *)&res, sizeof(res));
  }

  in.close();
  out.close();
  return 0;
}

int main(int argc, char *argv[])
{
  makenum(argv[1], argv[2]);
  return 0;
}
