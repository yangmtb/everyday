#include "websocket.hpp"
#include <sstream>

WebsocketHandler::WebsocketHandler(int fd)
  : mFD(fd), mWritten(0), mStatus(WEBSOCKET_UNCONNECT)
{
}

WebsocketHandler::~WebsocketHandler()
{
}

int WebsocketHandler::process()
{
  int ret = recv();
  if (-1 == ret) {
    return -1;
  }
  if (WEBSOCKET_UNCONNECT) {
    return handshark();
  }
  return 0;
}

int WebsocketHandler::recv()
{
  char buf[4096] = {0};
  int n = 0;
  mReaded = "";
  while ((n = ::read(mFD, buf, sizeof(buf))) > 0) {
    mReaded.append(buf, n);
  }
  if (n < 0 && (EAGIN == errno || EWOULDBLOCK == errno)) {
    return 1;
  } else if (n < 0) {
    cout << "read " << mFD << " errno: " << errno << " " << strerror(errno) << endl;
    return -1;
  }
  return 0;
}

int WebsocketHandler::handshark()
{
  if (-1 == fetchHttpInfo()) {
    return -1;
  }
  if (-1 == generateUpgradeString()) {
    return -1;
  }
  mStatus = WEBSOCKET_HANDSHARKED;
  return 0;
}

int WebsocketHandler::generateUpgradeString()
{
  mWriteBuff += "HTTP/1.1 101 Switching Protocols\r\n";
  mWriteBuff += "Connection: upgrade\r\n";
  mWriteBuff += "Sec-WebSocket-Accept: ";
  if (mHeaderMap.end() == mHeaderMap.find("Sec-Websocket-Key")) {
    return -1;
  }
  string serverKey = mHeaderMap["Sec-Websocket-Key"];
  serverKey += MAGIC_KEY;
  SHA1 s1;
  unsigned int msgDigest[5];
  s1.Reset();
  s1 << serverKey.c_str();
  s1.Result(msgDigest);
  for (int i = 0; i < 5; ++i) {
    msgDigest[i] = htonl(msgDigest[i]);
  }
  serverKey = base64Encode(reinterpret_cast<const unsigned char *>(msgDigest), 20);
  serverKey += "\r\n";
  mWriteBuff += serverKey;
  mWriteBuff += "Upgrade: websocket\r\n\r\n";
  return 0;
}

int WebsocketHandler::fetchHttpInfo()
{
  std::stringstream s(mReaded);
  string request;
  std::getline(s, request);
  if ('\r' == request[request.size()-1]) {
    request.erase(request.end()-1);
  } else {
    return -1;
  }
  string header;
  string::size_type end;
  while (std::getline(s, header) && "\r" != header) {
    if ('\r' != header[header.size()-1]) {
      continue;
    } else {
      header.erase(header.end()-1);
    }
    end = header.find(": ", 0);
    if (string::npos != end) {
      string key = header.substr(0, end);
      string value = header.substr(end+2);
      mHeaderMap[key] = value;
    }
  }
  return 0
}
