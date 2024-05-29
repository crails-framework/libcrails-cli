#include "process.hpp"
#include <crails/utils/split.hpp>
#include <boost/process.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <filesystem>
#ifndef _WIN32
# include <unistd.h>
#else
# include <process.h>
#endif

using namespace std;

static bool is_executable_path(const filesystem::path& path)
{
  error_code ec;
  bool regular_file = filesystem::is_regular_file(path, ec);
#ifdef _WIN32
  return !ec && regular_file;
#else
  return !ec && regular_file && ::access(path.string().c_str(), X_OK) == 0;
#endif
}

ostream& operator<<(ostream& stream, const Crails::ExecutableCommand& command)
{
  stream << command.path;
  for (const string& argv : command.arguments)
  {
    if (argv.find(' ') == string::npos)
      stream << ' ' << argv;
    else
      stream << ' ' << quoted(argv);
  }
  return stream;
}

namespace Crails
{
  string which(const string& command)
  {
    const char* env_path = getenv("PATH");
    string current_path = filesystem::current_path().string();
    list<string_view> candidates;

    if (env_path)
      candidates = Crails::split<string_view>(env_path, ':');
    candidates.insert(candidates.begin(), string_view(current_path));
    for (const string_view part : candidates)
    {
      filesystem::path path = filesystem::path(part) / command;

#ifdef _WIN32
      vector<filesystem::path> candidate_paths{path + ".exe", path + ".bat", path};

      for (const filesystem::path& candidate_path)
      {
        if (is_executable_path(path))
          return filesystem::canonical(path);
      }
#else
      if (is_executable_path(path))
        return filesystem::canonical(path);
#endif
    }
    return string();
  }

  bool require_command(const string& command)
  {
    if (Crails::which(command).length() == 0)
    {
      cerr << "Requires `" << command << "` to be installed." << endl;
      return false;
    }
    return true;
  }

  bool run_command(const string& command)
  {
    boost::process::child process(command);

    process.wait();
    return process.exit_code() == 0;
  }

  bool run_command(const ExecutableCommand& desc)
  {
    boost::process::child process(
      desc.path,
      boost::process::args(desc.arguments)
    );

    process.wait();
    return process.exit_code() == 0;
  }

  bool run_command(const string& command, string& result)
  {
    future<string> std_out;
    boost::asio::io_context ios;
    boost::process::child process(
      command,
      boost::process::std_in.close(),
      boost::process::std_out > std_out,
      ios
    );

    process.detach();
    process.wait();
    ios.run();
    result = std_out.get();
    return process.exit_code() == 0;
  }

  bool run_command(const ExecutableCommand& desc, string& result)
  {
    future<string> std_out;
    boost::asio::io_context ios;
    boost::process::child process(
      desc.path,
      boost::process::args(desc.arguments),
      boost::process::std_in.close(),
      boost::process::std_out > std_out,
      ios
    );

    process.detach();
    process.wait();
    ios.run();
    result = std_out.get();
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
