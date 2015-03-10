#!/bin/bash 
# set build flags
compiler=clang++
cflags=-std=c++11
boostIncPath=/usr/local/include

# these objects will exist after compilation of everything
objects="VarCache.o \
         IfCommand.o \
         RepeatCommand.o \
         BlockCommand.o \
         StartCommand.o \
         CallCommand.o \
         MathExpression.o \
         ComparisonExpression.o \
         CommandInterpretor.o"

function build_VarCache {
    echo "Building VarCache..."
    $compiler $cflags -I$boostIncPath -c VarCache.cpp -o VarCache.o 
}

function build_CallCommand {
    echo "Building CallCommand..."
    $compiler $cflags -I$boostIncPath -c commands/CallCommand.cpp -o CallCommand.o 
}

function build_IfCommand {
    echo "Building IfCommand..."
    $compiler $cflags -I$boostIncPath -c commands/IfCommand.cpp -o IfCommand.o 
}

function build_RepeatCommand {
    echo "Building RepeatCommand..."
    $compiler $cflags -I$boostIncPath -c commands/RepeatCommand.cpp -o RepeatCommand.o 
}

function build_StartCommand {
    echo "Building StartCommand..."
    $compiler $cflags -I$boostIncPath -c commands/StartCommand.cpp -o StartCommand.o 
}

function build_BlockCommand {
    echo "Building BlockCommand..."
    $compiler $cflags -I$boostIncPath -c commands/BlockCommand.cpp -o BlockCommand.o 
}

function build_MathExpression {
    echo "Building MathExpression..."
    $compiler $cflags -I$boostIncPath -c commands/expressions/MathExpression.cpp -o MathExpression.o 
}

function build_ComparisonExpression {
    echo "Building ComparisonExpression..."
    $compiler $cflags -I$boostIncPath -c commands/expressions/ComparisonExpression.cpp -o ComparisonExpression.o 
}

function build_CommandInterpretor {
    echo "Building CommandInterpretor..."
    $compiler $cflags -I$boostIncPath -c CommandInterpretor.cpp -o CommandInterpretor.o
}

function build_tests {
    echo "Building tests..."
    $compiler $cflags -I$boostIncPath $objects test.cpp -o test
}

function build_all {
    build_VarCache
    build_CallCommand
    build_IfCommand
    build_RepeatCommand
    build_StartCommand
    build_BlockCommand
    build_MathExpression
    build_ComparisonExpression
    build_CommandInterpretor
    build_tests
}

# control which parts we need to be built.
# For example "./sh buildIt.sh all" will build everything
# while "./sh buildIt.sh tests" will attempt to build the tests
build_$1
