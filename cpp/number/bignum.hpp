#include <iostream>
#include <string>
#include <stack>

using namespace std;

class BigNum
{
public:
  BigNum();
  BigNum(string str);
  ~BigNum();

  friend ostream & operator<<(ostream &out, BigNum &obj);
  //friend istream & operator>>(istream &in, BigNum &obj);
  BigNum & operator+(const BigNum &s) const;

  stack<char> getData() {
    return mData;
  }

  bool getPlus() {
    return mPlus;
  }

private:
  bool mPlus;
  stack<char> mData;
};
