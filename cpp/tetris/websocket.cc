#include "websocket.hpp"
#include <sstream>
#include <iostream>
#include <cerrno>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

using std::cerr;
using std::cout;
using std::endl;
/*
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
  if (n < 0 && (EAGAIN == errno || EWOULDBLOCK == errno)) {
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
  return 0;
}
*/

int WebSocket::Read(string &str) const
{
  cout << "ws read" << endl;
  int ret = Socket::Read(str);
  if (0 != ret) {
    return ret;
  }
  cout << "unpack ws" << endl;
  ret = unpack(str);
  if (-1 == ret) {
    return ret;
  }
  return 0;
}

int WebSocket::Handshake()
{
  string tmp;
  int ret = Socket::Read(tmp);
  if (0 != ret && 1 != ret) {
    cerr << "read tmp error" << endl;
    return ret;
  }
  if (-1 == fetchHttpInfo(tmp)) {
    cerr << "fetch http info error" << endl;
    return -1;
  }
  tmp = "";
  if (-1 == generateUpgradeString(tmp)) {
    cerr << "generate upgrade string error" << endl;
    return -1;
  }
  cout << "tmp " << tmp << endl;
  ret = Socket::Write(tmp);
  if (0 != ret && 1 != ret) {
    cerr << "write tmp error" << endl;
    return ret;
  }
  mStatus = WEBSOCKET_HANDSHAKED;
  return 0;
}

int WebSocket::Write(const string &str) const
{
  cout << "ws write" << endl;
  string tmp(str);
  cout << "pack ws" << endl;
  int ret = pack(tmp);
  if (-1 == ret) {
    return ret;
  }
  return Socket::Write(tmp);
}

int WebSocket::unpack(string &str) const
{
  const unsigned char *data = (const unsigned char *)str.data();
  uint8_t fin, opcode, mask;
  uint8_t maskingKey[4];
  uint64_t payloadLength;
  char *parload;
  int pos = 0;
  fin = data[pos] >> 7;
  opcode = data[pos] & 0x0F;
  pos++;
  mask = data[pos] >> 7;
  if (1 == mask) {
    for (int i = 0; i < 4; ++i) {
      maskingKey[i] = data[pos+i];
    }
    pos += 4;
  }
  payloadLength = data[pos] & 0x7F;
  pos++;
  if (126 == payloadLength) {
    uint16_t length = 0;
    memcpy(&length, data+pos, 2);
    pos += 2;
    payloadLength = ntohs(length);
  } else if (127 == payloadLength) {
    uint32_t length = 0;
    memcpy(&length, data+pos, 4);
    pos += 4;
    payloadLength = ntohl(length);
  }
  try {
    char *tmp = new char[payloadLength];
    memset(tmp, 0, payloadLength);
    if (1 != mask) {
      memcpy(tmp, data+pos, payloadLength);
    } else {
      for (uint i = 0; i < payloadLength; ++i) {
        int j = i % 4;
        tmp[i] = data[pos+i] ^ maskingKey[j];
      }
    }
    str = tmp;
    delete[] tmp;
  } catch (const std::exception &e) {
    cerr << "exception " << e.what() << endl;
    return -1;
  }
  return 0;
}

int WebSocket::pack(string &str) const
{
  uint32_t len = str.length();
  char head[16] = {0};
  int pos = 0;
  head[pos++] |= 0x81;
  if (len < 126) {
    head[pos++] = len;
  } else if (len < 0xFFFF) {
    head[pos++] = 0x7E;
    head[pos++] = (len >> 8) & 0xFF;
    head[pos++] = len & 0xFF;
  } else {
    cerr << "not implement yet" << endl;
    return -1;
  }
  str.insert(0, head, pos);
  return 0;
}

int WebSocket::generateUpgradeString(string &out)
{
  cout << "out:" << out << endl;
	out += "HTTP/1.1 101 Switching Protocols\r\n";
	out += "Connection: upgrade\r\n";
	out += "Sec-WebSocket-Accept: ";
	std::string server_key = mHeaders["Sec-WebSocket-Key"];
	server_key += MAGIC_KEY;

	SHA1 sha;
	unsigned int message_digest[5];
	sha.Reset();
	sha << server_key.c_str();

	sha.Result(message_digest);
	for (int i = 0; i < 5; i++) {
		message_digest[i] = htonl(message_digest[i]);
	}
	server_key = base64Encode(reinterpret_cast<const unsigned char*>(message_digest),20);
	server_key += "\r\n";
	out += server_key.c_str();
	out += "Upgrade: websocket\r\n\r\n";
  cout << "out:" << out << endl;
  /*out += "HTTP/1.1 101 Switching Protocols\r\n";
  out += "Connection: upgrade\r\n";
  out += "Sec-WebSocket-Accept: ";
  if (mHeaders.end() == mHeaders.find("Sec-WebSocket-Key")) {
    return -1;
  }
  string serverKey(mHeaders["Sec-Websocket-Key"]);
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
  out += serverKey;
  out += "Upgrade: websocket\r\n\r\n";*/
  return 0;
}

int WebSocket::fetchHttpInfo(const string &str)
{
  std::stringstream s(str);
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
      mHeaders[key] = value;
    }
  }
  return 0;
}

int WebSocket::test()
{
  string tm = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
  cout << "length:" << tm.length() << endl;
  int ret = pack(tm);
  cout << "length:" << tm.length() << endl;
  for (int i = 0; i < tm.length(); ++i) {
    printf("%X ", (unsigned char *)(tm.data()[i]));
  }
  cout << endl;
  return 0;
}
