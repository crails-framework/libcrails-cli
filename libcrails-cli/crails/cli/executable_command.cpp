#include "process.hpp"
#include <crails/utils/tokenizer.hpp>
#include <iostream>
#include <filesystem>

using namespace std;

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
  ExecutableCommand ExecutableCommand::from_string(const string_view input, error_code& ec) noexcept
  {
    ExecutableCommand result;

    result.arguments = tokenize(input, ec);
    if (!ec) [[likely]]
    {
      result.path = move(result.arguments.front());
      result.arguments.erase(result.arguments.begin());
    }
    return result;
  }

  ExecutableCommand ExecutableCommand::from_string(const string_view input)
  {
    ExecutableCommand result;

    result.arguments = tokenize(input);
    result.path = move(result.arguments.front());
    result.arguments.erase(result.arguments.begin());
    return result;
  }

  filesystem::path ExecutableCommand::absolute_path() const
  {
    if (filesystem::exists(path))
      return path;
    return which(path);
  }
}
