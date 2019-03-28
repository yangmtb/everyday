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
#include "threadpool.hpp"
using namespace std;

bool output_log = true;

#define exit_if(r, ...)                                                 \
  if (r) {                                                              \
    printf(__VA_ARGS__);                                                \
    printf("%s:%d error no: %d error msg %s\n", __FILE__, __LINE__, errno, strerror(errno)); \
    exit(1);                                                            \
  }

void setNonBlock(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  exit_if(flags < 0, "fcntl failed");
  int r = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
  exit_if(r < 0, "fcntl failed");
}

void updateEvents(int efd, int fd, int events, int op) {
  struct epoll_event ev;
  memset(&ev, 0, sizeof(ev));
  ev.events = events;
  ev.data.fd = fd;
  //printf("%s fd %d events read %d write %d\n", op == EPOLL_CTL_MOD ? "mod" : "add", fd, ev.events & EPOLLIN, ev.events & EPOLLOUT);
  int r = epoll_ctl(efd, op, fd, &ev);
  exit_if(r, "epoll_ctl failed");
}

void handleAccept(int efd, int fd) {
  struct sockaddr_in raddr;
  socklen_t rsz = sizeof(raddr);
  int cfd = accept(fd, (struct sockaddr *) &raddr, &rsz);
  exit_if(cfd < 0, "accept failed");
  sockaddr_in peer, local;
  socklen_t alen = sizeof(peer);
  int r = getpeername(cfd, (sockaddr *) &peer, &alen);
  exit_if(r < 0, "getpeername failed");
  printf("%2d accept a connection from %s:%d\n", cfd, inet_ntoa(raddr.sin_addr), ntohs(raddr.sin_port));
  setNonBlock(cfd);
  updateEvents(efd, cfd, EPOLLIN | EPOLLOUT | EPOLLET, EPOLL_CTL_ADD);
}
struct Con {
  string readed;
  size_t written;
  Con() : written(0) {}
};
map<int, Con> cons;

string httpRes;
int sendRes(int fd) {
  Con &con = cons[fd];
  if (!con.readed.length())
    return 0;
  string &readed = con.readed;
  //size_t left = httpRes.length() - con.written;
  size_t left = readed.length() - con.written;
  int wd = 0;
  while (left > 0) {
    wd = write(fd, readed.data() + con.written, left);
    if (wd <= 0) {
      if (wd < 0 && EINTR == errno) {
        wd = 0; // call write again
        continue;
      } else {
        return -1; // error
      }
    }
    left -= wd;
    con.written += wd;
  }
  return readed.length();
  while ((wd = ::write(fd, readed.data() + con.written, left)) > 0) {
    con.written += wd;
    left -= wd;
    if (output_log)
      printf("%d write %d bytes left: %lu\n", fd, wd, left);
  };
  if (left == 0) {
    //        close(fd); // 测试中使用了keepalive，因此不关闭连接。连接会在read事件中关闭
    //cons.erase(fd);
    return -1;
  }
  if (wd < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
    return -1;
  if (wd <= 0) {
    printf("write error for %d: %d %s\n", fd, errno, strerror(errno));
    close(fd);
    cons.erase(fd);
  }
  return 0;
}

int handleRead(int efd, int fd, void *vptr, size_t n) {
  size_t nleft = n;
  int nread = 0;
  char *ptr = (char *)vptr;
  string &readed = cons[fd].readed;
  while (nleft > 0) {
    nread = (int)read(fd, ptr, nleft);
    if (nread < 0) {
      if (EINTR == errno) {
        nread = 0; // call read again;
        continue;
      } else {
        return -1; // maybe errno is EAGAIN;
      }
    } else if (0 == nread) {
      break; // EOF;
    }
    readed.append(ptr, nread);
    nleft -= nread;
    ptr += nread;
  }
  return (n-nleft); // return >= 0;

  char buf[4096];
  int n1 = 0;
  while ((n = ::read(fd, buf, sizeof buf)) > 0) {
    if (output_log)
      printf("%d read %d bytes\n", fd, n);
    string &readed = cons[fd].readed;
    readed.append(buf, n);
    continue;
    if (readed.length() > 4) {
      if (readed.substr(readed.length() - 2, 2) == "\n\n" || readed.substr(readed.length() - 4, 4) == "\r\n\r\n") {
        //当读取到一个完整的http请求，测试发送响应
        sendRes(fd);
      }
    }
  }
  //sendRes(fd);
  if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
    return 2;
  //实际应用中，n<0应当检查各类错误，如EINTR
  if (n < 0) {
    printf("read %d error: %d %s\n", fd, errno, strerror(errno));
    return 3;
  }
  if (0 == n) {
    cout << "close:" << fd << endl;
    //close(fd);
    //cons.erase(fd);
    return 1;
  }
  return 0;
}

int handleWrite(int efd, int fd) {
  return sendRes(fd);
}

threadpool executor{20};

void loop_once(int efd, int lfd, int waitms) {
  const int kMaxEvents = 20;
  struct epoll_event activeEvs[100];
  int n = epoll_wait(efd, activeEvs, kMaxEvents, waitms);
  if (output_log)
    //printf("epoll_wait return %d\n", n);
  for (int i = 0; i < n; i++) {
    int fd = activeEvs[i].data.fd;
    int events = activeEvs[i].events;
    if (events & (EPOLLIN | EPOLLERR)) {
      if (fd == lfd) {
        handleAccept(efd, fd);
      } else {
        executor.Commit([](int efd, int fd){
                          char buf[1024+1] = {0};
                          int ret = handleRead(efd, fd, buf, 1024);
                          //cout << "close " << fd << endl;
                          //close(fd);
                          if (ret < 0 && EAGAIN != errno){
                            updateEvents(efd, fd, EPOLLIN | EPOLLOUT | EPOLLET, EPOLL_CTL_DEL);
                            close(fd);
                            cons.erase(fd);
                          } else if (0 == ret) {
                            updateEvents(efd, fd, EPOLLIN | EPOLLOUT | EPOLLET, EPOLL_CTL_DEL);
                            close(fd);
                            cons.erase(fd);
                          } else if (ret > 0) {
                            cout << ret << " buf:" << buf << endl;
                            updateEvents(efd, fd, EPOLLIN | EPOLLOUT | EPOLLET, EPOLL_CTL_MOD);
                          } else {
                            cout << ret << " " << errno << " boom :" << buf << endl;
                          }
                        }, efd, fd);
        continue;
        //handleRead(efd, fd);
      }
    } else if (events & EPOLLOUT) {
      executor.Commit([](int efd, int fd){
                        int ret = handleWrite(efd, fd);
                        cout << "write ret:" << ret << endl;
                        updateEvents(efd, fd, EPOLLIN | EPOLLET, EPOLL_CTL_MOD);
                        //printf("handling epollout\n");
                      }, efd, fd);
      continue;
      if (output_log)
        printf("handling epollout\n");
      handleWrite(efd, fd);
    } else {
      exit_if(1, "unknown event");
    }
  }
}

int main(int argc, const char *argv[]) {
  if (argc > 1) {
    output_log = false;
  }
  //output_log = false;
  ::signal(SIGPIPE, SIG_IGN);
  httpRes = "HTTP/1.1 200 OK\r\nConnection: Keep-Alive\r\nContent-Type: text/html; charset=UTF-8\r\nContent-Length: 1048576\r\n\r\n123456";
  for (int i = 0; i < 1048570; i++) {
    httpRes += '\0';
  }
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
  updateEvents(epollfd, listenfd, EPOLLIN, EPOLL_CTL_ADD);
  for (;;) {  //实际应用应当注册信号处理函数，退出时清理资源
    loop_once(epollfd, listenfd, 100000);
  }
  return 0;
}
