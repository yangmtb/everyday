#ifndef __SUDOKU_HPP__
#define __SUDOKU_HPP__

#include <vector>
#include <array>
#include <ostream>

enum class Direction{
  Ascending,
  Descending
};


using std::vector;
using std::ostream;
using std::array;

class Grid;
class Sign;
class Sudoku;

class Grid
{
public:
  Grid() {}
  Grid(int row, int col, int value = 0);
  ~Grid() {}
  bool IsOpend() { return mOpend; }
  friend ostream & operator<< (ostream &os, const Grid &g) {
    os << "row:" << g.mRow << " col:" << g.mCol << " sign:" << g.mSign << " value:" << g.mValue;
    return os;
  }
  int GetValue() const { return mValue; }
  void SetValue(int v) { mValue = v; }

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
  Sign() {}
  Sign(int sign);
  ~Sign() {}
  friend ostream & operator<< (ostream &os, const Sign &s) {
    for (auto x : s.mGrids) {
      os << x << "\n";
    }
    return os;
  }
  //bool isValid();

private:
  int mSign;
  array<Grid, 9> mGrids;
};

class Sudoku
{
public:
  Sudoku();
  friend ostream & operator<< (ostream &os, const Sudoku &s) {
    for (auto x : s.mGrids) {
      os << x << "\n";
    }
    return os;
  }
  void Show();
  bool Solve(Direction d = Direction::Ascending);

private:
  void init(int count = 28);
  static void randGen(array<Grid, 9*9> &da);
  static bool checkValue(array<Grid, 9*9> &da, int n, int key);
  static bool dfsByAsc(array<Grid, 9*9> &da, int n);
  static bool dfsByDesc(array<Grid, 9*9> &da, int n);
  static bool dfsByRand(array<Grid, 9*9> &da, int n);
  static bool diff(const array<Grid, 9*9> a, const array<Grid, 9*9> b);

private:
  array<Grid, 9*9> mGrids;
  //array<Grid, 9*9> mDone;
};

vector<int> genRandQueue();

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
