#ifndef __SERVICE_HPP__
#define __SERVICE_HPP__

#include "websocket.hpp"
#include <string>
#include <mutex>
#include <map>

using std::string;
using std::mutex;
using std::map;

class Service
{
public:
  static Service * GetInstance();
  int Run(unsigned short port = 8899);

private:
  Service();
  virtual ~Service() {}
  int init(unsigned short port);
  int loop(int waitms);
  int handleAccept();
  int handleRead(int fd);
  int handleWrite(int fd);

private:
  static Service *sService;
  int mEpollFD;
  int mListenFD;
  string mRecvStr;
  mutex mRecvMtx;
  string mSendStr;
  size_t mSended;
  mutex mSendMtx;
  map<int, WebSocket *> mWebSockets;

  class GC
  {
  public:
    ~GC()
    {
      if (nullptr != sService) {
        delete sService;
        sService = nullptr;
      }
    }
    static GC gc;
  };
};

#endif//__SERVICE_HPP__
