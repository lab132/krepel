#!py -3 || python3
"""
Creates a new project using krepel in a given directory.
"""

from pathlib import *
import argparse
import subprocess
import os
import stat

gitignoreTemplate = """
# Note: To check in binaries or libs, use `git add --force /bin/whatever.dll`.
/bin/
/lib/

# This is the intended place to generate CMake stuff in, i.e. the CMake binary dir.
/workspace/

# Windows thumbnails.
[tT]humbs.[dD][bB]
"""

editorconfigTemplate = """
# EditorConfig settings file.
# Check it out: http://editorconfig.org/
# There is a plugin for visual studio, sublime text 2/3, notepad++, vim, and more

# Editor config plugins will stop searching for any other .editorconfig file
# if the following is set to true
root = true

# Use unix line endings for all files
[*]
# Unix style line endings (\n)
end_of_line = lf

# Use 2 spaces for indentation
indent_style = space
indent_size = 2

# At least 1 blank line at the end of the file
insert_final_newline = true

# Use UTF-8 as encoding
charset = utf8

# Remove annoying white space at the end of a line
trim_trailing_whitespace = true
"""

toplevelCMakeListsTemplate = """
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

# Name of the entire project.
project("{name}")

find_path(KREPEL_DIR lib/krEngine.lib
          PATHS "$ENV{{KREPEL_DIR}}" "C:/Program Files/krepel" "$ENV{{PATH}}"
          DOC "Path to the krepel installation."
          NO_DEFAULT_PATH)

if(NOT EXISTS "${{KREPEL_DIR}}")
  message(FATAL_ERROR "Please specify the path to the krepel installation.")
endif()

set(CMAKE_MODULE_PATH "${{{cmake_name}_DIR}}/build/CMake" "${{KREPEL_DIR}}/build/CMake")
include(kr_setup)
kr_setup()

include(platforms/${{CMAKE_SYSTEM_NAME}})
include(targets/glew)
include(targets/ezEngine)
include(targets/krepel)


set({cmake_name}_DIR "${{CMAKE_SOURCE_DIR}}" CACHE PATH "Root directory of {name}.")
mark_as_advanced({cmake_name}_DIR)


include(kr_client_copy_dlls)
kr_client_copy_dlls("${{{cmake_name}_DIR}}/bin")

include_directories("code")
add_subdirectory("code")
"""

codeCMakeListsTemplate = """
add_subdirectory("{name}")
"""

mainprojectCMakeListsTemplate = """
include(kr_set_pch)
include(kr_mirror_source_tree)

file(GLOB_RECURSE SOURCES *.h *.inl *.cpp)
kr_set_pch("{name}/pch.h" "pch.cpp" ${{SOURCES}})
kr_mirror_source_tree("${{CMAKE_CURRENT_LIST_DIR}}" ${{SOURCES}})

add_executable({name} ${{SOURCES}})

find_package(OpenGL REQUIRED)
if(OPENGL_INCLUDE_DIR)
  # Needed on non-windows platforms (I think).
  target_include_directories({name} PUBLIC ${{OPENGL_INCLUDE_DIR}})
endif()

target_link_libraries({name}
                      krEngine
                      ${{OPENGL_LIBRARIES}})
"""

mainStubCpp = """#include <{name}/pch.h>
#include <cstdio>

int main(int argc, char* argv[])
{{
  std::printf("Hello {name}!\\n");
  return 0;
}}
"""
pchH = """#pragma once

// krepel - krEngine
#include <krEngine/pch.h>
#include <krEngine/ownership.h>
#include <krEngine/rendering.h>
"""

pchCpp = "#include <{name}/pch.h>\n"

def get_args():
  """Get arguments to this script."""
  parser = argparse.ArgumentParser(description="Create a new project using krepel.")
  parser.add_argument("dest_dir",
                      type=Path,
                      help=("The directory to create the project in. "
                            "Will prompt for confirmation if the directory already exists."))
  parser.add_argument("--name",
                      help="A friendly name for the project.")
  parser.add_argument("--force",
                      action="store_true",
                      help="If set, always assume 'yes' for prompts.")
  return parser.parse_args()

def prompt_user_bool(question, *, max_attempts=3):
  """Prompt the user with a yes/no question."""
  for _ in range(max_attempts):
    prompt = input(question + " [y/n] ").strip().lower()
    if prompt:
      if prompt[0] == "n":
        return False
      if prompt[0] == "y":
        return True
  print("Could not determine what you want after {} attempts. "
        "Assuming 'no' is your answer.".format(max_attempts))
  return False

def rm_tree(top):
  """Equivalent to `rm -rf <top>`."""
  for root, dirs, files in os.walk(top.as_posix(), topdown=False):
    for name in files:
      filename = os.path.join(root, name)
      os.chmod(filename, stat.S_IWUSR)
      os.remove(filename)
    for name in dirs:
      os.rmdir(os.path.join(root, name))
  os.rmdir(top.as_posix())

def create_editorconfig_file():
  """Create the .editorconfig file"""
  with Path(".editorconfig").open("w") as outfile:
    outfile.write(editorconfigTemplate)

def create_toplevel_cmakelists_file(friendly_name, cmake_name):
  """Create the top-level CMakeLists.txt file"""
  with Path("CMakeLists.txt").open("w") as outfile:
    outfile.write(toplevelCMakeListsTemplate.format(name=friendly_name, cmake_name=cmake_name.upper()))

def create_code_and_mainproject_cmakelists_file(cmake_name):
  """Create the CMakeLists.txt files in :/code and :/code/<cmake_name>"""
  code_dir = Path("code")
  code_dir.mkdir(parents=True)
  with (code_dir / "CMakeLists.txt").open("w") as outfile:
    outfile.write(codeCMakeListsTemplate.format(name=cmake_name))
  project_dir = code_dir / cmake_name
  project_dir.mkdir(parents=True)
  with (project_dir / "CMakeLists.txt").open("w") as outfile:
    outfile.write(mainprojectCMakeListsTemplate.format(name=cmake_name))
  with (project_dir / "main.cpp").open("w") as outfile:
    outfile.write(mainStubCpp.format(name=cmake_name))
  with (project_dir / "pch.h").open("w") as outfile:
    outfile.write(pchH.format(name=cmake_name))
  with (project_dir / "pch.cpp").open("w") as outfile:
    outfile.write(pchCpp.format(name=cmake_name))

def create_gitignore_file():
  """Create the .gitignore file"""
  with Path(".gitignore").open("w") as outfile:
    outfile.write(gitignoreTemplate)

def main():
  """Entry point."""
  args = get_args()
  dest = args.dest_dir
  # Replace all whitespaces with _: Hello World => Hello_World
  friendly_name = args.name or dest.name
  cmake_name = "_".join(friendly_name.split())
  print("Creating project {}".format(friendly_name))
  if dest.is_file():
    print("Destination dir already exists and is a file! Aborting.")
    return
  if dest.exists():
    if not args.force and not prompt_user_bool("Destination already exists. Overwrite?"):
      return
    rm_tree(dest)
  dest.mkdir(parents=True)
  dest = dest.resolve()
  os.chdir(dest.as_posix())

  # .editorconfig
  create_editorconfig_file()

  # CMakeLists.txt
  create_toplevel_cmakelists_file(friendly_name, cmake_name)
  create_code_and_mainproject_cmakelists_file(cmake_name)

  # .gitignore
  create_gitignore_file()

  print("Your project has been created.")
  print("Please use CMake to generate a build system for your new project.")
  print("Full path: {}".format(dest.as_posix()))

if __name__ == "__main__":
  main()
