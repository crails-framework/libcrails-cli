#ifndef LIBCRAILS_CLI_BOOST_PROCESS_V1
#include "process.hpp"
#include "drain_pipe.hpp"
#include <crails/utils/split.hpp>
#include <iostream>
#include <filesystem>
#include <future>
#ifndef _WIN32
# include <unistd.h>
#else
# include <process.h>
#endif

#define BOOST_PROCESS_USE_STD_FS
#ifdef LIBCRAILS_CLI_BOOST_PROCESS_SEPARATE_COMPILATION
# define BOOST_PROCESS_V2_SEPARATE_COMPILATION
#endif
#include <boost/version.hpp>
#if BOOST_VERSION >= 108600
# include <boost/process.hpp>
# ifdef LIBCRAILS_CLI_BOOST_PROCESS_SEPARATE_COMPILATION
#  include <boost/process/src.hpp>
# endif
namespace boost_process = boost::process;
#else
# include <boost/process/v2.hpp>
# ifdef LIBCRAILS_CLI_BOOST_PROCESS_SEPARATE_COMPILATION
#  include <boost/process/v2/src.hpp>
# endif
namespace boost_process = boost::process::v2;
#endif
#include <boost/asio.hpp>

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

static boost_process::process_environment make_environment(const Crails::ExecutableCommand& command)
{
  boost_process::environment::current_view current_env = boost_process::environment::current();
  unordered_map<string, string> env(command.env);

  for (const boost_process::environment::key_value_pair& entry : current_env)
  {
    string key = entry.key().string();

    if (command.env.find(key) == command.env.end())
      env.emplace(key, entry.value().string());
  }
  return boost_process::process_environment(env);
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

  bool run_command(const string_view command, string& result)
  {
    return run_command(ExecutableCommand::from_string(command), result);
  }

  bool run_command(const string_view command, ExecutableCommandOutput& result)
  {
    return run_command(ExecutableCommand::from_string(command), result);
  }

  bool run_command(const ExecutableCommand& desc)
  {
    filesystem::path path = desc.absolute_path();

    if (filesystem::exists(path))
    {
      boost::asio::io_context ios;
      boost_process::process process(
        ios,
        path.string(),
        desc.arguments,
        make_environment(desc)
      );

      if (desc.timeout == chrono::milliseconds::zero())
      {
        process.wait();
      }
      else if (async(launch::async, [&process]() { process.wait(); }).wait_for(desc.timeout) == future_status::timeout)
      {
        cerr << "Crails::run_command: command timed out: " << desc.path << endl;
        process.terminate();
        return false;
      }
      return process.exit_code() == 0;
    }
    else
      cerr << "Crails::run_command: command not found: " << desc.path << endl;
    return false;
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
      string errors;
      boost::asio::io_context ios;
      boost::asio::readable_pipe std_out(ios), std_err(ios);
      boost_process::process process(
        ios,
        path,
        desc.arguments,
        boost_process::process_stdio{nullptr, std_out, std_err},
        make_environment(desc)
      );
      future<string> future_out = async(launch::async, &drain_pipe, ref(std_out), result.buffer_capacity);
      future<string> future_err = async(launch::async, &drain_pipe, ref(std_err), result.buffer_capacity);

      if (desc.timeout == chrono::milliseconds::zero())
      {
        process.wait();
      }
      else
      {
        future<void> future_end = async(launch::async, [&process]() { process.wait(); });

        if (future_end.wait_for(desc.timeout) == future_status::timeout)
        {
          result.timed_out = true;
          process.terminate();
        }
      }
      result.out = future_out.get();
      result.error = future_err.get();
      result.status = process.exit_code();
      return result.status == 0 && !result.timed_out;
    }
    else
      cerr << "Crails::run_command: command not found: " << desc.path << endl;
    return false;
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
