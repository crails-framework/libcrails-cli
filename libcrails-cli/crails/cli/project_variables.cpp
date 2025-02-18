#include "project_variables.hpp"
#include "filesystem.hpp"
#include <crails/read_file.hpp>
#include <crails/utils/string.hpp>
#include <crails/utils/semantics.hpp>
#include <filesystem>
#include <list>
#include <sstream>

using namespace std;
using namespace Crails;

string ProjectVariables::lookup_variable_path() const
{
  auto path = filesystem::canonical(filesystem::current_path());

  while (!filesystem::exists(path.string() + '/' + filename))
  {
    if (path == path.parent_path())
      throw runtime_error("Failed to find project configuration file `" + filename + "`.");
    path = path.parent_path();
  }
  return path.string();
}

void ProjectVariables::initialize()
{
  string configuration_contents;

  if (filesystem::exists(filename))
    filepath = filesystem::canonical(filename).string();
  else
    filepath = (filesystem::path(lookup_variable_path()) / filepath).string();
  Crails::read_file(filepath, configuration_contents);
  load(configuration_contents);
}

void ProjectVariables::load(const std::string& configuration_contents)
{
  list<string> configuration_lines;

  configuration_lines = Crails::split(configuration_contents, '\n');
  for (auto configuration_line : configuration_lines)
  {
    auto stripped_line = Crails::strip(configuration_line, '\r');
    auto parts = Crails::split(stripped_line, ':', true);

    if (parts.size() >= 2)
      variables.emplace(*parts.begin(), Crails::join(++parts.begin(), parts.end(), ':'));
  }
}

void ProjectVariables::append_to_string(string& output)
{
  for (auto it = variables.begin() ; it != variables.end() ; ++it)
    output += it->first + ':' + it->second + '\n';
}

void ProjectVariables::save()
{
  string output;

  append_to_string(output);
  Crails::write_file("FILE", filepath.length() ? filepath : filename, output);
}
