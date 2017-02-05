project(sample_ssh_server C)

# required cmake version
cmake_minimum_required(VERSION 2.8.5)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules
)

# add definitions
include(DefineCMakeDefaults)
include(DefinePlatformDefaults)
include(DefineCompilerFlags)
include(DefineInstallationPaths)
include(DefineOptions.cmake)
include(CPackConfig.cmake)

# config.h checks
include(ConfigureChecks.cmake)
configure_file(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

# set project directory
set(PROJECT_F ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(
	# target executable
	sample_sshd

	# project source files
	${PROJECT_F}/samplesshd-kbdint.c
)

# include headers directories
include_directories(
	${PROJECT_F}
)

# compilation flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -lssh")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -ggdb -lssh")
