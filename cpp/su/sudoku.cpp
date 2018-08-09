#include "sudoku.hpp"
#include <set>
#include <thread>
#include <iostream>
#include <ctime>
#include <cstdlib>

using std::set;
using std::thread;
using std::cout;
using std::endl;

Grid::Grid(int row, int col, int value)
  : mRow(row), mCol(col), mValue(value), mDisplay({0}), mOpend(false), mSign((row-1)/3*3 + (col-1)/3 + 1)
{
}

Sign::Sign(int sign)
  : mSign(sign)
{
  for (int i = 0; i < 9; ++i) {
    mGrids[i] = Grid((sign-1)/3*3+(i/3)+1, (sign-1)%3*3+(i%3)+1, sign);
  }
}

Sudoku::Sudoku()
{
  for (int i = 0; i < 9*9; ++i) {
    mGrids[i] = Grid(i/9+1, i%9+1, 0);
  }
  //init();
  randGen(mGrids);
}

void Sudoku::Show()
{
  for (int i = 0; i < 9*9; ++i) {
    cout << mGrids[i].GetValue() << " ";
    if (0 == (i+1)%9) {
      cout << endl;
    }
  }
}

bool Sudoku::Solve(Direction d)
{
  bool res = false;
  switch (d) {
  case Direction::Ascending:
    //cout << "begin ascending" << endl;
    //res = dfsByAsc(0);
    //cout << "end ascending" << endl;
    break;
  case Direction::Descending:
    cout << "begin descending" << endl;
    //res = dfsByDes(0);
    break;
  default:
    cout << "no match" << endl;
    res = false;
  }
  return res;
}

void Sudoku::init(int count)
{
  srand(time(nullptr));
  int n, key;
  int err = 0;
  while (1) {
    while (count) {
      n = rand() % 81;
      key = rand() % 9;
      if (0 == mGrids[n].GetValue()) {
        if (checkValue(mGrids, n, key)) {
          mGrids[n].SetValue(key);
          count--;
        }
      }
    }
    auto asc = mGrids;
    auto desc = mGrids;
    bool f0 = false;
    bool f1 = false;
    thread first ([&asc, &f0](bool &b) {
        b = dfsByAsc(asc, 0);
        //cout << "err:" << asc[0].GetValue() << endl;
      }, std::ref(f0));
    thread second ([&desc, &f1]() {
        f1 = dfsByDesc(desc, 0);
      });
    first.join();
    second.join();
    if (f0 && f1 && diff(asc, desc)) {
      Show();
      cout << "ok" << endl;
      break;
    }
    if (++err > 200) {
      break;
    }
  }
  cout << "err:" << err << endl;
}

void Sudoku::randGen(array<Grid, 9*9> &da) {
  srand(time(nullptr));
  int count = 18;
  int n, val;
  set<int> pool;
  for (int i = 0; i < 9*9; ++i) {
    pool.emplace(i);
  }
  while (count && !pool.empty()) {
    n = rand() % pool.size();
    val = rand() % 9;
    if (0 == da[n].GetValue()) {
      if (checkValue(da, n, val)) {
        da[n].SetValue(val);
        count--;
        //pool.erase((pool.begin()+n));
      }
    }
  }
  auto b = dfsByRand(da, 0);
  cout << "rand:" << b << endl;
}

bool Sudoku::checkValue(array<Grid, 9*9> &da, int n, int key)
{
  int row = n / 9;
  int col = n % 9;
  for (int i = 0; i < 9; ++i) {
    if (key == da[i*9+col].GetValue()) return false;
    if (key == da[9*row+i].GetValue()) return false;
  }
  int x = n / 9 / 3 * 3;
  int y = n % 9 / 3 * 3;
  for (int i = x; i < x+3; ++i) {
    for (int j = y; j < y+3; ++j) {
      if (key == da[i*9+j].GetValue()) return false;
    }
  }
  return true;
}

bool Sudoku::dfsByAsc(array<Grid, 9*9> &da, int n)
{
  if (n > 80) {
    return true;
  }
  if (0 != da[n].GetValue()) {
    if (dfsByAsc(da, n+1)) { return true; }
  } else {
    for (int i = 1; i < 10; ++i) {
      if (checkValue(da, n, i)) {
        da[n].SetValue(i);
        if (dfsByAsc(da, n+1)) { return true; }
        da[n].SetValue(0);
      }
    }
  }
  return false;
}

bool Sudoku::dfsByDesc(array<Grid, 9*9> &da, int n)
{
  if (n > 80) {
    return true;
  }
  if (0 != da[n].GetValue()) {
    if (dfsByDesc(da, n+1)) { return true; }
  } else {
    for (int i = 9; i > 0; --i) {
      if (checkValue(da, n, i)) {
        da[n].SetValue(i);
        if (dfsByDesc(da, n+1)) { return true; }
        da[n].SetValue(0);
      }
    }
  }
  return false;
}

bool Sudoku::dfsByRand(array<Grid, 9*9> &da, int n)
{
  if (n > 80) {
    return true;
  }
  if (0 != da[n].GetValue()) {
    if (dfsByRand(da, n+1)) { return true; }
  } else {
    for (auto i : genRandQueue()) {
      if (checkValue(da, n, i)) {
        da[n].SetValue(i);
        if (dfsByDesc(da, n+1)) { return true; }
        da[n].SetValue(0);
      }
    }
  }
  return false;
}

bool Sudoku::diff(const array<Grid, 9*9> a, const array<Grid, 9*9> b)
{
  for (int i = 0; i < 9*9; ++i) {
    if (a[i].GetValue() != b[i].GetValue()) { return false; }
  }
  return true;
}

vector<int> genRandQueue()
{
  vector<int> res (9, 0);
  for (int i = 1; i < 10; ++i) {
    while (1) {
      int idx = rand() % 9;
      if (0 == res[idx]) {
        res[idx] = i;
        break;
      }
    }
  }
  return res;
}
