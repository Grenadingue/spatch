set (CMAKE_CXX_STANDARD 11)

#required cmake version
cmake_minimum_required (VERSION 2.8.5)

# set project directory
set(PROJECT_F ${CMAKE_CURRENT_SOURCE_DIR})

### Spatch project
project (spatch)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH
  ${PROJECT_F}/libssh_examples/sample-ssh-server/cmake/Modules
)

add_executable (spatch ${PROJECT_F}/source/main.cpp)
target_link_libraries (spatch ssh)
