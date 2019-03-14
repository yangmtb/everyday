#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/errno.h>
#include <sys/socket.h>

using std::cout;
using std::cerr;
using std::endl;
using std::thread;
using std::vector;
using std::mutex;

#define THREAD_MAX 20
#define LISTEN_MAX 20
#define SERVER_IP "127.0.0.1"

typedef struct {
  char ip4[128];
  int port;
  int fd;
}LISTEN_INFO;

// 服务器参数
static LISTEN_INFO s_listens[LISTEN_MAX];
// 线程池参数
static unsigned int s_thread_para[THREAD_MAX][8]; // 线程参数
//static pthread_t s_tid[THREAD_MAX]; // 线程ID
vector<thread> s_threads;
mutex s_mutex[THREAD_MAX]; // 线程锁

static int init_thread_pool(); // 初始化数据
static int init_listen(char *ip4, int port, int max_link); // 初始化监听

void test_server(unsigned int thread_para[]);

// 设置文件描述符为Nonblock
bool setNonblock(int fd)
{
  int flags = fcntl(fd, F_GETFL, 0);
  flags |= O_NONBLOCK;
  if (-1 == fcntl(fd, F_SETFL, flags)) {
    return false;
  }
  return true;
}

int main(const int argc, const char *argv[])
{
  int i,j,rc;
  int sock_listen; // 监听套接字
  int sock_cli; // 客户端连接
  int listen_index;
  int epfd;
  int nfds;
  struct epoll_event ev;
  struct epoll_event events[LISTEN_MAX];
  socklen_t addrlen; // 地址信息长度
  struct sockaddr_in addr4; // IPV4地址结构
  // 线程池初始化
  rc = init_thread_pool();
  if (0 != rc) {
    exit(-1);
  }
  // 初始化服务监听
  for (i = 0; i < LISTEN_MAX; ++i) {
    sprintf(s_listens[i].ip4, "%s", SERVER_IP);
    s_listens[i].port = 40000+i;
    // 创建监听
    rc = init_listen(s_listens[i].ip4, s_listens[i].port, 64);
    if (0 > rc) {
      cerr << "无法创建服务器监听" << s_listens[i].ip4 << ":" << s_listens[i].port << endl;
      exit(-1);
    } else {
      cout << "已创建服务器监听" << s_listens[i].ip4 << ":" << s_listens[i].port << endl;
    }
    s_listens[i].fd = rc;
  }
  // 设置集合
  epfd = epoll_create1(0);
  for (i = 0; i < LISTEN_MAX; ++i) {
    // 加入epoll事件集合
    ev.events = EPOLLIN|EPOLLET;
    ev.data.u32 = i; // 记录listen数组下标
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, s_listens[i].fd, &ev) < 0) {
      cerr << "向epoll集合添加套接字失败" << endl;
      exit(-1);
    }
  }
  // 服务循环
  while (true) {
    // 等待epoll事件
    nfds = epoll_wait(epfd, events, LISTEN_MAX, -1);
    // 处理epoll事件
    for (i = 0; i < nfds; ++i) {
      // 接收客户端连接
      listen_index = events[i].data.u32;
      sock_listen = s_listens[listen_index].fd;
      addrlen = sizeof(struct sockaddr_in);
      memset(&addr4, 0, addrlen);
      sock_cli = accept(sock_listen, (struct sockaddr *)&addr4, &addrlen);
      if (0 > sock_cli) {
        cerr << "接收客户端连接失败" << endl;
        continue;
      } else {
        char *myIP = inet_ntoa(addr4.sin_addr);
        cout << "accept a connection from " << myIP << endl;
      }
      setNonblock(sock_cli);
      // 查询空闲线程对
      for (j = 0; j < THREAD_MAX; ++j) {
        if (0 == s_thread_para[j][0]) break;
      }
      if (j >= THREAD_MAX) {
        cerr << "线程池已满，连接将被放弃" << endl;
        shutdown(sock_cli, SHUT_RDWR);
        close(sock_cli);
        continue;
      }
      // 复制有关参数
      s_thread_para[j][0] = 1; // 设置活动标志为“活动”
      s_thread_para[j][1] = sock_cli; // 客户端连接
      s_thread_para[j][2] = listen_index; // 服务索引
      // 线程解锁
      s_mutex[j].unlock();
    }
  }
  return 0;
}

static int init_thread_pool()
{
  int i, rc;
  // 初始化线程池参数
  for (i = 0; i < THREAD_MAX; ++i) {
    s_thread_para[i][0] = 0; // 设置线程占用标志为“空闲”
    s_thread_para[i][7] = i; // 线程池索引
    s_mutex[i].lock();
  }
  // 创建线程池
  for (i = 0; i < THREAD_MAX; ++i) {
    //thread(test_server, s_thread_para[i]);//.detach(); // &脱离
    s_threads.push_back(thread(test_server, s_thread_para[i]));
  }
  return 0;
}

static int init_listen(char *ip4, int port, int max_link)
{
  int sock_listen;
  struct sockaddr_in addr4;
  unsigned int optval;
  struct linger optval1;
  memset(&addr4, 0, sizeof(addr4));
  inet_pton(AF_INET, ip4, &(addr4.sin_addr));
  addr4.sin_family = AF_INET;
  addr4.sin_port = htons(port);
  sock_listen = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_listen < 0) {
    cerr << "创建socket异常" << endl;
    return -1;
  }
  // 设置SO_REUSEADDR选项（服务器快速重启）
  optval = 0x1;
  setsockopt(sock_listen, SOL_SOCKET, SO_REUSEADDR, &optval, 4);
  // 设置SO_LINGER选项（防范CLOSE_WAIT挂住所有套接字
  optval1.l_onoff = 1;
  optval1.l_linger = 60;
  setsockopt(sock_listen, SOL_SOCKET, SO_LINGER, &optval1, sizeof(optval1));
  if (bind(sock_listen, (struct sockaddr *)&addr4, sizeof(addr4)) < 0) {
    cerr << "bind socket异常" << endl;
    close(sock_listen);
    return -1;
  }
  if (listen(sock_listen, max_link) < 0) {
    cerr << "listen socket异常" << endl;
    close(sock_listen);
    return -1;
  }
  return sock_listen;
}

void test_server(unsigned int thread_para[])
{
  int sock_cli; // 客户端连接
  int pool_index; // 线程池索引
  int listen_index; // 监听索引
  char buff[32768] = {0}; // 传输缓冲区
  int i,j,len;
  char *p;
  // 线程脱离创建者
  pool_index = thread_para[7];
  s_threads[pool_index].detach();

 wait_unlock:
  s_mutex[pool_index].unlock();
 //线程变量内容复制
    sock_cli = thread_para[1];//客户端连接
    listen_index = thread_para[2];//监听索引

    //接收请求
    len = recv(sock_cli, buff, sizeof(buff), MSG_NOSIGNAL);
    printf("%s\n", buff);

    //构造响应
    p = buff;
    //HTTP头
    p += sprintf(p, "HTTP/1.1 200 OK\r\n");
    p += sprintf(p, "Content-Type: text/html\r\n");
    p += sprintf(p, "Connection: closed\r\n\r\n");
    //页面
    p += sprintf(p, "<html>\r\n<head>\r\n");
    p += sprintf(p, "<meta content=\"text/html; charset=UTF-8\" http-equiv=\"Content-Type\">\r\n");
    p += sprintf(p, "</head>\r\n");
    p += sprintf(p, "<body style=\"background-color: rgb(229, 229, 229);\">\r\n");

    p += sprintf(p, "<center>\r\n");
    p += sprintf(p, "<H3>连接状态</H3>\r\n");
    p += sprintf(p, "<p>服务器地址 %s:%d</p>\r\n", s_listens[listen_index].ip4, s_listens[listen_index].port);
    j = 0;
    for(i = 0; i < THREAD_MAX; i++) {
        if (0 != s_thread_para[i][0]) j++;
    }
    p += sprintf(p, "<H3>线程池状态</H3>\r\n");
    p += sprintf(p, "<p>线程池总数 %d 活动线程总数 %d</p>\r\n", THREAD_MAX, j);
    p += sprintf(p, "</center></body></html>\r\n");
    len = p - buff;

    //发送响应
    send(sock_cli, buff, len, MSG_NOSIGNAL);
    memset(buff, 0, 32768);

    //释放连接
    shutdown(sock_cli, SHUT_RDWR);
    close(sock_cli);

    //线程任务结束
    thread_para[0] = 0;//设置线程占用标志为"空闲"
    goto wait_unlock;
}
