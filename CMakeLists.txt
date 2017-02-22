#required cmake version
cmake_minimum_required (VERSION 2.8.5)

#Enable C++11
set (CMAKE_CXX_STANDARD 11)

# set project directory
set(PROJECT_F ${CMAKE_CURRENT_SOURCE_DIR}/source)

### Spatch project
project (spatch)

add_executable (spatch
  ${PROJECT_F}/main.cpp
  ${PROJECT_F}/ServerController.cpp
  ${PROJECT_F}/SshProxy.cpp
  ${PROJECT_F}/SshSession.cpp
  ${PROJECT_F}/SshChannel.cpp
  ${PROJECT_F}/PseudoTerminal.cpp
)

target_link_libraries (spatch ssh)
