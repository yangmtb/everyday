#include "game.hpp"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <thread>
//#include <boost/uuid/uuid.hpp>
//#include <boost/uuid/uuid_io.hpp>
//#include <boost/uuid/uuid_generators.hpp>

using std::cout;
using std::cerr;
using std::endl;

Game::Game(int fd) : mFd(fd), mLayout(nullptr), mMove(nullptr), mState(Ready), mLevel(1), mScore(0), mMaxQueueSize(64)
{
  //boost::uuids::uuid u = boost::uuids::random_generator()();
  //mID = boost::uuids::to_string(u);
  //cout << "id:" << mID << endl;
  mLayout = new Shape();
  mMove = new Shape(mLayout);
  mNext = new Shape(mLayout);
}

Game::~Game()
{
  if (mMove) delete mMove;
  if (mNext) delete mNext;
  if (mLayout) delete mLayout;
}

/*bool Game::Left()
{
  return mMove->Left();
}

bool Game::Right()
{
  return mMove->Right();
}

bool Game::Down()
{
  if (nullptr == mMove) {
    cout << "boom" << endl;
  }
  if (!mMove->Down()) {
    int eli = mLayout->Eliminate();
    //cout << "eli:" << eli << endl;
    if (eli > 0) {
      mScore += eli;
    }
    //mNext->SetLayout(mLayout);
    delete mMove;
    mMove = nullptr;
    //cout << "will next:" << endl;
    //mMove = mNext;
    if (!mMove->IsValid()) {
      std::cout << "game over" << std::endl;
      return false;
    }
    //mNext = new Shape(mLayout);
    //cout << "new " << endl;
  }
  return true;
}

bool Game::Rotate()
{
  return mMove->Rotate();
}*/

bool Game::Run()
{
  mTimer.StartTimer(1000, std::bind(&Game::addDown, this));
  mState = State::Running;
  std::thread ([this]() {
                 while (true) {
                   std::unique_lock<mutex> locker(mMutexState);
                   if (State::Over == mState) {
                     cout << "game    over" << endl;
                     mTimer.Expire();
                     break;
                   }
                   mCandState.wait(locker, [this] { return State::Running == mState && !mOperateQueue.empty(); });
                   std::unique_lock<mutex> lck(mMutexQueue);
                   while (!mOperateQueue.empty()) {
                     switch (mOperateQueue.top().operate) {
                     case MoveLeft:
                       mMove->Left();
                       break;
                     case MoveRight:
                       mMove->Right();
                       break;
                     case MoveDown:
                       if (!mMove->Down()) {
                         int eli = mLayout->Eliminate();
                         if (eli > 0) {
                           cout << "elimi: " << eli << endl;
                           mScore += eli;
                         }
                         delete mMove;
                         mMove = nullptr;
                         //cout << "will next:" << endl;
                         mNext->SetLayout(mLayout);
                         mMove = mNext;
                         mNext = nullptr;
                         if (!mMove->IsValid()) {
                           mState = State::Over;
                           std::cout << "game over" << std::endl;
                           return;
                         }
                         mNext = new Shape(nullptr);
                         //cout << "new " << endl;
                       }
                       break;
                     case MoveRotate:
                       mMove->Rotate();
                       break;
                     default:
                       cerr << "unkown operate " << mOperateQueue.top().operate << endl;
                       return;
                     }
                     mOperateQueue.pop();
                   }
                 }
               }).detach();
  return true;
}

string Game::GetJson()
{
  std::unique_lock<mutex> locker(mMutexQueue);
  if (nullptr == mMove) {
    cout << "null??" << endl;
    return "";
  }
  string tmp;
  tmp += "{";
  tmp += "\"score\":";
  tmp += to_string(mScore);
  tmp += ",";
  if (mNext) {
    tmp += "\"next\":";
    tmp += mNext->GetString();
    tmp += ",";
  }
  tmp += "\"body\":";
  string body = mMove->GetString();
  tmp += body;
  tmp += "}";
  return tmp;
}

bool Game::addDown()
{
  std::lock_guard<mutex> locker(mMutexQueue);
  if (State::Running == mState) {
    if (mOperateQueue.size() < mMaxQueueSize) {
      mOperateQueue.emplace(Operate{operate:MoveDown, priority:1});
    } else {
      cout << mOperateQueue.size() << " to big" << endl;
      return false;
    }
    mCandState.notify_one();
    return true;
  }
  return false;
}

bool Game::addOperate(Move op)
{
  std::lock_guard<mutex> locker(mMutexQueue);
  if (State::Running == mState) {
    if (mOperateQueue.size() < mMaxQueueSize) {
      mOperateQueue.emplace(Operate{operate:op, priority:2});
    } else {
      cout << mOperateQueue.size() << " to big" << endl;
      return false;
    }
    mCandState.notify_one();
    return true;
  }
  return false;
}
