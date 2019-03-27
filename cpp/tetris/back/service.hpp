#ifndef __SOCKET_HPP__
#define __SOCKET_HPP__

#include <string>
#include <mutex>

using std::string;
using std::mutex;

class Socket
{
public:
  //static Socket * GetInstance();
  int Run(unsigned short port = 8899);
  Socket();
  virtual ~Socket() {}

private:
  int init(unsigned short port);
  int loop(int efd, int lfd, int waitms);
public:
  virtual int handleAccept();
  virtual int handleRead(int fd);
  virtual int handleWrite(int fd);

private:
  //static Socket *sSocket;
  int mEpollFD;
  int mListenFD;
  string mRecvStr;
  mutex mRecvMtx;
  string mSendStr;
  size_t mSended;
  mutex mSendMtx;

  /*class GC
  {
  public:
    ~GC()
    {
      if (nullptr != sSocket) {
        delete sSocket;
        sSocket = nullptr;
      }
    }
    static GC gc;
  };*/
};

int SetNonBlock(int fd);
int UpdateEvents(int efd, int fd, int events, int op);

#endif//__SOCKET_HPP__
