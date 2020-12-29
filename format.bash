#!/usr/bin/env bash

# src/ test/ 以下のファイルに clang-format を適用する

set -e

shopt -s globstar

for file in example/**/*.[ch]pp include/**/*.hpp src/**/*.[ch]pp test/**/*.[ch]pp; do
  echo applying "$file"
  clang-format -i -style=file "$file"
done
