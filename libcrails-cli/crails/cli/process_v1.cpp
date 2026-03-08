#ifdef LIBCRAILS_CLI_BOOST_PROCESS_V1
#include "process.hpp"
#include <crails/utils/split.hpp>
#include <boost/process.hpp>
#include <boost/version.hpp>
#if BOOST_VERSION >= 108600 // 1.86
# include <boost/process/v1/env.hpp>
#else
# include <boost/process/env.hpp>
#endif
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
#ifdef _WIN32
  bool regular_file = filesystem::is_regular_file(path.string(), ec);
  return !ec && regular_file;
#else
  bool regular_file = filesystem::is_regular_file(path, ec);
  return !ec && regular_file && ::access(path.string().c_str(), X_OK) == 0;
#endif
}

static boost::process::native_environment make_environment(const Crails::ExecutableCommand& command)
{
  auto env = boost::this_process::environment();

  for (auto it = command.env.begin() ; it != command.env.end() ; ++it)
    env[it->first.c_str()] = it->second;
  return env;
}

struct ArgvArray
{
  const char** data;
  ArgvArray(const std::vector<std::string>& argv_array) { data = new const char*[argv_array.size() + 1]; }
  ~ArgvArray() { delete[] data; }
};

namespace Crails
{
  string which(const string_view command)
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
        if (is_executable_path(candidate_path))
          return candidate_path.string();
      }
#else
      if (is_executable_path(path))
        return path.string();
#endif
    }
    return string();
  }

  bool require_command(const string_view command)
  {
    if (Crails::which(command).length() == 0)
    {
      cerr << "Requires `" << command << "` to be installed." << endl;
      return false;
    }
    return true;
  }

  bool run_command(const string_view command)
  {
    return run_command(ExecutableCommand::from_string(command));
  }

  bool run_command(const ExecutableCommand& desc)
  {
    filesystem::path path = desc.absolute_path();

    if (filesystem::exists(path))
    {
      auto env = boost::this_process::environment();
      boost::process::child process(
        path.string(),
        boost::process::args(desc.arguments),
        make_environment(desc)
      );

      process.wait();
      return process.exit_code() == 0;
    }
    else
      cerr << "Crails::run_command: command not found: " << desc.path << endl;
    return false;
  }

  bool run_command(const string_view command, string& result)
  {
    return run_command(ExecutableCommand::from_string(command), result);
  }

  bool run_command(const ExecutableCommand& desc, string& std_out)
  {
    ExecutableCommandOutput result;
    bool retval = run_command(desc, result);

    std_out = std::move(result.out);
    if (result.error.length())
      cerr << desc.absolute_path() << ": " << result.error << endl;
    return retval;
  }

  bool run_command(const ExecutableCommand& desc, ExecutableCommandOutput& result)
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
        make_environment(desc),
        ios
      );

      process.detach();
      process.wait();
      ios.run();
      result.out = std_out.get();
      result.error = std_err.get();
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
#endif
