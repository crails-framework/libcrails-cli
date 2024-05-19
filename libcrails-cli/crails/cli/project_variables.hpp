#pragma once
#include <map>
#include <string>

namespace Crails
{
  class ProjectVariables
  {
    std::string filename;
    std::string filepath;
  protected:
    std::map<std::string, std::string> variables;
  public:
    ProjectVariables(const std::string& filename) : filename(filename) {}
    virtual ~ProjectVariables() {}

    std::string lookup_variable_path() const;
    
    void        initialize();
    void        load(const std::string&);
    void        save();
    void        append_to_string(std::string&);

    bool        has_variable(const std::string& name) const { return variables.find(name) != variables.end(); }
    std::string variable_or(const std::string& name, const std::string& fallback) const { return has_variable(name) ? variables.at(name) : fallback; }
    std::string variable(const std::string& name) const { return variable_or(name, ""); }
    void        variable(const std::string& name, const std::string& value) { variables[name] = value; }
  };
}
