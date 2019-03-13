#include "common.hpp"

std::minstd_rand0 gRandom (std::chrono::system_clock::now().time_since_epoch().count());
