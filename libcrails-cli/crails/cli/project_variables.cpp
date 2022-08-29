#include "project_variables.hpp"
#include "filesystem.hpp"
#include <crails/read_file.hpp>
#include <crails/utils/string.hpp>
#include <filesystem>
#include <list>
#include <sstream>

using namespace std;
using namespace Crails;

string ProjectVariables::lookup_variable_path() const
{
  auto path = filesystem::canonical(filesystem::current_path());

  while (!filesystem::exists(path.string() + '/' + filepath))
  {
    if (path == path.parent_path())
      throw runtime_error("Failed to find project configuration file `" + filepath + "`.");
    path = path.parent_path();
  }
  return path.string();
}

void ProjectVariables::initialize()
{
  string       configuration_contents;
  list<string> configuration_lines;

  Crails::read_file(filepath, configuration_contents);
  configuration_lines = Crails::split(configuration_contents, '\n');
  for (auto configuration_line : configuration_lines)
  {
    auto parts = Crails::split(configuration_line, ':', true);

    if (parts.size() >= 2)
      variables.emplace(*parts.begin(), Crails::join(++parts.begin(), parts.end(), ':'));
  }
}

void ProjectVariables::save()
{
  stringstream stream;

  for (auto it = variables.begin() ; it != variables.end() ; ++it)
    stream << it->first << ':' << it->second << '\n';
  Crails::write_file("FILE", filepath, stream.str());
}
