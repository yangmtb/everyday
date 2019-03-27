#ifndef __COMMON_HPP__
#define __COMMON_HPP__

#include <chrono>
#include <random>
#include <string>

using std::string;

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

string base64Encode(unsigned char const *, unsigned int len);
string base64Decode(string const &s);

class SHA1
{
public:
  SHA1() { Reset(); }
  virtual ~SHA1() {}

  void Reset();
  bool Result(unsigned *messageDigestArray);
  void Input(const unsigned char *msgArray, unsigned len);
  void Input(const char *msgArray, unsigned len);
  void Input(unsigned char msgEle);
  void Input(char msgEle);
  SHA1 & operator<<(const char *msgArray);
  SHA1 & operator<<(const unsigned char *msgArray);
  SHA1 & operator<<(const char msgEle);
  SHA1 & operator<<(const unsigned char msgEle);

private:
  void ProcessMessageBlock();
  void PadMessage();
  inline unsigned CircularShift(int bits, unsigned word);

  unsigned H[5]; // message digest buffers
  unsigned LenLow;
  unsigned LenHigh;
  unsigned char MessageBlock[64]; // 512-bit message blocks
  int MessageBlockIndex; // index into message block array
  bool Computed; // is the digest computed
  bool Corrupted; // is the message digest corruped
};

#endif//__COMMON_HPP__
