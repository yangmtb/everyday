#ifndef __SUDOKU_HPP__
#define __SUDOKU_HPP__

#include <array>
#include <ostream>

using std::ostream;
using std::array;

class Grid;
class Sign;
class Sudoku;

class Grid
{
public:
  Grid(int row, int col, int sign, int value = 0);
  ~Grid() {}
  bool IsOpend() { return mOpend; }
  friend ostream & operator<< (ostream &os, const Grid &g) {
    os << "row:" << g.mRow << " col:" << g.mCol << " sign:" << g.mSign << " value:" << g.mValue;
    return os;
  }

private:
  int mValue;
  array<int, 9> mDisplay;
  bool mOpend;
  int mRow;  // 行 1-9
  int mCol;  // 列 1-9
  int mSign; // 宫 1-9
};

class Sign
{
public:
  Sign(int sign);
  ~Sign(){}

private:
  int mSign;
  array<Grid, 9> mGrids;
};

class Sudoku
{
public:
  Sudoku();

private:
  array<Sign, 9> mSigns;
};

/*
typedef array<array<int, 9>, 9> Arr;

class Data
{
public:
  Data();
  ~Data();
  bool isLegal(int row, int col, int key);
  bool dfsByAsc(int idx);
  bool dfsByDes(int idx);
private:
  Arr mData;
};

class Sudoku
{
public:
  Sudoku();
  Sudoku(int count = 24);
  ~Sudoku();
private:
  Arr mSource;
  Arr mDisplay;
};
*/
#endif//__SUDOKU_HPP__
