#include <iostream>
#include <cstdlib>
#include <ctime>
#include <array>
#include <functional>
#include <string>

using std::cout;
using std::endl;
using std::string;
using std::array;
using std::hash;

typedef array<array<int, 9>, 9> Sudoku;

void showSudoku(Sudoku data);
Sudoku genSudoku(int count);
bool checkValue(Sudoku data, int row, int col, int key);
bool solveSudoku(Sudoku &data, int &count);
bool dfs(Sudoku &data, int n, int &count);
bool dfs2(Sudoku &data, int n, int &count);
bool diffSudoku(Sudoku da0, Sudoku da1);

int main(int argc, char *argv[])
{
  auto da = genSudoku(24);
  cout << "begin" << endl;
  showSudoku(da);
  auto da1 = da;
  int cnt = 0;
  solveSudoku(da, cnt);
  showSudoku(da);
  //cout << "cnt:" << cnt << endl;
  cout << "begin1" << endl;
  showSudoku(da1);
  cnt = 0;
  dfs2(da1, 0, cnt);
  showSudoku(da1);
  cout << "diff:" << diffSudoku(da, da1) << endl;
  return 0;
}

void showSudoku(Sudoku data)
{
  for (auto x : data) {
    for (auto y : x) {
      cout << y << " ";
    }
    cout << endl;
  }
  cout << endl;
}

Sudoku genSudoku(int count)
{
  Sudoku fin{0};
  register unsigned int cou = 0;
  srand(time(nullptr));
  while (1) {
    Sudoku res{0};
    int row, col, val;
    while (count) {
      row = rand() % 9;
      col = rand() % 9;
      val = rand() % 9;
      if (0 == res[row][col]) {
        if (checkValue(res, row, col, val)) {
          res[row][col] = val;
          count--;
        }
      }
    }
    auto sol0 = res;
    auto sol1 = res;
    int cnt = 0;
    int cnt1 = 0;
    if (dfs(sol0, 0, cnt)) {
      dfs2(sol1, 0, cnt1);
      bool diff = diffSudoku(sol0, sol1);
      if (diff) {
        cout << cou++ << " diffff:" << diff << endl;
        fin = res;
        break;
      }
    } else {
      cout << cou++ << " no solvo" <<endl;
    }
  }
  return fin;
}

bool checkValue(Sudoku data, int row, int col, int key)
{
  for (int i = 0; i < 9; ++i) {
    if (key == data[row][i]) return false;
    if (key == data[i][col]) return false;
  }
  int x = row / 3 * 3;
  int y = col / 3 * 3;
  for (int i = x; i < x+3; ++i) {
    for (int j = y; j < y+3; ++j) {
      if (key == data[i][j]) return false;
    }
  }
  return true;
}

bool solveSudoku(Sudoku &data, int &count)
{
  return dfs(data, 0, count);
}

bool dfs(Sudoku &data, int n, int &count)
{
  if (n > 80) {
    count++;
    //showSudoku(data);
    return true;
  }
  if (0 != data[n/9][n%9]) {
    dfs(data, n+1, count);
  } else {
    for (int i = 1; i <= 9; ++i) {
      if (checkValue(data, n/9, n%9, i)) {
        data[n/9][n%9] = i;
        dfs(data, n+1, count);
        if (count > 0) return true;
        data[n/9][n%9] = 0;
      }
    }
  }
  return false;
}

bool dfs2(Sudoku &data, int n, int &count)
{
  if (n > 80) {
    count++;
    //showSudoku(data);
    return true;
  }
  if (0 != data[n/9][n%9]) {
    dfs2(data, n+1, count);
  } else {
    for (int i = 9; i > 0; --i) {
      if (checkValue(data, n/9, n%9, i)) {
        data[n/9][n%9] = i;
        dfs2(data, n+1, count);
        if (count > 0) return true;
        data[n/9][n%9] = 0;
      }
    }
  }
  return false;
}

bool diffSudoku(Sudoku da0, Sudoku da1)
{
  for (int i = 0; i < 9; ++i) {
    for (int j = 0; j < 9; ++j) {
      if (da0[i][j] != da1[i][j]) return false;
    }
  }
  return true;
}
