#ifndef __COMMON_HPP__
#define __COMMON_HPP__

#include <chrono>
#include <random>

extern std::minstd_rand0 gRandom;

enum Color {
            gray,
            cyan,
            yellow,
            purple,
            green,
            red,
            blue,
            orange
};

struct Point
{
  Point() {}
  Point(double _x, double _y) : x(_x), y(_y) {}

  double x;
  double y;
};

inline void Rotate270(const Point &src, const Point &r, Point &res)
{
  double c = 0; // cos270
  double s = -1;// sin270
  res.x = (src.x - r.x) * c - (src.y - r.y) * s + r.x;
  res.y = (src.x - r.x) * s + (src.y - r.y) * c + r.y;
}

#endif//__COMMON_HPP__
