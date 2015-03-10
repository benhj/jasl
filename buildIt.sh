boostIncPath=/usr/local/include

echo "Building VarCache..."
clang++ -std=c++11 -I$boostIncPath -c VarCache.cpp -o VarCache.o 

echo "Building IfCommand..."
clang++ -std=c++11 -I$boostIncPath -c commands/IfCommand.cpp -o IfCommand.o 

echo "Building RepeatCommand..."
clang++ -std=c++11 -I$boostIncPath -c commands/RepeatCommand.cpp -o RepeatCommand.o 

echo "Building BlockCommand..."
clang++ -std=c++11 -I$boostIncPath -c commands/BlockCommand.cpp -o BlockCommand.o 

echo "Building MathExpression..."
clang++ -std=c++11 -I$boostIncPath -c commands/expressions/MathExpression.cpp -o MathExpression.o 

echo "Building ComparisonExpression..."
clang++ -std=c++11 -I$boostIncPath -c commands/expressions/ComparisonExpression.cpp -o ComparisonExpression.o 

echo "Building CommandInterpretor..."
clang++ -std=c++11 -I$boostIncPath -c CommandInterpretor.cpp -o CommandInterpretor.o

echo "Building test..."
clang++ -std=c++11 -I$boostIncPath -L/usr/local/lib VarCache.o \
                                                    IfCommand.o \
                                                    RepeatCommand.o \
                                                    BlockCommand.o \
                                                    MathExpression.o \
                                                    ComparisonExpression.o \
                                                    CommandInterpretor.o \
                                                    test.cpp -o test