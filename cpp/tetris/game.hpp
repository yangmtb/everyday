#ifndef __GAME_HPP__
#define __GAME_HPP__

#include "shape.hpp"
#include "timer.hpp"
#include <queue>
#include <mutex>
#include <atomic>

using std::mutex;
using std::priority_queue;

enum Move {
           MoveLeft,
           MoveRight,
           MoveDown,
           MoveRotate
};

enum State {
            Ready,
            Running,
            Pause,
            Over
};

struct Operate
{
  friend bool operator<(Operate o1, Operate o2) {
    return o1.priority < o2.priority;
  }
  Move operate;
  int priority;
};

class Game
{
public:
  Game(int fd = -1);
  virtual ~Game();

  /*bool Left();
  bool Right();
  bool Down();
  bool Rotate();*/
  bool Run();
  void Continue() { mState = State::Running; }
  void Pause() { mState = State::Pause; }
  void Over() { mState = State::Over; }
  bool IsOver() { return State::Over == mState; }
  string GetJson();
  bool addOperate(Move);

private:
  bool addDown();

private:
  Timer mTimer;
  int mMaxQueueSize;
  priority_queue<Operate> mOperateQueue;
  mutex mMutexQueue;
  int mFd;
  Shape *mLayout;
  Shape *mMove;
  //Shape *mNext;
  std::atomic<State> mState;
  mutex mMutexState;
  std::condition_variable mCandState;
  int mLevel;
  std::atomic<unsigned long> mScore;
};

#endif//__GAME_HPP__
