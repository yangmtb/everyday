#include "service.hpp"
#include "common.hpp"
#include "threadpool.hpp"
#include <iostream>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

using std::cerr;
using std::endl;
using std::cout;
using std::string;

Service *Service::sService = new Service();
Service::GC Service::GC::gc;

Service * Service::GetInstance()
{
  return sService;
}

int Service::Run(unsigned short port)
{
  int ret = init(port);
  if (0 != ret) {
    cerr << "init error" << endl;
    return -1;
  }
  return loop(10000);
}

Service::Service()
  : mEpollFD(-1), mListenFD(-1)
{
}

int Service::init(unsigned short port)
{
  mEpollFD = epoll_create(1);
  if (mEpollFD < 0) {
    cerr << "epoll create error" << endl;
    return -1;
  }
  mListenFD = socket(AF_INET, SOCK_STREAM, 0);
  if (mListenFD < 0) {
    cerr << "socket failed" << endl;
    return -1;
  }
  sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;
  int ret = ::bind(mListenFD, (sockaddr *)&addr, sizeof(sockaddr));
  if (0 != ret) {
    cerr << "bind failed" << endl;
    return -1;
  }
  ret = listen(mListenFD, 20);
  if (0 != ret) {
    cerr << "listen failed" << endl;
    return -1;
  }
  ret = SetNonBlock(mListenFD);
  if (0 != ret) {
    return -1;
  }
  ret = UpdateEvents(mEpollFD, mListenFD, EPOLLIN | EPOLLET, EPOLL_CTL_ADD);
  return 0;
}

int Service::loop(int waitms)
{
  const int cMaxEvents = 66;
  std::threadpool executor{cMaxEvents};
  epoll_event activeEvents[cMaxEvents];
  while (true) {
    int n = epoll_wait(mEpollFD, activeEvents, cMaxEvents, waitms);
    if (n < 0) {
      cerr << "epoll wait error" << endl;
      return -1;
    }
    cout << "wait:" << n << endl;
    for (int i = 0; i < n; ++i) {
      int fd = activeEvents[i].data.fd;
      uint32_t events = activeEvents[i].events;
      if ((EPOLLIN | EPOLLERR) & events) {
        if (fd == mListenFD) {
          // accept
          executor.Commit([this] () {
                            if (0 != handleAccept()) {
                              cerr << "accept error" << endl;
                            }
                          });
        } else {
          // read
          executor.Commit([this, fd] () {
                            if (0 != handleRead(fd)) {
                              cerr << "read error" << endl;
                            }
                            UpdateEvents(mEpollFD, fd, EPOLLOUT | EPOLLET, EPOLL_CTL_MOD);
                          });
        }
      } else if (EPOLLOUT & events) {
        // write
        executor.Commit([this, fd] () {
                          handleWrite(fd);
                        });
      } else {
        // unknown event
        return -1;
      }
    }
  }
  return 0;
}

int Service::handleAccept()
{
  cout << "socket accept" << endl;
  sockaddr_in raddr;
  memset(&raddr, 0, sizeof(raddr));
  socklen_t rsz = sizeof(raddr);
  int cfd = accept(mListenFD, (sockaddr *)&raddr, &rsz);
  if (cfd < 0) {
    cerr << "accept error" << endl;
    return -1;
  }
  sockaddr_in peer;
  memset(&peer, 0, sizeof(peer));
  socklen_t alen = sizeof(peer);
  int ret = getpeername(cfd, (sockaddr *)&peer, &alen);
  if (ret < 0) {
    cerr << "getpeername error" << endl;
    return -1;
  }
  cout << "addr " << inet_ntoa(raddr.sin_addr) << ":" << ntohs(raddr.sin_port) << endl;
  //SetNonBlock(cfd);
  mWebSockets.try_emplace(cfd, new WebSocket(cfd));
  UpdateEvents(mEpollFD, cfd, EPOLLIN | EPOLLET, EPOLL_CTL_ADD);
  return 0;
}

int Service::handleRead(int fd)
{
  cout << "socket read" << endl;
  try {
    WebSocket *s = mWebSockets.at(fd);
    if (s->IsHandShaked()) {
      string command;
      int ret = s->Read(command);
      if (0 != ret) {
        return ret;
      }
      cout << "command:" << command << endl;
    } else {
      int ret = s->Handshake();
      if (0 != ret) {
        return ret;
      }
    }
  } catch (std::exception &e) {
    cerr << "at err: " << e.what() << endl;
    return -1;
  }
  return 0;
}

int Service::handleWrite(int fd)
{
  cout << "socket write" << endl;
  return 0;
}
