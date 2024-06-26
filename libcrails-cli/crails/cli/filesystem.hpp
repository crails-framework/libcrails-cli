#pragma once
#include <filesystem>
#include <crails/read_file.hpp>

using namespace std;

namespace Crails
{
  bool require_folder(const std::string& task_name, const std::filesystem::path&);
  bool prompt_write_file(const std::string& task_name, const std::filesystem::path&);
  bool write_file(const std::string& task_name, const std::string& filepath, const std::string& contents);
  bool move_file(const std::filesystem::path& src, const std::filesystem::path& target);
}
