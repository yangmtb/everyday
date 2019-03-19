#ifndef __USER_HPP__
#define __USER_HPP__

#include "game.hpp"

class User
{
public:
  User();
  virtual ~User();
private:
  Game *mGame;
};

#endif//__USER_HPP__
