#pragma once
#include "with_path.hpp"
#include "process.hpp"
#include "build_options.hpp"
#include <filesystem>
#include <iostream>

class CMakeBuilder : private Crails::WithPath
{
  std::filesystem::path project_directory;
  std::stringstream options;
  bool verbose, should_clean;
public:
  CMakeBuilder(const std::filesystem::path& project_directory, const std::filesystem::path& build_directory, int options = 0) :
    Crails::WithPath(build_directory),
    project_directory(project_directory)
  {
    verbose = options & BuildVerbose;
    should_clean = options & BuildClean;
  }

  CMakeBuilder& option(const std::string& name, const std::string& value)
  {
    options << "\"-D" << name << '=' << value << "\" ";
    return *this;
  }

  static bool installed()
  {
    return Crails::which("cmake").length() > 0;
  }

  std::unordered_map<std::string,std::string> environment() const
  {
    return {
      {"PKG_CONFIG_PATH", "/usr/local/lib/pkgconfig"}
    };
  }

  bool configure()
  {
    std::string input = "cmake " + options.str() + project_directory.string();
    auto command = Crails::ExecutableCommand::from_string(input);

    command.env = environment();
    if (verbose) std::cout << "+ " << command << std::endl;
    return Crails::run_command(command);
  }

  bool make()
  {
    std::string input = verbose ? "make VERBOSE=1" : "make";
    auto command = Crails::ExecutableCommand::from_string(input);

    command.env = environment();
    if (verbose) std::cout << "+ " << command << std::endl;
    return Crails::run_command(command);
  }

  bool clean()
  {
    return Crails::run_command({"make", {"clean"}, environment()});
  }

  bool build()
  {
    return configure() && (!should_clean || clean()) && make();
  }
};
