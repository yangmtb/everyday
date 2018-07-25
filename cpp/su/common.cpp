#include "common.hpp"

bool dfsByAsc(Arr &src, int idx)
{
  if (idx > 81) {
    return true;
  }
  if (0 != src[idx/9][idx%9]) {
    dfsByAsc(src, idx+1);
  } else {
    for (int i = 1; i <= 9; ++i) {
      ;
    }
  }
  return false;
}

bool dfsByDes(Arr &src, int idx)
{
  return false;
}
