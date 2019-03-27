#include "websocket_respond.h"
#include <cstring>
#include <iostream>

Websocket_Respond::Websocket_Respond(std::string res)
{
  //char mAll[4096] = {0};
  mSz = 0;
  memset(mAll, 0, 4096);
  memset(mPayload, 0, 2048);
  uint32_t len = res.size();
  //std::cout << "str:" << res;// << std::endl;
  //printf(" len:%x\n", len);
  mAll[mSz++] |= 0x81;
  if (len < 126) {
    mAll[mSz++] = len;
    memcpy(mAll+2, res.c_str(), len);
    mSz += len;
  } else if (len < 0xFFFF) {
    mAll[mSz++] = 126;
    mAll[mSz++] = (len >> 8) & 0xFF;
    mAll[mSz++] = len & 0xFF;
    //printf("%X %X %X\n", mAll[1], (unsigned char)mAll[2], (unsigned char)mAll[3]);
    memcpy(mAll+4, res.c_str(), len);
    mSz += len;
  } else {
    std::cout << "len:" << len << std::endl;
  }
}

Websocket_Respond::~Websocket_Respond()
{
}
