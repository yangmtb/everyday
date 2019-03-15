#include "game.hpp"

Game::Game() : mLayout(nullptr), mMove(nullptr), mNext(nullptr), mState(Ready), mLevel(1), mScore(0)
{
  mLayout = new Shape();
  mMove = new Shape(mLayout);
  //mNext = new Shape(mLayout);
}

bool Game::Left()
{
  return mMove->Left();
}

bool Game::Right()
{
  return mMove->Right();
}

bool Game::Down()
{
  if (!mMove->Down()) {
    int eli = mLayout->Eliminate();
    if (eli > 0) {
      mScore += eli;
    }
    delete mMove;
    mMove = mNext;
    if (!mMove->IsValid()) {
      cout << "game over" << endl;
      return false;
    }
    mNext = new Shape(mLayout);
  }
  return true;
}

bool Game::Rotate()
{
  return mMove->Rotate();
}

bool Game::Run()
{
  ;
}
