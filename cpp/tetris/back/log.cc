#include "log.hpp"
#include <cstdarg>

void DEBUG_LOG(const char *msg, ...)
{
  va_list args;
  va_start(args, msg);
  string message;
  vsprintf(message, msg, args);
  va_end(args);
  Logger::log()->writeLog(message);
}

Logger * Logger::sLog = nullptr;

Logger::Logger() : tim(0), t(nullptr), fp(nullptr), filepath(""), message(""), lastLogTime()
{
}

static Logger * Logger::log()
{
  if (nullptr == sLog) {
    sLog = new Logger();
  }
  return sLog;
}

void Logger::writeLog(const char *msg)
{
  time(&tim);
  t = localtime(&tim);
  sprintf(message, "[%02d:%02d:%02d] %s\n", t->tm_hour, t->tm_min, t->tm_sec, msg);
  printf("\n%s", message);
  fflush(stdout);
}
