#ifndef __WEBSOCKET_HPP__
#define __WEBSOCKET_HPP__

#include "common.hpp"
#include "socket.hpp"
#include <string>
#include <map>

using std::string;
using std::map;

#define MAGIC_KEY "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

enum WEBSOCKET_STATUS {
                       WEBSOCKET_UNCONNECT = 0,
                       WEBSOCKET_HANDSHAKED,
};

class WebSocket : public Socket
{
public:
  WebSocket(int fd) : Socket(fd), mStatus(WEBSOCKET_UNCONNECT) {};
  virtual ~WebSocket() {}

public:
  int test();
  int IsHandShaked() { return WEBSOCKET_HANDSHAKED == mStatus; }
  int Handshake();
  int Read(string &str) const;
  int Write(const string &str) const;

private:
  int generateUpgradeString(string &out);
  int fetchHttpInfo(const string &str);
  int unpack(string &str) const;
  int pack(string &str) const;

private:
  map<string, string> mHeaders;
  WEBSOCKET_STATUS mStatus;
};

#endif//__WEBSOCKET_HPP__
