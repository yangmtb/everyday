#include "shape.hpp"
#include <iostream>

using std::cin;
using std::cout;
using std::endl;

void ws();

void testShape()
{
  Shape *layout = new Shape();
  Shape *a = new Shape(layout);
  char input = 'q';
  bool v = false;
  do {
    cin >> input;
    switch (input) {
    case 'a':
      a->Left();
      break;
    case 'd':
      a->Right();
      break;
    case 'w':
      a->Rotate();
      break;
    case 's':
      v = a->Down();
      if (!v) {
        a = new Shape(layout);
        if (!a->IsValid()) {
          cout << "game over" << endl;
          input = 'q';
        }
      }
      break;
    default:
      input = 'q';
    }
    //cout << "input " << input << endl;
    cout << a->GetString() << endl;
    //a->Show();
  } while ('q' != input);
  layout->Show();
}

int main(const int argc, const char *argv[])
{
  testShape();
  //ws();
  cout << "done" << endl;
  return 0;
}
