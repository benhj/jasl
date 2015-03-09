echo "Building VarCache..."
clang++ -std=c++11 -I/usr/local/include/ -c VarCache.cpp -o VarCache.o 
echo "Building IfCommand..."
clang++ -std=c++11 -I/usr/local/include/ -c commands/IfCommand.cpp -o IfCommand.o 
echo "Building MathExpression..."
clang++ -std=c++11 -I/usr/local/include/ -c commands/expressions/MathExpression.cpp -o MathExpression.o 
echo "Building ComparisonExpression..."
clang++ -std=c++11 -I/usr/local/include/ -c commands/expressions/ComparisonExpression.cpp -o ComparisonExpression.o 
echo "Building CommandInterpretor..."
clang++ -std=c++11 -I/usr/local/include/ -c CommandInterpretor.cpp -o CommandInterpretor.o
echo "Building test..."
clang++ -std=c++11 -I/usr/local/include/ -L/usr/local/lib \
VarCache.o \
IfCommand.o \
MathExpression.o \
ComparisonExpression.o \
CommandInterpretor.o \
test.cpp -o test