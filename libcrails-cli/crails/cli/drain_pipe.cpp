#include "drain_pipe.hpp"

using namespace std;

namespace Crails
{
  string drain_pipe(boost::asio::readable_pipe& pipe)
  {
    string result;
    boost::system::error_code ec;
    char buffer[4096];
    size_t n;

    do
    {
      n = pipe.read_some(boost::asio::buffer(buffer), ec);
      if (n > 0)
        result.append(buffer, n);
    } while (!ec && n > 0);
    return result;
  }
}
