/*
 * 编译：c++ -o epoll-et epoll-et.cc
 * 运行： ./epoll-et
 * 测试：curl -v localhost
 * 客户端发送GET请求后，服务器返回1M的数据，会触发EPOLLOUT，从epoll-et输出的日志看，EPOLLOUT事件得到了正确的处理
 */
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <map>
#include <string>
#include <iostream>
#include <atomic>
#include "threadpool.hpp"

using namespace std;

bool output_log = true;

#define exit_if(r, ...)                                                 \
  if (r) {                                                              \
    printf(__VA_ARGS__);                                                \
    printf("%s:%d error no: %d error msg %s\n", __FILE__, __LINE__, errno, strerror(errno)); \
    exit(1);                                                            \
  }

void setNonBlock(int fd)
{
  int flags = fcntl(fd, F_GETFL, 0);
  exit_if(flags < 0, "fcntl failed");
  int r = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
  exit_if(r < 0, "fcntl failed");
}

int updateEvents(int efd, int fd, int events, int op)
{
  struct epoll_event ev;
  memset(&ev, 0, sizeof(ev));
  ev.events = events;
  ev.data.fd = fd;
  int r = epoll_ctl(efd, op, fd, &ev);
  if (0 != r) {
    cout << errno << " ctl " << fd << endl;
    return r;
  }
  return 0;
}

atomic_uint all{0};

atomic_uint maxfd{0};

void handleAccept(int workid, int efd, int fd)
{
  int cn = 0;
  struct sockaddr_in raddr;
  socklen_t rsz = sizeof(raddr);
  int cfd;
  while (1) {
    memset(&raddr, 0, sizeof(raddr));
    cfd = accept(fd, (struct sockaddr *) &raddr, &rsz);
    if (-1 == cfd && EAGAIN == errno) {
      //all += cn;
      //cout << workid << " accept done " << cn << endl;
      break;
    } else if (-1 == cfd) {
      cout << errno << " accpet error " << strerror(errno) << " maxfd: " << maxfd << endl;
      usleep(10);
    }
    if (maxfd < cfd) {
      maxfd = cfd;
    }
    cn++;
    setNonBlock(cfd);
    updateEvents(efd, cfd, EPOLLIN | EPOLLONESHOT | EPOLLET, EPOLL_CTL_ADD);
  }
  //updateEvents(efd, fd, EPOLLIN | EPOLLONESHOT | EPOLLET, EPOLL_CTL_MOD);
}

int handleRead(int efd, int fd)
{
  char buf[4096];
  int n = 0;
  while ((n = ::read(fd, buf, sizeof(buf))) > 0) {
    ;
  }
  if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
    return 1;
  }
  if (n < 0) {
    printf("read %d error: %d %s\n", fd, errno, strerror(errno));
    return -1;
  }
  return 0;
}

threadpool executor{20};

void loop_once(int workid, int efd, int lfd, int waitms)
{
  const int kMaxEvents = 100;
  epoll_event *activeEvs = (epoll_event *)calloc(kMaxEvents, sizeof(epoll_event));
  int n = 0;
  while(1) {
    n = epoll_wait(efd, activeEvs, kMaxEvents, waitms);
    for (int i = 0; i < n; i++) {
      int fd = activeEvs[i].data.fd;
      int events = activeEvs[i].events;
      if (events & EPOLLHUP) {
        //cout << fd << " it's epollerr event " << events << endl;
        close(fd);
      } else if (events & (EPOLLIN | EPOLLERR)) {
        if (fd == lfd) {
          executor.Commit(handleAccept, workid, efd, fd);
        } else {
          handleRead(efd, fd);
          updateEvents(efd, fd, EPOLLIN | EPOLLONESHOT | EPOLLET, EPOLL_CTL_DEL);
          usleep(1);
          close(fd);
        }
      } else if (events & EPOLLOUT) {
        ;
      } else {
        ;
      }
    }
  }
  free(activeEvs);
}

int main(int argc, const char *argv[]) {
  if (argc > 1) {
    output_log = false;
  }
  ::signal(SIGPIPE, SIG_IGN);
  unsigned short port = 9999;
  int epollfd = epoll_create(1);
  exit_if(epollfd < 0, "epoll_create failed");
  int listenfd = socket(AF_INET, SOCK_STREAM, 0);
  exit_if(listenfd < 0, "socket failed");
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof addr);
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;
  int r = ::bind(listenfd, (struct sockaddr *) &addr, sizeof(struct sockaddr));
  exit_if(r, "bind to 0.0.0.0:%d failed %d %s", port, errno, strerror(errno));
  r = listen(listenfd, 20);
  exit_if(r, "listen failed %d %s", errno, strerror(errno));
  printf("fd %d listening at %d\n", listenfd, port);
  setNonBlock(listenfd);
  updateEvents(epollfd, listenfd, EPOLLIN | EPOLLET, EPOLL_CTL_ADD);
  //loop_once(1, epollfd, listenfd, 100000);
  //return 0;
  vector<thread> pools;
  const int poolcount = 16;
  for (int i = 0; i < poolcount; ++i) {  //实际应用应当注册信号处理函数，退出时清理资源
    cout << "thread" << i << " start" << endl;
    pools.push_back(thread (loop_once, i, epollfd, listenfd, 100000));
  }
  cout << "ok" << endl;
  for (auto &t : pools) {
    t.join();
  }
  return 0;
}
