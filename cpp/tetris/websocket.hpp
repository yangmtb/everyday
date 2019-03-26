#ifndef __WEBSOCKET_HPP__
#define __WEBSOCKET_HPP__

#include "base64.h"
#include "common.hpp"
#include "timer.hpp"
#include <string>
#include <map>

using std::string;
using std::map;

#define MAGIC_KEY "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

enum WEBSOCKET_STATUS {
                       WEBSOCKET_UNCONNECT = 0,
                       WEBSOCKET_HANDSHARKED,
};

class WebsocketHandler
{
public:
  WEBSOCKET_STATUS mStatus;
  string mReaded;
  string mWriteBuff;
  size_t mWritten;

private:
  Timer mTimer;
  int mFD;
  map<string, string> mHeaderMap;

public:
  WebsocketHandler(int fd);
  virtual ~WebsocketHandler();
  int process();

private:
  int recv();
  int handshark();
  int generateUpgradeString();
  int fetchHttpInfo();
};

#endif//__WEBSOCKET_HPP__
