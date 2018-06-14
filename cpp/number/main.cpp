#include "bignum.hpp"

void test0()
{
  BigNum s0("1234");
  BigNum s1("5");
  BigNum s = s0+s1;
  cout << sizeof(long long) << " done" << endl;
}

int main(int argc, char *argv[])
{
  cout << sizeof(long) << " done" << endl;
  return 0;
}
