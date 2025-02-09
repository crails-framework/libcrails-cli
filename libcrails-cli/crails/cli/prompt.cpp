#include "prompt.hpp"
#include <iostream>

using namespace std;

namespace Crails
{
  string prompt(const string_view message, const string_view prefix)
  {
    string result;

    cout << message << endl << prefix;
    cin >> result;
    return result;
  }
}
