#pragma once
#include <string>
#include <string_view>

namespace Crails
{
  std::string prompt(const std::string_view message, const std::string_view prefix = "$> ");
}
