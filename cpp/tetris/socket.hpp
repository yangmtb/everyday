#ifndef __SOCKET_HPP__
#define __SOCKET_HPP__

#include <string>
#include <mutex>

using std::string;

class Socket
{
public:
  Socket(int fd);
  virtual ~Socket() {}

public:
  virtual int Read(string &str) const;
  virtual int Write(const string &str) const;

private:
  int mFD;
};

int SetNonBlock(int fd);
int UpdateEvents(int efd, int fd, int events, int op);

#endif//__SOCKET_HPP__
