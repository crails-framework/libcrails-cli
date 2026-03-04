#pragma once
#include <boost/asio.hpp>

namespace Crails
{
  std::string drain_pipe(boost::asio::readable_pipe&);
}
