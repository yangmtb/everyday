#include <iostream>
#include <thread>
#include <queue>
#include <chrono>
#include <condition_variable>

using namespace std;

mutex mtx;
condition_variable produce, consume;

queue<int> q;
int maxSize = 20;

void consumer()
{
  while (true) {
    this_thread::sleep_for(chrono::milliseconds(1000));
    unique_lock<mutex> lck(mtx);
    consume.wait(lck, [] { return q.size() != 0; });
    cout << "consumer" << this_thread::get_id() << ":";
    q.pop();
    cout << q.size() << endl;
    produce.notify_all();
  }
}

void producer(int id)
{
  while (true) {
    this_thread::sleep_for(chrono::milliseconds(900));
    unique_lock<mutex> lck(mtx);
    produce.wait(lck, [] { return q.size() != maxSize; });
    cout << "-> producer" << this_thread::get_id() << ":";
    q.push(id);
    cout << q.size() << endl;
    consume.notify_all();
  }
}

void pause_thread(int n)
{
  this_thread::sleep_for(chrono::seconds(n));
  cout << "pause of " << n << " seconds ended" << endl;
}

const int cnt = 5;
mutex chopstick[cnt];

void tfn(int arg)
{
  int i = (int)arg;
  int left, right;
  if (cnt-1 == i) {
    left = 0;
    right = i;
  } else {
    left = i;
    right = i+1;
  }
  while (true) {
    this_thread::sleep_for(chrono::seconds(1));
    //unique_lock<mutex> lck(chopstick[left]);
    if (!chopstick[left].try_lock()) {
      continue;
    }
    //cout << "philosopher " << i << " fetches chopstick " << left << endl;
    if (!chopstick[right].try_lock()) {
      chopstick[left].unlock();
      continue;
    }
    //cout << "philosopher " << i << " fetches chopstick " << right << endl;
    cout << "philosopher " << i << " is eating" << endl;
    this_thread::sleep_for(chrono::seconds(2));
    chopstick[right].unlock();
    //cout << "philosopher " << i << " release chopstick " << right << endl;
    chopstick[left].unlock();
    //cout << "philosopher " << i << " release chopstick " << left << endl;
  }
}

int main(const int argc, const char *argv[])
{
  thread philosophers[cnt];
  for (int i = 0; i < cnt; ++i) {
    philosophers[i] = thread(tfn, i);
  }
  for (int i = 0; i < cnt; ++i) {
    philosophers[i].join();
  }
  system("pause");
  return 0;
  thread consumers[1], producers[2];
  consumers[0] = thread(consumer);
  for (int i = 0; i < 2; ++i) {
    producers[i] = thread(producer, i+1);
  }
  consumers[0].join();
  for (int i = 0; i < 2; ++i) {
    producers[i].join();
  }
  system("pause");
  return 0;
  cout << "spawning and detaching 3 threads..." << endl;
  thread(pause_thread, 1).detach();
  thread(pause_thread, 2).detach();
  thread(pause_thread, 3).detach();
  cout << "done spawning threads." << endl;
  cout << "the main thread will now pause for s seconds" << endl;
  pause_thread(5);
  return 0;
}
