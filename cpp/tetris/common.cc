#include "common.hpp"

std::minstd_rand0 gRandom (std::chrono::system_clock::now().time_since_epoch().count());

static const std::string base64_chars =
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";


static inline bool is_base64(unsigned char c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64Encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
  std::string ret;
  int i = 0;
  int j = 0;
  unsigned char char_array_3[3];
  unsigned char char_array_4[4];

  while (in_len--) {
    char_array_3[i++] = *(bytes_to_encode++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for(i = 0; (i <4) ; i++)
        ret += base64_chars[char_array_4[i]];
      i = 0;
    }
  }

  if (i)
  {
    for(j = i; j < 3; j++)
      char_array_3[j] = '\0';

    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
    char_array_4[3] = char_array_3[2] & 0x3f;

    for (j = 0; (j < i + 1); j++)
      ret += base64_chars[char_array_4[j]];

    while((i++ < 3))
      ret += '=';

  }

  return ret;

}

std::string base64Decode(std::string const& encoded_string) {
  size_t in_len = encoded_string.size();
  int i = 0;
  int j = 0;
  int in_ = 0;
  unsigned char char_array_4[4], char_array_3[3];
  std::string ret;

  while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
    char_array_4[i++] = encoded_string[in_]; in_++;
    if (i ==4) {
      for (i = 0; i <4; i++)
        char_array_4[i] = base64_chars.find(char_array_4[i]);

      char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

      for (i = 0; (i < 3); i++)
        ret += char_array_3[i];
      i = 0;
    }
  }

  if (i) {
    for (j = i; j <4; j++)
      char_array_4[j] = 0;

    for (j = 0; j <4; j++)
      char_array_4[j] = base64_chars.find(char_array_4[j]);

    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

    for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
  }

  return ret;
}

/*  
 *  Reset
 *
 *  Description:
 *      This function will initialize the sha1 class member variables
 *      in preparation for computing a new message digest.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *
 */
void SHA1::Reset()
{
  LenLow = 0;
  LenHigh = 0;
  MessageBlockIndex = 0;
  H[0] = 0x67452301;
  H[1] = 0xEFCDAB89;
  H[2] = 0x98BADCFE;
  H[3] = 0x10325476;
  H[4] = 0xC3D2E1F0;
  Computed = false;
  Corrupted = false;
}

/*
 *  Result
 *
 *  Description:
 *      This function will return the 160-bit message digest into the
 *      array provided.
 *
 *  Parameters:
 *      message_digest_array: [out]
 *          This is an array of five unsigned integers which will be filled
 *          with the message digest that has been computed.
 *
 *  Returns:
 *      True if successful, false if it failed.
 *
 *  Comments:
 *
 */
bool SHA1::Result(unsigned *msgDigestArray)
{
  if (Corrupted) {
    return false;
  } else if (!Computed) {
    PadMessage();
    Computed = true;
  }
  for (int i = 0; i < 5; ++i) {
    msgDigestArray[i] = H[i];
  }
  return true;
}

/*
 *  Input
 *
 *  Description:
 *      This function accepts an array of octets as the next portion of
 *      the message.
 *
 *  Parameters:
 *      message_array: [in]
 *          An array of characters representing the next portion of the
 *          message.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *
 */
void SHA1::Input(const unsigned char *msgArray, unsigned len)
{
  if (!len) {
    return;
  } else if (Computed || Corrupted) {
    Corrupted = true;
    return;
  }
  while (len-- && !Corrupted) {
    MessageBlock[MessageBlockIndex++] = (*msgArray & 0xFF);
    LenLow += 8;
    LenLow &= 0xFFFFFFFF;
    if (0 == LenLow) {
      LenHigh++;
      LenHigh &= 0xFFFFFFFF;
      if (0 == LenHigh) {
        Corrupted = true;
      }
    }
    if (64 == MessageBlockIndex) {
      ProcessMessageBlock();
    }
    msgArray++;
  }
}

/*
 *  Input
 *
 *  Description:
 *      This function accepts an array of octets as the next portion of
 *      the message.
 *
 *  Parameters:
 *      message_array: [in]
 *          An array of characters representing the next portion of the
 *          message.
 *      length: [in]
 *          The length of the message_array
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *
 */
void SHA1::Input(const char *msgArray, unsigned len)
{
  Input((unsigned char *)msgArray, len);
}

/*
 *  Input
 *
 *  Description:
 *      This function accepts a single octets as the next message element.
 *
 *  Parameters:
 *      message_element: [in]
 *          The next octet in the message.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *
 */
void SHA1::Input(unsigned char msgEle)
{
  Input(&msgEle, 1);
}

/*
 *  Input
 *
 *  Description:
 *      This function accepts a single octet as the next message element.
 *
 *  Parameters:
 *      message_element: [in]
 *          The next octet in the message.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *
 */
void SHA1::Input(char msgEle)
{
  Input((unsigned char *)&msgEle, 1);
}

/*
 *  operator<<
 *
 *  Description:
 *      This operator makes it convenient to provide character strings to
 *      the SHA1 object for processing.
 *
 *  Parameters:
 *      message_array: [in]
 *          The character array to take as input.
 *
 *  Returns:
 *      A reference to the SHA1 object.
 *
 *  Comments:
 *      Each character is assumed to hold 8 bits of information.
 *
 */
SHA1 & SHA1::operator<<(const char *msgArray)
{
  const char *p = msgArray;
  while (*p) {
    Input(*p);
    p++;
  }
  return *this;
}

/*
 *  operator<<
 *
 *  Description:
 *      This operator makes it convenient to provide character strings to
 *      the SHA1 object for processing.
 *
 *  Parameters:
 *      message_array: [in]
 *          The character array to take as input.
 *
 *  Returns:
 *      A reference to the SHA1 object.
 *
 *  Comments:
 *      Each character is assumed to hold 8 bits of information.
 *
 */
SHA1 & SHA1::operator<<(const unsigned char *msgArray)
{
  const unsigned char *p = msgArray;
  while (*p) {
    Input(*p);
    p++;
  }
  return *this;
}

/*
 *  operator<<
 *
 *  Description:
 *      This function provides the next octet in the message.
 *
 *  Parameters:
 *      message_element: [in]
 *          The next octet in the message
 *
 *  Returns:
 *      A reference to the SHA1 object.
 *
 *  Comments:
 *      The character is assumed to hold 8 bits of information.
 *
 */
SHA1 & SHA1::operator<<(const char msgEle)
{
  Input((unsigned char *)&msgEle, 1);
  return *this;
}

/*
 *  operator<<
 *
 *  Description:
 *      This function provides the next octet in the message.
 *
 *  Parameters:
 *      message_element: [in]
 *          The next octet in the message
 *
 *  Returns:
 *      A reference to the SHA1 object.
 *
 *  Comments:
 *      The character is assumed to hold 8 bits of information.
 *
 */
SHA1 & SHA1::operator<<(const unsigned char msgEle)
{
  Input(&msgEle, 1);
  return *this;
}

/*
 *  ProcessMessageBlock
 *
 *  Description:
 *      This function will process the next 512 bits of the message
 *      stored in the Message_Block array.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      Many of the variable names in this function, especially the single
 *      character names, were used because those were the names used
 *      in the publication.
 *
 */
void SHA1::ProcessMessageBlock()
{
  const unsigned K[] = { // constants defined for SHA-1
                        0x5A827999,
                        0x6ED9EBA1,
                        0x8F1BBCDC,
                        0xCA62C1D6
  };
  int t; // loop counter
  unsigned temp; // temporary word value
  unsigned W[80]; // word sequence
  unsigned A,B,C,D,E; // word buffers
  // initialize the first 16 words in the array W
  for (t = 0; t < 16; ++t) {
    W[t] = ((unsigned)MessageBlock[t*4]) << 24;
    W[t] |= ((unsigned)MessageBlock[t*4] + 1) << 16;
    W[t] |= ((unsigned)MessageBlock[t*4] + 2) << 8;
    W[t] |= ((unsigned)MessageBlock[t*4] + 3);
  }
  for (t = 16; t < 80; ++t) {
    W[t] = CircularShift(1, W[t-3]^W[t-8]^W[t-14]^W[t-16]);
  }
  A = H[0];
  B = H[1];
  C = H[2];
  D = H[3];
  E = H[4];
  for (t = 0; t < 20; ++t) {
    temp = CircularShift(5,A) + ((B&C)|((~B)&D)) + E + W[t] + K[0];
    temp &= 0xFFFFFFFF;
    E = D;
    D = C;
    C = CircularShift(30, B);
    B = A;
    A = temp;
  }
  for (t = 20; t < 40; ++t) {
    temp = CircularShift(5,A) + (B^C^D) + E + W[t] + K[1];
    temp &= 0xFFFFFFFF;
    E = D;
    D = C;
    C = CircularShift(30, B);
    B = A;
    A = temp;
  }
  for (t = 40; t < 60; ++t) {
    temp = CircularShift(5,A) + ((B&C)|(B&D)|(C&D)) + E + W[t] + K[2];
    temp &= 0xFFFFFFFF;
    E = D;
    D = C;
    C = CircularShift(30, B);
    B = A;
    A = temp;
  }
  for (t = 60; t < 80; ++t) {
    temp = CircularShift(5,A) + (B^C^D) + E + W[t] + K[3];
    temp &= 0xFFFFFFFF;
    E = D;
    D = C;
    C = CircularShift(30, B);
    B = A;
    A = temp;
  }
  H[0] = (H[0] + A) & 0xFFFFFFFF;
  H[1] = (H[1] + B) & 0xFFFFFFFF;
  H[2] = (H[2] + C) & 0xFFFFFFFF;
  H[3] = (H[3] + D) & 0xFFFFFFFF;
  H[4] = (H[4] + E) & 0xFFFFFFFF;
  MessageBlockIndex = 0;
}

/*
 *  PadMessage
 *
 *  Description:
 *      According to the standard, the message must be padded to an even
 *      512 bits.  The first padding bit must be a '1'.  The last 64 bits
 *      represent the length of the original message.  All bits in between
 *      should be 0.  This function will pad the message according to those
 *      rules by filling the message_block array accordingly.  It will also
 *      call ProcessMessageBlock() appropriately.  When it returns, it
 *      can be assumed that the message digest has been computed.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *
 */
void SHA1::PadMessage()
{
  if (MessageBlockIndex > 55) {
    MessageBlock[MessageBlockIndex++] = 0x80;
    while (MessageBlockIndex < 64) {
      MessageBlock[MessageBlockIndex++] = 0;
    }
    ProcessMessageBlock();
    while (MessageBlockIndex < 56) {
      MessageBlock[MessageBlockIndex++] = 0;
    }
  } else {
    MessageBlock[MessageBlockIndex++] = 0x80;
    while (MessageBlockIndex < 56) {
      MessageBlock[MessageBlockIndex++] = 0;
    }
  }
  MessageBlock[56] = (LenHigh >> 24) & 0xFF;
  MessageBlock[57] = (LenHigh >> 16) & 0xFF;
  MessageBlock[58] = (LenHigh >> 8) & 0xFF;
  MessageBlock[59] = (Length_High) & 0xFF;
  MessageBlock[60] = (Length_Low >> 24) & 0xFF;
  MessageBlock[61] = (Length_Low >> 16) & 0xFF;
  MessageBlock[62] = (Length_Low >> 8) & 0xFF;
  MessageBlock[63] = (Length_Low) & 0xFF;
  ProcessMessageBlock();
}

/*
 *  CircularShift
 *
 *  Description:
 *      This member function will perform a circular shifting operation.
 *
 *  Parameters:
 *      bits: [in]
 *          The number of bits to shift (1-31)
 *      word: [in]
 *          The value to shift (assumes a 32-bit integer)
 *
 *  Returns:
 *      The shifted value.
 *
 *  Comments:
 *
 */
unsigned SHA1::CircularShift(int bits, unsigned word)
{
  return ((word << bits) & 0xFFFFFFFF) | ((word & 0xFFFFFFFF) >> (32-bits));
}
