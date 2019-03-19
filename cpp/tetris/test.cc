#include <iostream>
#include <queue>
#include <functional>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
#include "common.hpp"
#include "timer.hpp"
#include "game.hpp"
#include "network_interface.h"

using std::cin;
using std::cerr;
using std::cout;
using std::endl;
using std::thread;
using std::atomic;
using std::greater;
using std::vector;
using std::priority_queue;

struct node
{
  friend bool operator<(node n1, node n2) {
    return n1.priority < n2.priority;
  }
  int priority;
  int value;
};

bool EchoFunc(std::string &&s)
{
  cout << "test:" << s << endl;
  return true;
}

void testTimer()
{
  Timer t;
  t.StartTimer(1000, std::bind(EchoFunc, "hhhhh"));
  cout << "start" << endl;
  std::this_thread::sleep_for(std::chrono::seconds(4));
  cout << "try to expire timer" << endl;
  t.Expire();

  t.StartTimer(1000, std::bind(EchoFunc, "c11"));
  std::this_thread::sleep_for(std::chrono::seconds(4));
  cout << "try to expire timer" << endl;
  t.Expire();

  std::this_thread::sleep_for(std::chrono::seconds(4));
  cout << "will wait" << endl;
  t.SyncWait(2000, EchoFunc, "xxxx");
  t.AsyncWait(2000, EchoFunc, "ccc11");
  cout << "haha" << endl;
  std::this_thread::sleep_for(std::chrono::seconds(10));

  return;
}

void testPriorityQueue()
{
  const int len = 5;
  int i;
  int a[len] = {3,5,9,7,2};
  priority_queue<int> qi;
  for (i = 0; i < len; ++i) {
    qi.emplace(a[i]);
  }
  for (i = 0; i < len; ++i) {
    cout << qi.top() << " ";
    qi.pop();
  }
  cout << endl;
  priority_queue<int, vector<int>, greater<int>> qi2;
  for (i = 0; i < len; ++i) {
    qi2.emplace(a[i]);
  }
  for (i = 0; i < len; ++i) {
    cout << qi2.top() << " ";
    qi2.pop();
  }
  cout << endl;
  priority_queue<node> qn;
  node b[len];
	b[1].priority = 9; b[1].value = 5;
	b[3].priority = 8; b[3].value = 2;
  b[0].priority = 6; b[0].value = 1;
	b[2].priority = 2; b[2].value = 3;
	b[4].priority = 1; b[4].value = 4;
  for (i = 0; i < len; ++i) {
    qn.emplace(b[i]);
  }
  while (!qn.empty()) {
    cout << qn.top().priority << '\t' << qn.top().value << endl;
    qn.pop();
  }
}

atomic<bool> state(true);
void testThread()
{
  thread ([]() {
            while(state) {
              cout << "haha" << endl;
              std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
          }).detach();
  char a;
  cin >> a;
  state = false;
std::this_thread::sleep_for(std::chrono::milliseconds(100));
  return;
}

int main(const int argc, const char *argv[])
{
  //NETWORK_INTERFACE->run();
  //return 0;
  //testThread();
  //return 0;
  cout << "begin" << endl;
  char o;
  Game g;
  g.Run();
  do {
    cout << "input operate:";
    cin >> o;
    switch (o) {
    case 'a':
      g.addOperate(MoveLeft);
      break;
    case 'd':
      g.addOperate(MoveRight);
      break;
    case 's':
      g.addOperate(MoveDown);
      break;
    case 'w':
      g.addOperate(MoveRotate);
      break;
    case 'q':
      goto done;
    default:
      cerr << "invalid operate!!" << endl;
      //continue;
    }
    cout << g.GetJson() << endl;
  } while ('q' != o);
 done:
  g.Over();
  cout << "done" << endl;
  return 0;
}
