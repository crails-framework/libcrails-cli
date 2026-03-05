#pragma once
#include <boost/version.hpp>
#if BOOST_VERSION >= 107800 // 1.86
# include <boost/asio.hpp>

namespace Crails
{
  std::string drain_pipe(boost::asio::readable_pipe&);
}

#else
# error "include <crails/cli/drain_pipe.hpp> requires boost 1.78 or above"
#endif
