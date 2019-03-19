#ifndef __WEBSOCKET_RESPOND__
#define __WEBSOCKET_RESPOND__

#include <stdint.h>
#include <string>

class Websocket_Respond {
public:
	Websocket_Respond(std::string res);
	virtual ~Websocket_Respond();
  char * Get(int &sz) {
		sz = mSz;
		return mAll;
	}
private:
  char mAll[4096];
	int mSz;
	uint8_t mFin;
	uint8_t mOpcode;
	uint8_t mMask;
	uint8_t mMasking_key[4];
	uint64_t mPayload_length;
	char mPayload[2048];
};

#endif
