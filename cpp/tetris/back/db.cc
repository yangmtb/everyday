#include "db.hpp"
#include <iostream>

using std::cerr;
using std::cout;
using std::endl;

DB::DB()
{
  mConn = mysql_init(nullptr);
}

DB::~DB()
{
  if (mConn) {
    mysql_close(mConn);
  }
}

bool DB::Connect(string username, string password, string dbname, string hostname, int port)
{
  if (!mysql_real_connect(mConn, hostname.c_str(), username.c_str(), password.c_str(), dbname.c_str(), port, nullptr, 0)) {
    cerr << mysql_error(mConn) << endl;
    return false;
  }
  return true;
}

bool DB::Exec(string sql, vector<vector<string>> &out)
{
  if (mysql_query(mConn, sql.c_str())) {
    cerr << mysql_error(mConn) << endl;
    return false;
  }
  MYSQL_RES *res = mysql_use_result(mConn);
  MYSQL_ROW row;
  unsigned int num_fields = mysql_num_fields(res);
  while ((row = mysql_fetch_row(res)) != nullptr) {
    unsigned long *lengths = mysql_fetch_lengths(res);
    vector<string> tmp;
    for (int i = 0; i < num_fields; ++i) {
      //printf("[%.*s] ", (int)lengths[i], row[i]?row[i]:"null");
      //cout << row[i][(int)lengths[i]-1] << " ";
      string x(row[i]);
      tmp.emplace_back(x);
    }
    out.emplace_back(tmp);
    //cout << endl;
  }
  mysql_free_result(res);
  return true;
}
