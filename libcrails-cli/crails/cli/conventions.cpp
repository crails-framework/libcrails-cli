#include "conventions.hpp"
#include <crails/utils/string.hpp>
#include <crails/read_file.hpp>
#include <iostream>

using namespace Crails;
using namespace std;

namespace Crails
{
  NamingConvention naming_convention;
}

NamingConvention::NamingConvention()
{
  filenames  = std::bind(&Crails::underscore, std::placeholders::_1);
  classnames = std::bind(&Crails::camelize, std::placeholders::_1, Crails::UpperCamelcase);
  functions  = std::bind(&Crails::underscore, std::placeholders::_1);
}

static NamingConvention::Transformation* find_transformation(const string& name)
{
  if (name == "filenames")  { return &naming_convention.filenames; }
  if (name == "classnames") { return &naming_convention.classnames; }
  if (name == "functions")  { return &naming_convention.functions; }
  return nullptr;
}

static void set_convention(NamingConvention::Transformation& transformation, const string& name)
{
  if (name == "underscore") transformation = std::bind(&Crails::underscore, std::placeholders::_1);
  else if (name == "dash") transformation = std::bind(&Crails::dasherize, std::placeholders::_1);
  else if (name == "camelcase") transformation = std::bind(&Crails::camelize, std::placeholders::_1, UpperCamelcase);
  else if (name == "lower-camelcase") transformation = std::bind(&Crails::camelize, std::placeholders::_1, LowerCamelcase);
}

bool NamingConvention::load_from_file(const string& path)
{
  string contents;

  if (read_file(path, contents))
  {
    for (const string& line : Crails::split(contents, '\n'))
    {
      const auto parts = Crails::split(line, ':');

      if (parts.size() == 2)
      {
        auto* transformation = find_transformation(*parts.begin());

        if (*parts.rbegin() == "dash" && *parts.begin() != "filenames")
          cerr << "NamingConvention: dash is not acceptable for " << *parts.begin() << endl;
        else if (transformation)
          set_convention(*transformation, *parts.rbegin());
        else
          cerr << "NamingConvention: unknown category " << *parts.begin() << endl;
      }
    }
  }
  return false;
}
