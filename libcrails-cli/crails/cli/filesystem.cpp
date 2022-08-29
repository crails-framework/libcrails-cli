#include <filsystem>
#include <iostream>
#include <crails/read_file.hpp>

using namespace std;

namespace Crails
{
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

  bool write_file(const string& task_name, const string& filepath, const string& contents)
  {
    string original_source;
    bool   exists = Crails::read_file(filepath, original_source);

    if (contents != original_source)
    {
      ofstream file(filepath.c_str());
      filesystem::create_directories(filesystem::path(filepath).parent_path());
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
}
