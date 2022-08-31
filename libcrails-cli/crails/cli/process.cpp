#include "process.hpp"
#include <boost/process.hpp>
#ifndef _WIN32
# include <unistd.h>
#else
# include <process.h>
#endif

using namespace std;

namespace Crails
{
  string which(const string& command)
  {
    boost::process::ipstream stream;
    boost::process::child process("which " + command, boost::process::std_out > stream);
    string path;

    process.wait();
    if (process.exit_code() == 0)
      getline(stream, path);
    return path;
  }

  bool run_command(const string& command)
  {
    boost::process::child process(command);

    process.wait();
    return process.exit_code() == 0;
  }

  int execve(const string& command, const vector<string>& arguments)
  {
    if (command.length() > 0)
    {
      const char* argv[arguments.size() + 1];

      for (size_t i = 0 ; i < arguments.size() ; ++i)
        argv[i] = arguments[i].c_str();
      argv[arguments.size()] = nullptr;
#ifndef _WIN32
      return ::execve(command.c_str(), const_cast<char **const>(argv), nullptr);
#else
      return _execve(command.c_str(), const_cast<char **const>(argv), nullptr);
#endif
    }
    return 0;
  }
}
