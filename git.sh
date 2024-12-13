#!/bin/sh

set -e

script_dir="$(dirname "$0")"
(
  cd "$script_dir"
  cmake -B build -S .
  cmake --build ./build
)

if ! test -d test_dir; then
  mkdir test_dir
fi
cd test_dir
exec "../build/git" "$@" && cd -
