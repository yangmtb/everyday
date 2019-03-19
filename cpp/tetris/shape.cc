#include "shape.hpp"
#include <iostream>
#include <algorithm>

using std::cout;
using std::endl;
using std::to_string;

Shape::Shape(const Shape &s) : mCenter(s.mCenter), mContent(s.mContent), mLayout(s.mLayout)
{
}

Shape::Shape(Shape *layout) : mLayout(layout)
{
  int ca = gRandom() % 7;
  Color c = Color(gRandom() % 7);
  //cout << "rand:" << ca << " color:" << c << endl;
  switch (ca) {
  case 0:
    mCenter = Point(1.5,4.5);
    mContent.emplace_back(Grid(Point(0,5), c)); // *
    mContent.emplace_back(Grid(Point(1,5), c)); // *
    mContent.emplace_back(Grid(Point(2,5), c)); // *
    mContent.emplace_back(Grid(Point(3,5), c)); // *
    break;
  case 1:                      // * *
    mCenter = Point(0.5, 4.5); // * *
    mContent.emplace_back(Grid(Point(0,4), c));
    mContent.emplace_back(Grid(Point(0,5), c));
    mContent.emplace_back(Grid(Point(1,4), c));
    mContent.emplace_back(Grid(Point(1,5), c));
    break;
  case 2:                    //  *
    mCenter = Point(1, 5);   //  * * *
    mContent.emplace_back(Grid(Point(0,4), c));
    mContent.emplace_back(Grid(Point(1,4), c));
    mContent.emplace_back(Grid(Point(1,5), c));
    mContent.emplace_back(Grid(Point(1,6), c));
    break;
  case 3:                    //      *
    mCenter = Point(1, 5);   //  * * *
    mContent.emplace_back(Grid(Point(0,6), c));
    mContent.emplace_back(Grid(Point(1,4), c));
    mContent.emplace_back(Grid(Point(1,5), c));
    mContent.emplace_back(Grid(Point(1,6), c));
    break;
  case 4:                    //    * *
    mCenter = Point(1, 5);   //  * *
    mContent.emplace_back(Grid(Point(0,5), c));
    mContent.emplace_back(Grid(Point(0,6), c));
    mContent.emplace_back(Grid(Point(1,4), c));
    mContent.emplace_back(Grid(Point(1,5), c));
    break;
  case 5:                    //  * *
    mCenter = Point(1, 5);   //    * *
    mContent.emplace_back(Grid(Point(0,4), c));
    mContent.emplace_back(Grid(Point(0,5), c));
    mContent.emplace_back(Grid(Point(1,5), c));
    mContent.emplace_back(Grid(Point(1,6), c));
    break;
  case 6:                    //    *
    mCenter = Point(1, 5);   //  * * *
    mContent.emplace_back(Grid(Point(0,5), c));
    mContent.emplace_back(Grid(Point(1,4), c));
    mContent.emplace_back(Grid(Point(1,5), c));
    mContent.emplace_back(Grid(Point(1,6), c));
    break;
  }
}

bool Shape::Left()
{
  Shape tmp(*this);
  for (auto &g : tmp.mContent) {
    g.position.y--;
  }
  if (!tmp.IsValid()) {
    return false;
  }
  if (-1 != mCenter.x) {
    mCenter.y--;
  }
  mContent = tmp.mContent;
  return true;
}

bool Shape::Right()
{
  Shape tmp(*this);
  for (auto &g : tmp.mContent) {
    g.position.y++;
  }
  if (!tmp.IsValid()) {
    return false;
  }
  if (-1 != mCenter.x) {
    mCenter.y++;
  }
  mContent = tmp.mContent;
  return true;
}

bool Shape::Down()
{
  Shape tmp(*this);
  for (auto &g : tmp.mContent) {
    g.position.x++;
  }
  if (!tmp.IsValid()) {
    *mLayout += *this;
    mLayout->sort();
    return false;
  }
  mCenter.x++;
  mContent = tmp.mContent;
  return true;
}

bool Shape::Rotate()
{
  Shape tmp(*this);
  Point p;
  for (auto &g : tmp.mContent) {
    Rotate270(g.position, mCenter, p);
    g.position = p;
  }
  if (!tmp.IsValid()) {
    return false;
  }
  mContent = tmp.mContent;
  return true;
}

void Shape::Show() const
{
  for (auto x : mContent) {
    cout << "pos:" << x.position.x << "," << x.position.y << "\t";
  }
  cout << endl;
  return;
}

string Shape::GetString() const
{
  string tmp;
  if (nullptr == mLayout) {
    return tmp;
  }
  tmp += "[";
  for (auto g : mContent) {
    tmp += "{";
    tmp += "\"x\":";
    tmp += to_string(int(g.position.x));
    tmp += ",";
    tmp += "\"y\":";
    tmp += to_string(int(g.position.y));
    tmp += ",";
    tmp += "\"c\":";
    tmp += to_string(g.color);
    tmp += "},";
  }
  for (auto g0 : mLayout->mContent) {
    tmp += "{";
    tmp += "\"x\":";
    tmp += to_string(int(g0.position.x));
    tmp += ",";
    tmp += "\"y\":";
    tmp += to_string(int(g0.position.y));
    tmp += ",";
    tmp += "\"c\":";
    tmp += to_string(g0.color);
    tmp += "},";
  }
  auto pos = tmp.find_last_of(',');
  if (string::npos != pos) {
    tmp.erase(pos);
  }
  tmp += "]";
  return tmp;
}

int Shape::Eliminate(int ret)
{
  for (auto g = mContent.begin(); g != mContent.end();) {
    if (0 == g->position.y) {
      //cout << "it's 0:" << g->position.x << "," << g->position.y << endl;
      auto tmp = g;
      bool done = true;
      for (int i = 0; i < 10; ++i,++tmp) {
        if (i != tmp->position.y) {
          //cout << i << " not != " << g->position.x << "," << tmp->position.y << endl;
          done = false;
          break;
        }
      }
      if (done) {
        ret++;
        cout << "will remove:" << g->position.x << endl;
        g = mContent.erase(g, g+10);
        for (auto b = mContent.begin(); b != g; ++b) {
          b->position.x++;
        }
        return Eliminate(ret);
      }
    }
    ++g;
  }
  return ret;
}

bool Shape::IsValid() const
{
  if (nullptr == mLayout) {
    return false;
  }
  for (auto g : mContent) {
    if (g.position.x >= 20) { return false; }
    if (g.position.y >= 10 || g.position.y < 0) { return false; }
  }
  for (auto x : mLayout->mContent) {
    for (auto y : mContent) {
      if (x == y) { return false; }
    }
  }
  return true;
}

void Shape::sort()
{
  std::sort(mContent.begin(), mContent.end(), [](Grid a, Grid b) {
                                           if (a.position.x == b.position.x) {
                                             return a.position.y < b.position.y;
                                           }
                                           return a.position.x < b.position.x;
                                         });
}
