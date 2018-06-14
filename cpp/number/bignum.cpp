#include "bignum.hpp"

BigNum::BigNum()
{
  mPlus = true;
}

BigNum::BigNum(string str)
{
  if ('-' == str.at(0)) {
    mPlus = false;
    str.erase(str.begin());
  } else if ('+' == str.at(0)){
    str.erase(str.begin());
    mPlus = true;
  } else {
    mPlus = true;
  }
  for (int i = 0; i < str.length(); ++i) {
    mData.push(str.at(i));
  }
}

BigNum::~BigNum()
{
  ;
}

ostream & operator<<(ostream &out, BigNum &obj)
{
  stack<char> tmp(obj.getData());
  while (!tmp.empty()) {
    out << tmp.top();
    tmp.pop();
  }
  return out;
}

BigNum & BigNum::operator+(const BigNum &s) const
{
  BigNum sum;
  return sum;
}
