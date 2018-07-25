#include <csignal>
#include <iostream>
#include <unistd.h>
#include <thread>
#include <atomic>
#include <vector>

using std::atomic;
using std::cin;
using std::cout;
using std::endl;
using std::thread;
using std::vector;

atomic<unsigned long long> gcount(0);

/* 构造完成标志 */
//bool sign = false;

/* 创建数独矩阵 */
////int num[9][9];

/* 函数声明 */
void Output(int num[9][9]);
bool Check(int num[9][9], int n, int key);
int DFS(int num[9][9], int n);
void sigHandler(int signum)
{
  cout << "\nyes: " << gcount << endl;
  exit(signum);
}

void deal (int i) {
  //  signal(SIGINT, [i](int signum) {
  //               cout << i << " boom" << signum << endl;
  //             });
  signal(SIGINT, sigHandler);
  int num[9][9] = {0};
  num[0][0] = i;
  cout << "###" << i << " begin" << endl;
  DFS(num, 1);
  cout << "###" << i << " done" << endl;
  Output(num);
  num[0][0] = i+4;
  cout << "###" << i+4 << " begin" << endl;
  DFS(num, 1);
  cout << "###" << i+4 << " done" << endl;
  Output(num);
}

/* 主函数 */
int main()
{
  //signal(SIGINT, sigHandler);
  //  signal(SIGINT, [](int signum) {
  //                   cout << "boomm " << signum << endl;
  //                   exit(signum);
  //                 });
  vector<thread> ths;
  for (int i = 1; i < 5; ++i) {
    ths.push_back(thread(deal, i));
  }
  for (auto &x : ths) {
    x.join();
  }
  cout << "all done" << endl;
  return 0;
  //register unsigned long long gnum = 0;
  //cout << "请输入一个9*9的数独矩阵，空位以0表示:" << endl;
  //Input();
  //DFS(0, gnum);
  //Output();
  // cout << "all:" << gnum << endl;
    //system("pause");
    return 0;
}
 
 
/* 输出数独矩阵 */
void Output(int num[9][9])
{
    cout << endl;
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            cout << num[i][j] << " ";
            if (j % 3 == 2)
            {
                cout << "   ";
            }
        }
        cout << endl;
        if (i % 3 == 2)
        {
            cout << endl;
        }
    }
}
 
/* 判断key填入n时是否满足条件 */
bool Check(int num[9][9], int n, int key)
{
    int row = n / 9;
    int col = n % 9;
    for (int i = 0; i < 9; i++)
    {
        if (num[row][i] == key) return false;
        if (num[i][col] == key) return false;
    }

    /* x为n所在的小九宫格左顶点竖坐标 */
    int x = n / 9 / 3 * 3;
    /* y为n所在的小九宫格左顶点横坐标 */
    int y = n % 9 / 3 * 3;
    /* 判断n所在的小九宫格是否合法 */
    for (int i = x; i < x + 3; i++)
    {
        for (int j = y; j < y + 3; j++)
        {
            if (num[i][j] == key) return false;
        }
    }

    if (8 == row+col) {
      for (int i = 0; i < 9; ++i) {
        if (key == num[i][8-i]) return false;
      }
    }
    if (row == col) {
      for (int i = 0; i < 9; ++i) {
        if (key == num[i][i]) return false;
      }
    }
    /* 全部合法，返回正确 */
    return true;
}
 
/* 深搜构造数独 */
int DFS(int num[9][9], int n)
{
    /* 所有的都符合，退出递归 */
    if (n > 80)
    {
      //        sign = true;
        gcount++;
        return 0;
    }
    /* 当前位不为空时跳过 */
    if (num[n/9][n%9] != 0)
    {
      DFS(num, n+1);
    }
    else
    {
        /* 否则对当前位进行枚举测试 */
        for (int i = 1; i <= 9; i++)
        {
            /* 满足条件时填入数字 */
          if (Check(num, n, i) == true)
            {
                num[n/9][n%9] = i;
                /* 继续搜索 */
                DFS(num, n+1);
                /* 返回时如果构造成功，则直接退出 */
                //                if (sign == true) return 0;
                /* 如果构造不成功，还原当前位 */
                num[n/9][n%9] = 0;
            }
        }
    }
    return 1;
}
