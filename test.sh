#!/bin/sh
set -e # Exit early if any commands fail

script_dir="$(dirname "$0")"
(
  cd "$script_dir" # Ensure compile steps are run within the repository directory
  cmake -B build -S .
  cmake --build ./build
)

(
  if ! test -d test_dir; then
    mkdir test_dir
  fi
  cd test_dir
  "../build/git" "$@"
)
