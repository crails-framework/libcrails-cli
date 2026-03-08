#pragma once
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <ostream>
#include <filesystem>
#include <system_error>

namespace Crails
{
  struct ExecutableCommand
  {
    std::string path;
    std::vector<std::string> arguments;
    std::unordered_map<std::string,std::string> env;
    std::filesystem::path absolute_path() const;
    ExecutableCommand& operator<<(const std::string& v) { arguments.push_back(v); return *this; }
    ExecutableCommand& operator<<(const std::string_view v) { arguments.push_back(std::string(v)); return *this; }
    ExecutableCommand& operator<<(const char* v) { arguments.push_back(v); return *this; }
    static ExecutableCommand from_string(const std::string_view, std::error_code&) noexcept;
    static ExecutableCommand from_string(const std::string_view);
  };

  struct ExecutableCommandOutput
  {
    int status = -1;
    std::string out, error;
  };

  std::string which(const std::string_view command);
  bool run_command(const ExecutableCommand&);
  bool run_command(const ExecutableCommand&, std::string& result);
  bool run_command(const ExecutableCommand&, ExecutableCommandOutput&);
  bool run_command(const std::string_view command);
  bool run_command(const std::string_view command, std::string& result);
  bool run_command(const std::string_view command, ExecutableCommandOutput&);
  bool require_command(const std::string_view command);
  int  execve(const std::string& command, const std::vector<std::string>& arguments);
}

std::ostream& operator<<(std::ostream&, const Crails::ExecutableCommand&);
