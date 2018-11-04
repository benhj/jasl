# (c) 2017 Ben Jones JASL build

cmake_minimum_required (VERSION 2.6)
project (jasl)

# put all source code in one place for convenience
file(GLOB caching src/caching/*.cpp src/caching/*.hpp)
file(GLOB commands src/commands/*.cpp src/commands/*.hpp)
file(GLOB commands_expressions src/commands/expressions/*.cpp src/commands/expressions/*.hpp)
file(GLOB commands_fileio src/commands/fileio/*.cpp src/commands/fileio/*.hpp)
file(GLOB commands_net src/commands/net/*.cpp src/commands/net/*.hpp)
file(GLOB commands_string src/commands/string/*.cpp src/commands/string/*.hpp)
file(GLOB commands_list src/commands/list/*.cpp src/commands/list/*.hpp)
file(GLOB other src/other/*.hpp)
file(GLOB core src/core/*.cpp src/core/*.hpp)
file(GLOB simpleprompt simpleprompt/*.hpp)
file(GLOB simpletest simpletest/*.hpp)

# ensure headers in the src folder are compiler-found
include_directories(src)
include_directories(simpleprompt)
include_directories(simpletest)

# annoyingly, lbs here aren't picked up automatically
link_directories(/usr/local/lib)

# break above sub-folders into individual libraries
add_library(caching_lib ${caching})
add_library(commands_lib ${commands})
add_library(commands_expressions_lib ${commands_expressions})
add_library(commands_fileio_lib ${commands_fileio})
add_library(commands_net_lib ${commands_net})
add_library(commands_string_lib ${commands_string})
add_library(commands_list_lib ${commands_list})
add_library(core_lib ${core})

# external lib deps. Need to make this generic. dylibs are mac-specifc
find_package(Boost COMPONENTS filesystem system REQUIRED)
find_package(CURL REQUIRED)
find_library(ICUUC_LIB NAMES icuuc REQUIRED)

# make sure boost headers can be found
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CURL_INCLUDE_DIR})

# print some shit out...
message(${ICUUC_LIB})
message(${CURL_LIBRARIES})

# build the interpretor
add_executable(jasl src/bin/jasl.cpp)
set(EXT_DEPS ${Boost_LIBRARIES} ${CURL_LIBRARIES} ${ICUUC_LIB})
target_link_libraries(jasl
                      caching_lib
                      commands_lib
                      commands_expressions_lib
                      commands_fileio_lib
                      commands_net_lib
                      commands_string_lib
                      commands_list_lib
                      core_lib
                      ${EXT_DEPS})

# build test binary
add_executable(jtest src/bin/test.cpp)
target_link_libraries(jtest 
                      caching_lib 
                      commands_lib
                      commands_expressions_lib
                      commands_fileio_lib
                      commands_net_lib
                      commands_string_lib
                      commands_list_lib
                      core_lib
	                    ${EXT_DEPS})

# compile options. Lots of redundancy here. Can prob clean up.
set(COMP_FLAGS -std=c++14 -O3 -ffast-math -funroll-loops -Wno-ctor-dtor-privacy -fno-pic)
target_compile_options(caching_lib PUBLIC ${COMP_FLAGS})
target_compile_options(commands_lib PUBLIC ${COMP_FLAGS})
target_compile_options(commands_expressions_lib PUBLIC ${COMP_FLAGS})
target_compile_options(commands_fileio_lib PUBLIC ${COMP_FLAGS})
target_compile_options(commands_net_lib PUBLIC ${COMP_FLAGS})
target_compile_options(commands_string_lib PUBLIC ${COMP_FLAGS})
target_compile_options(commands_list_lib PUBLIC ${COMP_FLAGS})
target_compile_options(core_lib PUBLIC ${COMP_FLAGS})
target_compile_options(jasl PUBLIC ${COMP_FLAGS})
target_compile_options(jtest PUBLIC ${COMP_FLAGS})

