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

struct ArgvArray
{
  const char** data;
  ArgvArray(const std::vector<std::string>& argv_array) { data = new const char*[argv_array.size() + 1]; }
  ~ArgvArray() { delete[] data; }
};

ostream& operator<<(ostream& stream, const Crails::ExecutableCommand& command)
{
  filesystem::path final_path = command.absolute_path();

  if (filesystem::exists(final_path))
    stream << final_path;
  else
    stream << command.path << "[not-found]";
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
  filesystem::path ExecutableCommand::absolute_path() const
  {
    if (filesystem::exists(path))
      return path;
    return which(path);
  }

  string which(const string& command)
  {
    const char* env_path = getenv("PATH");
    string current_path = filesystem::current_path().string();
    list<string_view> candidates;
#ifdef _WIN32
    char env_separator = ';';
#else
    char env_separator = ':';
#endif

    if (env_path)
      candidates = Crails::split<string_view>(env_path, env_separator);
    candidates.insert(candidates.begin(), string_view(current_path));
    for (const string_view part : candidates)
    {
      filesystem::path path = filesystem::path(part) / command;

#ifdef _WIN32
      vector<filesystem::path> candidate_paths{
        filesystem::path(path.string() + ".exe"),
        filesystem::path(path.string() + ".bat"),
        path
      };

      for (const filesystem::path& candidate_path : candidate_paths)
      {
        if (is_executable_path(path))
          return path.string();
      }
#else
      if (is_executable_path(path))
        return path.string();
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
    filesystem::path path = desc.absolute_path();

    if (filesystem::exists(path))
    {
      boost::process::child process(
        path.string(),
        boost::process::args(desc.arguments)
      );

      process.wait();
      return process.exit_code() == 0;
    }
    else
      cerr << "Crails::run_command: command not found: " << desc.path << endl;
    return false;
  }

  bool run_command(const string& command, string& result)
  {
    future<string> std_out, std_err;
    string errors;
    boost::asio::io_context ios;
    boost::process::child process(
      command,
      boost::process::std_in.close(),
      boost::process::std_out > std_out,
      boost::process::std_err > std_err,
      ios
    );

    process.detach();
    process.wait();
    ios.run();
    result = std_out.get();
    errors = std_err.get();
    if (errors.length())
      cerr << errors << endl;
    return process.exit_code() == 0;
  }

  bool run_command(const ExecutableCommand& desc, string& result)
  {
    filesystem::path path = desc.absolute_path();

    if (filesystem::exists(path))
    {
      future<string> std_out, std_err;
      string errors;
      boost::asio::io_context ios;
      boost::process::child process(
        path.string(),
        boost::process::args(desc.arguments),
        boost::process::std_in.close(),
        boost::process::std_out > std_out,
        boost::process::std_err > std_err,
        ios
      );

      process.detach();
      process.wait();
      ios.run();
      result = std_out.get();
      errors = std_err.get();
      if (errors.length())
        cerr << path << ": " << errors << endl;
      return process.exit_code() == 0;
    }
    else
      cerr << "Crails::run_command: command not found: " << desc.path << endl;
    return -1;
  }

  int execve(const string& command, const vector<string>& arguments)
  {
    if (command.length() > 0)
    {
      ArgvArray argv(arguments);

      argv.data[0] = command.c_str();
      for (size_t i = 1 ; i <= arguments.size() ; ++i)
        argv.data[i] = arguments[i - 1].c_str();
      argv.data[arguments.size() + 1] = nullptr;
#ifndef _WIN32
      return ::execve(command.c_str(), const_cast<char **const>(argv.data), nullptr);
#else
      return _execve(command.c_str(), const_cast<char **const>(argv.data), nullptr);
#endif
    }
    return 0;
  }
}
