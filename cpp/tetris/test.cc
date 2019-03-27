#include <iostream>
#include "service.hpp"

using std::cout;
using std::endl;

int main(const int argc, const char *argv[])
{
  Service::GetInstance()->Run(9999);
  cout << "done" << endl;
  return 0;
}
