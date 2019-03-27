#ifndef __TIMER_HPP__
#define __TIMER_HPP__

#include <functional>
#include <chrono>
#include <thread>
#include <atomic>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <iostream>

class Timer
{
public:
  Timer() : mExpired(true), mTryToExpire(false) {}
  Timer(const Timer &t) : mExpired(t.mExpired.load()), mTryToExpire(t.mTryToExpire.load()) {}
  virtual ~Timer() { Expire(); }

  void StartTimer(int interval, std::function<void()> task) {
    if (!mExpired) return;
    mExpired = false;
    std::thread([this, interval, task]() {
                  while (!mTryToExpire) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(interval));
                    task();
                  }
                  {
                    std::lock_guard<std::mutex> locker(mMutex);
                    mExpired = true;
                    mExpiredCond.notify_one();
                  }
                }).detach();
  }
  void Expire() {
    if (mExpired) return;
    if (mTryToExpire) {
      return;
    }
    mTryToExpire = true;
    {
      std::unique_lock<std::mutex> locker(mMutex);
      mExpiredCond.wait(locker, [this] { return true == mExpired; });
      if (mExpired) {
        mTryToExpire = false;
      }
    }
  }

  template<typename callable, class... arguments>
  void SyncWait(int after, callable &&f, arguments&&... args) {
    std::function<typename std::result_of<callable(arguments...)>::type()> task(std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));
    std::this_thread::sleep_for(std::chrono::milliseconds(after));
    task();
  }

  template<typename callable, class... arguments>
  void AsyncWait(int after, callable &&f, arguments&&... args) {
    std::function<typename std::result_of<callable(arguments...)>::type()> task(std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));
    std::thread([after, task]() {
                  std::this_thread::sleep_for(std::chrono::milliseconds(after));
                  task();
                }).detach();
  }


  /*template<typename callable, class... arguments>
  void SyncWait(int after, int count, callable &&f, arguments&&... args) {
    std::function<typename std::result_of<callable(arguments...)>::type()> task(std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));
    bool alway = (-1 == count ? true : false);
    std::cout << "alway:" << alway << " ex:" << mExpired << "cnt:" << count << std::endl;
    do {
      for (int i = 0; !mExpired && i < count; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(after));
        task();
      }
    } while (alway && !mExpired);
  }

  template<typename callable, class... arguments>
  void AsyncWait(int after, int count, callable &&f, arguments&&... args) {
    std::function<typename std::result_of<callable(arguments...)>::type()> task(std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));
    std::thread([this, after, count, task]() {
                  bool alway = (-1 == count ? true : false);
                  while (alway && !mExpired) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(after));
                    task();
                  }
                  for (int i = 0; !mExpired && i < count; ++i) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(after));
                    task();
                  }
                }).detach();
  }
  */

private:
  std::atomic<bool> mExpired;
  std::atomic<bool> mTryToExpire;
  std::mutex mMutex;
  std::condition_variable mExpiredCond;
};

#endif//__TIMER_HPP__
