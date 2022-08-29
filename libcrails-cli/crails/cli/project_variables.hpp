#pragma once
#include <map>
#include <string>

namespace Crails
{
  class ProjectVariables
  {
    std::string filepath;
  protected:
    std::map<std::string, std::string> variables;
  public:
    ProjectVariables(const std::string& filepath) : filepath(filepath) {}
    virtual ~ProjectVariables() {}

    std::string lookup_variable_path() const;
    void        initialize();
    void        save();

    std::string variable(const std::string& name) const { return variables.find(name) != variables.end() ? variables.at(name) : ""; }
    void        variable(const std::string& name, const std::string& value) { variables[name] = value; }
  };
}
