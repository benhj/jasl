# (c) 2017 Ben Jones JASL build

cmake_minimum_required (VERSION 2.6)
project (jasl)

# put all source code in one place for convenience
file(GLOB_RECURSE caching src/caching/*.cpp src/caching/*.hpp)
file(GLOB_RECURSE commands src/commands/*.cpp src/commands/*.hpp)
file(GLOB_RECURSE other src/other/*.cpp src/other/*.hpp)

# ensure headers in the src folder are compiler-found
include_directories(src)

# annoyingly, lbs here aren't picked up automatically
link_directories(/usr/local/lib)

# break above sub-folders into individual libraries
add_library(caching_lib ${caching})
add_library(commands_lib ${commands})
add_library(other_lib ${other})

# external lib deps. Need to make this generic. dylibs are mac-specifc
find_package(Boost COMPONENTS filesystem system REQUIRED)
find_library(SSL_LIB libssl.dylib REQUIRED)
find_library(CRYPTO_LIB libcrypto.dylib REQUIRED)
find_library(ICUUC_LIB libicuuc.dylib REQUIRED)

# print some shit out...
message(${SSL_LIB})
message(${CRYPTO_LIB})
message(${ICUUC_LIB})

# build the interpretor
add_executable(jasl src/bin/jasl.cpp)
set(EXT_DEPS ${Boost_LIBRARIES} ${SSL_LIB} ${CRYPTO_LIB} ${ICUUC_LIB})
target_link_libraries(jasl caching_lib commands_lib other_lib ${EXT_DEPS})

# build test binary
add_executable(jtest src/bin/test.cpp)
target_link_libraries(jtest caching_lib commands_lib other_lib ${EXT_DEPS})

# compile options. Lots of redundancy here. Can prob clean up.
set(COMP_FLAGS -std=c++14 -O3 -ffast-math -funroll-loops -Wno-ctor-dtor-privacy -fno-pic)
target_compile_options(caching_lib PUBLIC ${COMP_FLAGS})
target_compile_options(commands_lib PUBLIC ${COMP_FLAGS})
target_compile_options(other_lib PUBLIC ${COMP_FLAGS})
target_compile_options(jasl PUBLIC ${COMP_FLAGS})
target_compile_options(jtest PUBLIC ${COMP_FLAGS})

