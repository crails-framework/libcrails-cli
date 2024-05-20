#include "filesystem.hpp"
#include <crails/read_file.hpp>
#include <fstream>
#include <iostream>

using namespace std;

namespace Crails
{
  bool require_folder(const string& task_name, const filesystem::path& path)
  {
    filesystem::path directory(path.parent_path());

    if (directory.string().length() > 0 && !filesystem::is_directory(directory))
    {
      error_code ec;

      filesystem::create_directories(directory, ec);
      if (ec)
      {
        cout << '[' << task_name << "] Failed to create directory " << directory.generic_string() << ": " << ec.message() << endl;
        return false;
      }
      cout << '[' << task_name << "] Created directory " << directory.generic_string() << endl;
    }
    return true;
  }

  bool prompt_write_file(const string& task_name, const filesystem::path& path)
  {
    if (filesystem::exists(path))
    {
      char overwrite = 'n';

      cout << '[' << task_name << "] `" << path.string() << "` already exists. Overwrite ? [y/n] ";
      cin >> overwrite;
      if (overwrite == 'n')
        return false;
      cout << "\33[2K\r";
    }
    cout << '[' << task_name << "] Generating `" << path.string() << '`' << endl;
    return true;
  }

  static void make_file_directory(const filesystem::path& filepath)
  {
    filesystem::path parent = filepath.parent_path();
    if (parent.string().length() > 0)
      filesystem::create_directories(parent);
  }

  bool write_file(const string& task_name, const string& filepath, const string& contents)
  {
    string original_source;
    bool   exists = Crails::read_file(filepath, original_source);

    if (contents != original_source)
    {
      make_file_directory(filesystem::path(filepath));
      ofstream file(filepath.c_str());
      if (file.is_open())
      {
        file.write(contents.c_str(), contents.length());
        file.close();
        cout << '[' << task_name << "] " << filepath << ": " << (exists ? "updated" : "generated") << endl;
        return true;
      }
      else
        cerr << "cannot write in file `" << filepath << '`' << endl;
      return false;
    }
    return true;
  }

  bool move_file(const filesystem::path& src, const filesystem::path& target)
  {
    if (filesystem::exists(src) && !filesystem::exists(target))
    {
      error_code ec;

      filesystem::rename(src, target, ec);
      if (ec)
      {
        filesystem::copy(src, target, filesystem::copy_options::recursive);
        filesystem::remove_all(src);
      }
      return true;
    }
    return false;
  }
}
