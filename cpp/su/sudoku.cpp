#include "sudoku.hpp"
#include <iostream>

using std::cout;
using std::endl;

Grid::Grid(int row, int col, int sign, int value)
  : mRow(row), mCol(col), mValue(value), mDisplay({0}), mOpend(false), mSign((row-1)/3*3 + (col-1)/3 + 1)
{
}

Sign::Sign(int sign)
  : mSign(sign)
{
  for (int i = 0; i < 9; ++i) {
    mGrids[i] = Grid(sign);
  }
}

Sudoku::Sudoku()
{
  for (int i = 0; i < 9; ++i) {
    mSigns[i] = Sign(i+1);
  }
}
