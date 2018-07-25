#include <iostream>
#include <atomic>
#include <thread>
#include <vector>

using std::atomic;
using std::atomic_flag;
using std::cout;
using std::endl;
using std::vector;
using std::thread;

atomic<bool> ready(false);
atomic_flag winner = ATOMIC_FLAG_INIT;

void f0 (int id) {
  while (!ready) { std::this_thread::yield(); }
  for (volatile int i = 0; i < 100000000; ++i) {}
  if (!winner.test_and_set()) { cout << "thread #" << id << " won!!!" << endl; }
}

int testAtomic()
{
  vector<thread> ths;
  cout << "begin..." << endl;
  for (int i = 0; i < 50; ++i) {
    ths.push_back(thread([](int id) {
                           while (!ready) { std::this_thread::yield(); }
                           for (volatile int i = 0; i < 100000000; ++i) {}
                           if (!winner.test_and_set()) { cout << "thread ##" << id << " won!!!" << endl; }
                         }, i+1));
  }
  ready = true;
  for (auto &x : ths) {
    x.join();
  }
  return 0;
}

int main()
{
  testAtomic();
  return 0;
  int a = 1;
  auto f = [&] { cout << a << endl; };
  a = 2;
  f();
  //  return 0;
  int sum = 0;
  auto add = [&sum] (int a, int b) -> int {
               sum += a + b;
               cout << "sum:" << sum << endl;
               return a + b;
             };
  int ret = add(1, 2);
  cout << "ret:" << ret << " sum:" << sum << endl;
  ret = add(3, 4);
  cout << "ret:" << ret << " sum:" << sum << endl;
  return 0;
}
