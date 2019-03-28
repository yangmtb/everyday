#include <iostream>
#include <thread>
#include <string>
#include <cstdio>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "socket.hpp"

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::thread;

void error(const char *msg)
{
  perror(msg);
  exit(0);
}

int main()
{
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0) {
    error("socket error");
  }
  sockaddr_in server;
  memset(&server, 0, sizeof(server));
  server.sin_addr.s_addr = inet_addr("127.0.0.1");
  server.sin_family = AF_INET;
  server.sin_port = htons(9999);
  int ret = connect(fd, (sockaddr *)&server, sizeof(server));
  if (ret < 0) {
    error("connect error");
  }
  Socket *s = new Socket(fd);
  const string str = "hello";
  for (int i = 0; i < 10; ++i) {
    ret = s->Write(str);
    usleep(10);
    if (2 == ret || 3 == ret) {
      cout << "boom" << i << endl;
      break;
    }
  }
  cout << "sleep" << endl;
  sleep(2);
  string tmp;
  ret = s->Read(tmp);
  cout << ret << " " << tmp << endl;
  sleep(1);
  close(fd);
  delete s;
  cout << "done" << endl;
  return 0;
}
