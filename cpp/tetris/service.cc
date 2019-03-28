#include "service.hpp"
#include "common.hpp"
#include "threadpool.hpp"
#include <iostream>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
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
    //cout << "wait:" << n << endl;
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
                            int ret = handleRead(fd);
                            if (2 == ret || 3 == ret) {
                              cout << "will close" << endl;
                              auto g = mGroups.at(fd);
                              if (nullptr != g->ws) {
                                delete g->ws;
                                g->ws = nullptr;
                              }
                              if (nullptr != g->game) {
                                delete g->game;
                                g->game = nullptr;
                              }
                              mGroups.erase(fd);
                              delete g;
                              g = nullptr;
                              close(fd);
                              UpdateEvents(mEpollFD, fd, EPOLLIN | EPOLLOUT | EPOLLET, EPOLL_CTL_DEL);
                            }
                            UpdateEvents(mEpollFD, fd, EPOLLIN | EPOLLOUT | EPOLLET, EPOLL_CTL_MOD);
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
  //cout << "socket accept" << endl;
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
  if (-1 == SetNonBlock(cfd)) {
    return -1;
  }
  Group *g = new Group;
  g->ws = new WebSocket(cfd);
  g->game = nullptr;
  mGroups.try_emplace(cfd, g);
  UpdateEvents(mEpollFD, cfd, EPOLLIN | EPOLLET, EPOLL_CTL_ADD);
  return 0;
}

int Service::handleRead(int fd)
{
  try {
    Group *g = mGroups.at(fd);
    WebSocket *s = g->ws;
    Game *game = g->game;
    /*string out;
    int ret = s->Read(out);
    if (2 == ret) { // close
      return ret;
    } else if (3 == ret) {
      cerr << "read boom" << endl;
      return ret;
    }
    cout << "recv:" << out << endl;
    ret = s->Write(out); // echo
    cout << "write:" << ret << endl;*/
    if (s->IsHandShaked()) {
      string command;
      int ret = s->Read(command);
      if (0 != ret) {
        return ret;
      }
      if ("join" == command) {
        ;
      } else if ("begin" == command) {
        if (nullptr == game) {
          game = new Game;
          g->game = game;
        }
        game->Run();
        mTimer.StartTimer(100, [this, game, s]() {
                                 if (game->IsOver()) {
                                   mTimer.Expire();
                                   return;
                                 }
                                 string cnt(game->GetJson());
                                 if (cnt.empty()) {
                                   cout << "empty json" << endl;
                                 } else {
                                   int ret = s->Write(cnt);
                                   if (2 == ret) {
                                     // should close
                                     return;
                                   } else if (3 == ret) {
                                     // error
                                     return;
                                   }
                                 }
                               });
      } else if ("right" == command) {
        game->addOperate(MoveRight);
      } else if ("left" == command) {
        game->addOperate(MoveLeft);
      } else if ("down" == command) {
        game->addOperate(MoveDown);
      } else if ("rotate" == command) {
        game->addOperate(MoveRotate);
      } else if ("pause" == command) {
        mTimer.Expire();
        game->Pause();
      } else if ("continue" == command) {
        mTimer.StartTimer(100, [this, game, s]() {
                                 if (game->IsOver()) {
                                   mTimer.Expire();
                                   return;
                                 }
                                 string cnt(game->GetJson());
                                 if (cnt.empty()) {
                                   cout << "empty json" << endl;
                                 } else {
                                   int ret = s->Write(cnt);
                                   if (2 == ret) {
                                     // should close
                                     return;
                                   } else if (3 == ret) {
                                     // error
                                     return;
                                   }
                                 }
                               });
        game->Continue();
      } else if ("close" == command) {
        return 2;
      } else {
        cout << command.length() << " unknow command:" << command << endl;
        command = "";
        ret = s->Read(command);
        cout << ret << " len:" << command.length() << " str:" << command << endl;
        //return 3;
      }
    } else {
      int ret = s->Handshake();
      if (0 != ret) {
        return ret;
      }
      cout << "handshake done" << endl;
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
