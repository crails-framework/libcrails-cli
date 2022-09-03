#pragma once
#include "with_path.hpp"
#include "process.hpp"
#include "build_options.hpp"
#include <boost/process.hpp>
#include <filesystem>

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
    options << "-D" << name << '=' << value << ' ';
    return *this;
  }

  static bool installed()
  {
    return Crails::which("cmake").length() > 0;
  }

  bool configure()
  {
    boost::process::child cmake("cmake " + options.str() + project_directory.string());

    cmake.wait();
    return cmake.exit_code() == 0;
  }

  bool make()
  {
    boost::process::child make(verbose ? "make VERBOSE=1" : "make");

    make.wait();
    return make.exit_code() == 0;
  }

  bool clean()
  {
    boost::process::child clean("make clean");

    clean.wait();
    return clean.exit_code() == 0;
  }

  bool build()
  {
    return configure() && (!should_clean || clean()) && make();
  }
};
