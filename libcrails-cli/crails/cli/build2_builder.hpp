#pragma once
#include "with_path.hpp"
#include "process.hpp"
#include "build_options.hpp"
#include <filesystem>
#include <sstream>
#include <map>
#include <list>
#include <iostream>

class Build2Builder : public Crails::WithPath
{
  std::filesystem::path project_directory;
  std::string package_name;
  int options;
  std::vector<std::string> system_packages;
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

  static bool create(const std::string& directory, const std::map<std::string, std::string>& options, bool verbose = false)
  {
    std::stringstream command;

    command << Crails::which("bpkg") << " create -d \"" << directory << '"';
    command << " cc ";
    for (auto it = options.begin() ; it != options.end() ; ++it)
      command << ' ' << it->first << '=' << it->second;
    if (verbose)
      std::cout << "+ " << command.str() << std::endl;
    return Crails::run_command(command.str());
  }

  bool configure()
  {
    Crails::ExecutableCommand bpkg_add{Crails::which("bpkg")};

    bpkg_add << "add" << "--type" << "dir" << std::filesystem::relative(project_directory).string();
    if (options & BuildVerbose)
      std::cout << "+ " << bpkg_add << std::endl;
    return Crails::run_command(bpkg_add)
        && Crails::run_command(Crails::which("bpkg") + " fetch")
        && build(package_name, true);
  }

  bool build()
  {
    return build(package_name);
  }

  bool build(const std::string& package_name, bool only_configure = false)
  {
    Crails::ExecutableCommand command{Crails::which("bpkg")};

    command << "build" << package_name;
    if (options & BuildVerbose)
      command << "--verbose=4";
    if (only_configure)
      command << "--configure-only";
    for (const std::string& system_package : system_packages)
      command << ("?sys:" + system_package + "/*");
    if (options & BuildVerbose)
      std::cout << "+ " << command << std::endl;
    return Crails::run_command(command);
  }

  void use_system_package(const std::string& name)
  {
    system_packages.push_back(name);
  }

  template<typename ARRAY>
  void use_system_packages(const ARRAY& names)
  {
    for (const auto& name : names)
      use_system_package(name);
  }
};
