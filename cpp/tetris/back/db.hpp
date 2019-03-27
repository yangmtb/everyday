#ifndef __DB_HPP__
#define __DB_HPP__

#include <mysql/mysql.h>
#include <string>
#include <vector>

using std::vector;
using std::string;

class DB
{
public:
  DB();
  virtual ~DB();
  bool Connect(string username, string password, string dbname, string hostname, int port);
  bool Exec(string sql, vector<vector<string>> &out);

private:
  MYSQL *mConn;
};

#endif//__DB_HPP__
