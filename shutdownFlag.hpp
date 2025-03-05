#ifndef SHUTDOWN_FLAG_HPP
#define SHUTDOWN_FLAG_HPP

#include <atomic>

extern std::atomic<bool> shutdownFlag;  // Declare as extern

#endif
