#pragma once
#include <string>
#include <vector>

namespace Crails
{
  std::string which(const std::string& command);
  bool run_command(const std::string& command);
  bool run_command(const std::string& command, std::string& result);
  int  execve(const std::string& command, const std::vector<std::string>& arguments);
}
