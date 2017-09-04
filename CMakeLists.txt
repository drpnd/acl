cmake_minimum_required (VERSION 2.6)

project (acl)

#set (CMAKE_BUILD_TYPE Debug)
set (CMAKE_C_FLAGS_DEBUG "-O0")
#set (CMAKE_BUILD_TYPE Release)
set (CMAKE_C_FLAGS_RELEASE "-O3")

#set (CMAKE_BUILD_TYPE Debug)
#set (CMAKE_BUILD_TYPE Release)

# Define the build time
execute_process (COMMAND uname -a
  OUTPUT_VARIABLE _output OUTPUT_STRIP_TRAILING_WHITESPACE)
if(WIN32)
 execute_process (COMMAND cmd /C win_date.bat
    OUTPUT_VARIABLE BUILD_DATETIME)
else(WIN32)
  execute_process (COMMAND date "+%Y%m%d-%H%M%S"
    OUTPUT_VARIABLE BUILD_DATETIME)
endif(WIN32)
string(STRIP ${BUILD_DATETIME} BUILD_DATETIME)

execute_process (COMMAND git symbolic-ref --short HEAD
  OUTPUT_VARIABLE GIT_CURRENT_BRANCH)

add_definitions (-DBUILD_DATETIME=\"${BUILD_DATETIME}\")

include_directories(include)

#set (CMAKE_BINARY_DIR "build")
#set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "build")

set (acl_VERSION "current-${BUILD_DATETIME}")
#set (acl_VERSION_MAJOR "0")
#set (acl_VERSION_MINOR "1")
#set (acl_VERSION_PATCH "0")

# sourted list
add_executable(test_acl_sl tests/sl.c sl/sl.c)

# radix tree
add_executable(test_acl_radix tests/radix.c radix/radix.c)

# PATRICIA trie
add_executable(test_acl_patricia tests/patricia.c patricia/patricia.c)

# cross validation
add_executable(test_acl_cross tests/cross.c sl/sl.c radix/radix.c)
