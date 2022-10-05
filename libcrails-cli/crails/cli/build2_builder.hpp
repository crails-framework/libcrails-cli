#pragma once
#include "with_path.hpp"
#include "process.hpp"
#include "build_options.hpp"
#include <filesystem>
#include <sstream>
#include <map>

class Build2Builder : public Crails::WithPath
{
  std::filesystem::path project_directory;
  std::string package_name;
  int options;
public:
  Build2Builder(const std::string& package_name, const std::filesystem::path& project_directory, const std::filesystem::path& build_directory, int options = 0) :
    Crails::WithPath(build_directory),
    project_directory(project_directory),
    package_name(package_name),
    options(options)
  {
  }

  static bool installed()
  {
    return Crails::which("bpkg").length() > 0;
  }

  static bool create(const std::string& directory, const std::map<std::string, std::string>& options)
  {
    std::stringstream command;

    command << Crails::which("bpkg") << " create -d \"" << directory << '"';
    command << " cc ";
    for (auto it = options.begin() ; it != options.end() ; ++it)
      command << ' ' << it->first << '=' << it->second;
    return Crails::run_command(command.str());
  }

  bool configure()
  {
    return Crails::run_command(Crails::which("bpkg") + " add --type dir \"" + std::filesystem::relative(project_directory).string())
        && Crails::run_command(Crails::which("bpkg") + " fetch");
  }

  bool build()
  {
    std::stringstream command;

    command << Crails::which("bpkg") << " build " << package_name;
    if (options & BuildVerbose)
      command << " --verbose=4";
    return Crails::run_command(command.str());
  }
};
