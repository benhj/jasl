# set build flags
compiler=clang++
cflags=-std=c++11
boostIncPath=/usr/local/include

echo "Building VarCache..."
$compiler $cflags -I$boostIncPath -c VarCache.cpp -o VarCache.o 

echo "Building CallCommand..."
$compiler $cflags -I$boostIncPath -c commands/CallCommand.cpp -o CallCommand.o 

echo "Building IfCommand..."
$compiler $cflags -I$boostIncPath -c commands/IfCommand.cpp -o IfCommand.o 

echo "Building RepeatCommand..."
$compiler $cflags -I$boostIncPath -c commands/RepeatCommand.cpp -o RepeatCommand.o 

echo "Building StartCommand..."
$compiler $cflags -I$boostIncPath -c commands/StartCommand.cpp -o StartCommand.o 

echo "Building BlockCommand..."
$compiler $cflags -I$boostIncPath -c commands/BlockCommand.cpp -o BlockCommand.o 

echo "Building MathExpression..."
$compiler $cflags -I$boostIncPath -c commands/expressions/MathExpression.cpp -o MathExpression.o 

echo "Building ComparisonExpression..."
$compiler $cflags -I$boostIncPath -c commands/expressions/ComparisonExpression.cpp -o ComparisonExpression.o 

echo "Building CommandInterpretor..."
$compiler $cflags -I$boostIncPath -c CommandInterpretor.cpp -o CommandInterpretor.o

objects="VarCache.o \
         IfCommand.o \
         RepeatCommand.o \
         BlockCommand.o \
         StartCommand.o \
         CallCommand.o \
         MathExpression.o \
         ComparisonExpression.o \
         CommandInterpretor.o"

echo "Building test..."
$compiler $cflags -I$boostIncPath $objects test.cpp -o test