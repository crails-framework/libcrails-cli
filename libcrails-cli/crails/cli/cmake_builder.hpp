#pragma once
#include "with_path.hpp"
#include "process.hpp"
#include "build_options.hpp"
#include <boost/process.hpp>
#include <boost/process/env.hpp>
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

  boost::process::native_environment environment()
  {
    auto env = boost::this_process::environment();
    std::string pkg_config_path = "/usr/local/lib/pkgconfig";

    env["PKG_CONFIG_PATH"] = pkg_config_path;
    return env;
  }

  bool configure()
  {
    std::string command = "cmake " + options.str() + project_directory.string();
    if (verbose) std::cout << "+ " << command << std::endl;
    boost::process::child cmake(command, environment());

    cmake.wait();
    return cmake.exit_code() == 0;
  }

  bool make()
  {
    std::string command = verbose ? "make VERBOSE=1" : "make";
    if (verbose) std::cout << "+ " << command << std::endl;
    boost::process::child make(command, environment());

    make.wait();
    return make.exit_code() == 0;
  }

  bool clean()
  {
    boost::process::child clean("make clean", environment());

    clean.wait();
    return clean.exit_code() == 0;
  }

  bool build()
  {
    return configure() && (!should_clean || clean()) && make();
  }
};
