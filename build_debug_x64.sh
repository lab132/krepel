#!/bin/bash

cmake -G 'Unix Makefiles' \
      -DCMAKE_OSX_ARCHITECTURES='i386;x86_64' \
      -DCMAKE_CXX_COMPILER='/usr/bin/clang++' \
      -DCMAKE_C_COMPILER='/usr/bin/clang' \
      -DSFML_BUILD_FRAMEWORKS='ON' \
      -DSFML_INSTALL_XCODE4_TEMPLATES='ON' \
      -DCMAKE_CXX_FLAGS='-stdlib=libc++' \
      -DCMAKE_C_FLAGS='-stdlib=libc++' .
