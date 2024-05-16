#include <crails/cli/command.hpp>
#include <crails/cli/command_index.hpp>

#undef NDEBUG
#include <cassert>

struct TestCommand : public Crails::Command
{
  int run() override { return options.count("name") ? options["name"].as<std::string>().length() : 0; }
  void options_description(boost::program_options::options_description& desc) const override
  {
    desc.add_options()
      ("name,n", boost::program_options::value<std::string>(), "test name");
  }
};

int main()
{
  // Crails::Command
  {
    TestCommand command;
    const char* argv[] = {"driver", "-n", "1234"};

    command.initialize(3, argv);
    assert(command.run() == 4);
  }

  // Crails::CommandIndex success
  {
    Crails::CommandIndex index;
    const char* argv[] = {"driver", "test", "--name", "12345"};

    index.add_command("test", []() { return std::make_shared<TestCommand>(); });
    assert(index.initialize(4, argv) == true);
    assert(index.run() == 5);
  }

  // Crails::CommandIndex not found
  {
    Crails::CommandIndex index;
    const char* argv[] = {"driver", "test", "--name", "12345"};

    index.add_command("test2", []() { return std::make_shared<TestCommand>(); });
    assert(index.initialize(4, argv) == false);
  }

  return 0;
}
