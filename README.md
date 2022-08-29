# libcrails-cli

A small set of tools for command-line interface project management in crails framework:
* managing commands and their options with the `Command` and `CommandIndex` objects
* checking and prompting the user when a file might get overwritten with `prompt_write_file`
* write on file only if needed and report if it was created or updated with `write_file`
