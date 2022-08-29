#pragma once
#include <filesystem>

namespace Crails
{
  class WithPath
  {
    const std::filesystem::path old_path;
  public:
    WithPath(std::filesystem::path new_path) : old_path(std::filesystem::current_path())
    {
      std::filesystem::create_directories(new_path);
      std::filesystem::current_path(new_path);
    }

    ~WithPath()
    {
      std::filesystem::current_path(old_path);
    }
  };
}
