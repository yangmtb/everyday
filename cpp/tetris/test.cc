#include <iostream>
#include <queue>
#include <functional>
#include <vector>

using std::cin;
using std::cout;
using std::endl;
using std::greater;
using std::vector;
using std::priority_queue;

struct node
{
  friend bool operator<(node n1, node n2) {
    return n1.priority < n2.priority;
  }
  int priority;
  int value;
};

int main(const int argc, const char *argv[])
{
  const int len = 5;
  int i;
  int a[len] = {3,5,9,7,2};
  priority_queue<int> qi;
  for (i = 0; i < len; ++i) {
    qi.emplace(a[i]);
  }
  for (i = 0; i < len; ++i) {
    cout << qi.top() << " ";
    qi.pop();
  }
  cout << endl;
  priority_queue<int, vector<int>, greater<int>> qi2;
  for (i = 0; i < len; ++i) {
    qi2.emplace(a[i]);
  }
  for (i = 0; i < len; ++i) {
    cout << qi2.top() << " ";
    qi2.pop();
  }
  cout << endl;
  priority_queue<node> qn;
  node b[len];
	b[1].priority = 9; b[1].value = 5;
	b[3].priority = 8; b[3].value = 2;
  b[0].priority = 6; b[0].value = 1;
	b[2].priority = 2; b[2].value = 3;
	b[4].priority = 1; b[4].value = 4;
  for (i = 0; i < len; ++i) {
    qn.emplace(b[i]);
  }
  for (i = 0; i < len; ++i) {
    cout << qn.top().priority << '\t' << qn.top().value << endl;
    qn.pop();
  }
  cout << "done" << endl;
  return 0;
}
