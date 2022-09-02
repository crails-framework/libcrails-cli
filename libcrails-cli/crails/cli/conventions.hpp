#pragma once
#include <string>
#include <functional>

namespace Crails
{
  struct NamingConvention
  {
    typedef std::function<std::string (const std::string&)> Transformation;

    NamingConvention();

    bool load_from_file(const std::string& path);

    Transformation filenames, classnames, functions;
  };

  extern NamingConvention naming_convention;
}
