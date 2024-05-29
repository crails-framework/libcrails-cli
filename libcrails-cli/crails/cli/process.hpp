#pragma once
#include <string>
#include <string_view>
#include <vector>
#include <ostream>
#include <filesystem>

namespace Crails
{
  struct ExecutableCommand
  {
    std::string path;
    std::vector<std::string> arguments;
    std::filesystem::path absolute_path() const;
    ExecutableCommand& operator<<(const std::string& v) { arguments.push_back(v); return *this; }
    ExecutableCommand& operator<<(const std::string_view v) { arguments.push_back(std::string(v)); return *this; }
    ExecutableCommand& operator<<(const char* v) { arguments.push_back(v); return *this; }
  };

  std::string which(const std::string& command);
  bool run_command(const std::string& command);
  bool run_command(const std::string& command, std::string& result);
  bool run_command(const ExecutableCommand&);
  bool run_command(const ExecutableCommand&, std::string& result);
  bool require_command(const std::string& command);
  int  execve(const std::string& command, const std::vector<std::string>& arguments);
}

std::ostream& operator<<(std::ostream&, const Crails::ExecutableCommand&);
