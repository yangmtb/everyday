#ifndef __GAME_HPP__
#define __GAME_HPP__

#include <queue>

using std::priority_queue;

enum State {
            Ready,
            Running,
            Over
};

class Game
{
public:
  Game();
  virtual ~Game() {}

  bool Left();
  bool Right();
  bool Down();
  bool Rotate();
  bool Run();

private:
  Shape *mLayout;
  Shape *mMove;
  Shape *mNext;
  int mState;
  int mLevel;
  unsigned long mScore;
};

#endif//__GAME_HPP__
