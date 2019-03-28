#include "socket.hpp"
#include "common.hpp"
#include <string>
#include <iostream>
#include <unistd.h>
#include <cstring>

using std::cerr;
using std::endl;
using std::cout;
using std::string;

Socket::Socket(int fd)
  : mFD(fd)
{
}

// 0 is normal, 1 is again, 2 is need close, 3 is error
int Socket::Read(string &str) const
{
  char buf[4096];
  int n = 0;
  while ((n = read(mFD, buf, sizeof(buf))) > 0) {
    str.append(buf, n);
  }
  if (n < 0 && (EAGAIN == errno || EWOULDBLOCK == errno)) {
    cerr << errno <<" again? " << str << endl;
    return 1;
  } else if (n < 0) {
    cerr << n << " read error " << strerror(errno) << endl;
    return 3;
  } else if (0 == n) {
    cerr << "read 0" << endl;
    return 2;
  }
  return 0;
}

// 0 is normal, 1 is again, 2 is need close, 3 is error
int Socket::Write(const string &str) const
{
  if (0 == str.length()) {
    return 0;
  }
  size_t all = str.length();
  size_t left = all;
  size_t written = 0;
  int wd = 0;
  while ((wd = write(mFD, str.data()+written, left)) > 0) {
    written += wd;
    left -= wd;
  }
  if (0 == left) {
    return 0;
  }
  if (wd < 0 && (EAGAIN == errno || EWOULDBLOCK == errno)) {
    cout << "again? " << left << endl;
    return 1;
  } else if (0 == wd) {
    return 2;
  } else if (wd < 0) {
    cerr << "write error" << endl;
    return 3;
  }
  return 0;
}
