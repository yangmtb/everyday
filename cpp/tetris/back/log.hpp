#ifndef __LOG_H__
#define __LOG_H__

#include <string>
#include <cstdio>
#include <ctime>

#define DEBUG_ENABLE
#ifdef DEBUG_ENABLE
#define Dprint(fmt, args...) fprintf(stdout, "[debug] [%s %d] "fmt"\n", __FILE__, __LINE__, ##args);
#else
#define Dprint(fmt, ...)
#endif

using std::string;

void DEBUG_LOG(const char *msg, ...);

class Logger
{
private:
  Logger();
  virtual ~Logger() {}
  void createLogFile() {};
public:
  static Logger * log();
  void writeLog(const char *msg);
private:
  static Logger *sLog;
  time_t tim;
  struct tm *t;
  FILE *fp;
  string filepath;
  string message;
  struct tm lastLogTime;
};

#endif//__LOG_H__
