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

  bool run_command(const string& command, string& result)
  {
    boost::process::ipstream stream;
    boost::process::child process(command, boost::process::std_out > stream);
    string line;

    while (process.running() && getline(stream, line) && !line.empty())
      result += line + '\n';
    process.wait();
    return process.exit_code() == 0;
  }

  int execve(const string& command, const vector<string>& arguments)
  {
    if (command.length() > 0)
    {
      const char* argv[arguments.size() + 1];

      argv[0] = command.c_str();
      for (size_t i = 1 ; i <= arguments.size() ; ++i)
        argv[i] = arguments[i - 1].c_str();
      argv[arguments.size() + 1] = nullptr;
#ifndef _WIN32
      return ::execve(command.c_str(), const_cast<char **const>(argv), nullptr);
#else
      return _execve(command.c_str(), const_cast<char **const>(argv), nullptr);
#endif
    }
    return 0;
  }
}
