# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/spectradlaw/Desktop/Repositorys/Trafsim/trafsim-master/_deps/imgui-src"
  "/home/spectradlaw/Desktop/Repositorys/Trafsim/trafsim-master/_deps/imgui-build"
  "/home/spectradlaw/Desktop/Repositorys/Trafsim/trafsim-master/_deps/imgui-subbuild/imgui-populate-prefix"
  "/home/spectradlaw/Desktop/Repositorys/Trafsim/trafsim-master/_deps/imgui-subbuild/imgui-populate-prefix/tmp"
  "/home/spectradlaw/Desktop/Repositorys/Trafsim/trafsim-master/_deps/imgui-subbuild/imgui-populate-prefix/src/imgui-populate-stamp"
  "/home/spectradlaw/Desktop/Repositorys/Trafsim/trafsim-master/_deps/imgui-subbuild/imgui-populate-prefix/src"
  "/home/spectradlaw/Desktop/Repositorys/Trafsim/trafsim-master/_deps/imgui-subbuild/imgui-populate-prefix/src/imgui-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/spectradlaw/Desktop/Repositorys/Trafsim/trafsim-master/_deps/imgui-subbuild/imgui-populate-prefix/src/imgui-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/spectradlaw/Desktop/Repositorys/Trafsim/trafsim-master/_deps/imgui-subbuild/imgui-populate-prefix/src/imgui-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
