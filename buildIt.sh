clang++ -std=c++11 -I/usr/local/include/ -c VarCache.cpp -o VarCache.o 
clang++ -std=c++11 -I/usr/local/include/ -c commands/expressions/MathExpression.cpp -o MathExpression.o 
clang++ -std=c++11 -I/usr/local/include/ -c commands/expressions/ComparisonExpression.cpp -o ComparisonExpression.o 
clang++ -std=c++11 -I/usr/local/include/ -c CommandInterpretor.cpp -o CommandInterpretor.o
clang++ -std=c++11 -I/usr/local/include/ -L/usr/local/lib \
VarCache.o \
MathExpression.o \
ComparisonExpression.o \
CommandInterpretor.o \
test.cpp -o test