#!/bin/bash 
# set build flags
compiler=clang++
cflags="-std=c++1y -O3 -ffast-math -funroll-loops -Wno-ctor-dtor-privacy"
boostIncPath=/usr/local/include

# these objects will exist after compilation of everything
objects="*.o"
commandObs="commands/*.o"
expressionObs="commands/expressions/*.o"

function build_commands {
    for f in commands/*.cpp; do 
        echo "Building $f"
        $compiler $cflags -I$boostIncPath -c $f -o ${f%%.*}.o
    done
}

function build_expressions {
    for f in commands/expressions/*.cpp; do 
        echo "Building $f"
        $compiler $cflags -I$boostIncPath -c $f -o ${f%%.*}.o
    done
}

function build_other {
    for f in *.cpp; do 
        echo "Building $f"
        $compiler $cflags -I$boostIncPath -c $f -o ${f%%.*}.o
    done
}

function build_tests {
    echo "Building tests..."
    $compiler $cflags -I$boostIncPath $objects $commandObs $expressionObs bin/test.cpp -o test
}

function build_jasl {
    echo "Building jasl..."
    $compiler $cflags -I$boostIncPath $objects $commandObs $expressionObs bin/jasl.cpp -o jasl
}

function build_clean {
    echo "Removing compiled items..."
    rm *.o
    rm commands/*.o
    rm commands/expressions/*.o
    rm test
    rm jasl
}

function build_all {
    build_commands
    build_expressions
    build_other
    build_tests
    build_jasl
}

# control which parts we need to be built.
# For example "./sh buildIt.sh all" will build everything
# while "./sh buildIt.sh tests" will attempt to build the tests
build_$1
