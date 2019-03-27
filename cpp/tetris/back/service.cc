#include "socket.hpp"
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
#include "threadpool.hpp"

using std::cerr;
using std::endl;
using std::cout;
using std::string;

//Socket *Socket::sSocket = new Socket();
//Socket::GC Socket::GC::gc;

//Socket * Socket::GetInstance()
//{
//  return sSocket;
//}

int Socket::Run(unsigned short port)
{
  int ret = init(port);
  if (0 != ret) {
    cerr << "init error" << endl;
    return -1;
  }
  return loop(mEpollFD, mListenFD, 10000);
}

Socket::Socket()
  : mEpollFD(-1), mListenFD(-1)
{
}

int Socket::init(unsigned short port)
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

int Socket::loop(int waitms)
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
                            handleAccept();
                          });
        } else {
          // read
          executor.Commit([this, fd] () {
                            handleRead(fd);
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

int Socket::handleAccept()
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
  SetNonBlock(cfd);
  UpdateEvents(mEpollFD, cfd, EPOLLIN | EPOLLET, EPOLL_CTL_ADD);
  return 0;
}

string Socket::handleRead(int fd)
{
  cout << "socket read" << endl;
  string res;
  char buf[4096];
  int n = 0;
  while ((n = ::read(fd, buf, sizeof(buf))) > 0) {
    res.append(buf, n);
  }
  if (n < 0 && (EAGAIN == errno || EWOULDBLOCK == errno)) {
    return res;
  } else if (n < 0) {
    cerr << "read error" << endl;
    return "";
  } else if (0 == n) {
    cerr << "read 0" << endl;
    close(fd);
    return "";
  }
  return res;
}

int Socket::handleWrite(int fd)
{
  cout << "socket write" << endl;
  return 0;
}

int SetNonBlock(int fd)
{
  int flags = fcntl(fd, F_GETFL, 0);
  if (flags < 0) {
    cerr << "fcntl get failed" << endl;
    return -1;
  }
  int ret = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
  if (ret < 0) {
    cerr << "fcntl set nonblock failed" << endl;
    return -1;
  }
  return 0;
}

int UpdateEvents(int efd, int fd, int events, int op)
{
  epoll_event ev;
  memset(&ev, 0, sizeof(ev));
  ev.events = events;
  ev.data.fd = fd;
  int ret = epoll_ctl(efd, op, fd, &ev);
  if (0 != ret) {
    cerr << "epoll_ctl failed" << endl;
    return -1;
  }
  return 0;
}
