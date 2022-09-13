#pragma once
#include "command.hpp"
#include "scaffold_model.hpp"
#include <iostream>

namespace Crails
{
  class Scaffold : public Crails::Command
  {
    std::string type;
    std::unique_ptr<ScaffoldModel> model = nullptr;
  public:
    typedef std::map<std::string, std::function<ScaffoldModel*()>> Scaffolds;

    std::string_view description() const override { return "Generates files forming the basic structure of your project."; }

    virtual Scaffolds available_scaffolds() const = 0;

    bool initialize(int argc, const char** argv) override
    {
      auto scaffolds = available_scaffolds();

      if (argc > 1)
      {
        auto it = scaffolds.find(std::string(argv[1]));

        if (it == scaffolds.end())
        {
          std::cerr << "Unknown scaffold type " << type << std::endl;
          return false;
        }
        model.reset(it->second());
        return Command::initialize(argc - 1, &argv[1]);
      }
      else
      {
        std::cout << "Available scaffolds:" << std::endl;
        for (auto it = scaffolds.begin() ; it != scaffolds.end() ; ++it)
          std::cout << " - " << it->first << std::endl;
      }
      return false;
    }

    virtual void options_description(boost::program_options::options_description& desc) const override
    {
      if (model)
        model->options_description(desc);
    }

    int run() override
    {
      model->create(options);
      return 0;
    }
  };
}
