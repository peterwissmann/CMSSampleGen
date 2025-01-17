#!/usr/bin/bash

for file in $@; do
  if [ $file != $0 ]; then
    if grep -q  "$file"; then
      echo "Replacing Windows line endings in: $file"
      sed -i 's///g' $file
    fi
  fi
done
