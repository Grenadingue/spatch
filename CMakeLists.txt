#required cmake version
cmake_minimum_required (VERSION 2.8.5)

#Enable C++11
set (CMAKE_CXX_STANDARD 11)

# set project directory
set(PROJECT_F ${CMAKE_CURRENT_SOURCE_DIR}/source)

### Spatch project
project (spatch)

add_executable (spatch ${PROJECT_F}/main.cpp)
target_link_libraries (spatch ssh argp)
