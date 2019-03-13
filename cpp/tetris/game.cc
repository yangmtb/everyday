#include "game.hpp"

Game::Game() : mLayout(nullptr), mMove(nullptr), mNext(nullptr), mState(Ready), mLevel(1), mScore(0)
{
  mLayout = new Shape();
  mMove = new Shape(mLayout);
  //mNext = new Shape(mLayout);
  
}

bool Game::Run()
{
  ;
}
