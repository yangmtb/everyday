#ifndef __SHAPE_HPP__
#define __SHAPE_HPP__

#include "common.hpp"
#include <vector>
#include <string>

using std::vector;
using std::string;

class Grid
{
public:
  Grid() {}
  Grid(Point p, Color c) : position(p), color(c) {}

  bool operator==(const Grid &rhs) {
    if (position.x == rhs.position.x && position.y == rhs.position.y) {
      return true;
    }
    return false;
  }

  Point position;
  Color color;
};

/*bool mySort(Grid &lhs, Grid &rhs) {
  if (lhs.position.x == rhs.position.x) {
    return lhs.position.y < rhs.position.y;
  }
  return lhs.position.x < rhs.position.x;
  }*/

class Shape
{
public:
  Shape() : mLayout(nullptr), mCenter{-1,-1} {}
  Shape(const Shape &s);
  Shape(Shape *layout);
  virtual ~Shape() {};

  void SetLayout(Shape *layout) {
    mLayout = layout;
  }
  Shape * GetLayout() {
    return mLayout;
  }
  bool Left();
  bool Right();
  bool Down();
  bool Rotate();
  bool IsValid() const;
  void Show() const;
  string GetString() const;
  int Eliminate(int ret = 0);

  Shape & operator+=(const Shape &rhs) {
    mContent.insert(mContent.begin(), rhs.mContent.begin(), rhs.mContent.end());
    return *this;
  }

private:
  void sort();

private:
  Point mCenter;
  vector<Grid> mContent;
  Shape *mLayout;
};

#endif//__SHAPE_HPP__
