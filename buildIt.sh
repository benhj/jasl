#!/bin/bash
# set build flags
compiler=clang++
cflags="-std=c++1y -O3 -ffast-math -funroll-loops -Wno-ctor-dtor-privacy -fno-pic"
boostIncPath=/usr/local/include
boostLibPath=/usr/local/lib

# these objects will exist after compilation of everything
objects="objects/*.o"

function check_obs_dir {
    if [ ! -d "objects" ]; then
        mkdir objects
    fi
}

function build_commands {
    for f in commands/*.cpp; do
        echo "Building $f"
        $compiler $cflags -I$boostIncPath -c $f -o ${f%%.*}.o
        mv ${f%%.*}.o objects/
    done
}

function build_commands_fileio {
    for f in commands/fileio/*.cpp; do
        echo "Building $f"
        $compiler $cflags -I$boostIncPath -c $f -o ${f%%.*}.o
        mv ${f%%.*}.o objects/
    done
}

function build_expressions {
    for f in commands/expressions/*.cpp; do
        echo "Building $f"
        $compiler $cflags -I$boostIncPath -c $f -o ${f%%.*}.o
        mv ${f%%.*}.o objects/
    done
}

function build_caching {
    for f in caching/*.cpp; do
        echo "Building $f"
        $compiler $cflags -I$boostIncPath -c $f -o ${f%%.*}.o
        mv ${f%%.*}.o objects/
    done
}

function build_other {
    for f in *.cpp; do
        echo "Building $f"
        $compiler $cflags -I$boostIncPath -c $f -o ${f%%.*}.o
        mv ${f%%.*}.o objects/
    done
}

function build_single () {
    echo "Building $1"
    $compiler $cflags -I$boostIncPath -c $1 -o ${f%%.*}.o
    mv ${f%%.*}.o objects/
}

function build_tests {
    echo "Building tests..."
    $compiler $cflags -I$boostIncPath $objects -L$boostLibPath -lboost_filesystem -lboost_system bin/test.cpp -o test
}

function build_jasl {
    echo "Building jasl..."
    $compiler $cflags -I$boostIncPath $objects -L$boostLibPath -lboost_filesystem -lboost_system bin/jasl.cpp -o jasl
}

function build_clean {
    echo "Removing compiled items..."
    rm -fr objects
    rm test
    rm jasl
}

function build_all {
    check_obs_dir
    build_commands
    build_commands_fileio
    build_expressions
    build_caching
    build_other
    build_tests
    build_jasl
}

# control which parts we need to be built.
# For example "./sh buildIt.sh all" will build everything
# while "./sh buildIt.sh tests" will attempt to build the tests
# Or, to build a single object file:
# ./sh build.sh single commands/ArrayCommand.cpp 
build_$1 $2