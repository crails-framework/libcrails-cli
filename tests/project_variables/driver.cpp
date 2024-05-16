#include <crails/cli/project_variables.hpp>

#undef NDEBUG
#include <cassert>

static const std::string test_input =
  "testvar:42\n"
  "longvar:supposedly:joined:parts\n"
  "lastvar:1";

int main()
{
  {
    Crails::ProjectVariables vars("");
    std::string output;

    vars.load(test_input);
    assert(vars.variable("testvar") == "42");
    assert(vars.variable("longvar") == "supposedly:joined:parts");
    assert(vars.variable("lastvar") == "1");
    assert(vars.variable("nope") == "");
    assert(vars.variable_or("nope", "toto") == "toto");
    assert(vars.variable_or("testvar", "toto") == "42");
    vars.variable("nope", "yes");
    assert(vars.variable_or("nope", "toto") == "yes");
    vars.append_to_string(output);
    assert(output.find("\nnope:yes\n") != std::string::npos);
  }

  return 0;
}
