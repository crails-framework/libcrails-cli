#pragma once
#include <crails/cli/filesystem.hpp>
#include <crails/shared_vars.hpp>
#include <crails/render_target.hpp>
#include <filesystem>
#include <iostream>

namespace Crails
{
  template<class RENDERER>
  class FileRendererBase
  {
  public:
    RENDERER           renderer;
    Crails::SharedVars vars;
    bool               should_overwrite = false;

    bool generate_file(std::string_view template_name) { return generate_file(template_name, template_name); }

    bool generate_file(std::string_view template_name, std::string_view filepath)
    {
      std::filesystem::path path(filepath.data());

      return (should_overwrite || Crails::prompt_write_file("FILE", path.string()))
          && render_file(template_name, path);
    }

  private:
    bool render_file(string_view template_name, std::filesystem::path path)
    {
      Crails::RenderString render_target;

      if (renderer.can_render(template_name.data()))
      {
        if (Crails::require_folder("DIR", path.string()))
        {
          renderer.render_template(template_name.data(), render_target, vars);
          Crails::write_file("FILE", path.string(), string(render_target.value()));
          return true;
        }
      }
      else
        cerr << "[FILE] `" << path.string() << "` could not be generated: template not found." << endl;
      return false;
    }
  };
}
