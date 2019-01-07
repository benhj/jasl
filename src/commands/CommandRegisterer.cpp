#include "CommandRegisterer.hpp"

// Hack to enforce self-registration
#include "commands/ArgsCommand.cpp"
#include "commands/ExecCommand.cpp"

#include "commands/screenio/AnsiUPCommand.cpp"
#include "commands/screenio/EchoCommand.cpp"
#include "commands/screenio/EchoNLCommand.cpp"
#include "commands/screenio/InputCommand.cpp"

#include "commands/types/ArrayCommand.cpp"
#include "commands/types/ArrayPutCommand.cpp"
#include "commands/types/ArrayGetCommand.cpp"
#include "commands/types/NewPrimitiveSyntaxCommand.cpp"
#include "commands/types/PutCommand.cpp"
#include "commands/types/ReleaseCommand.cpp"
#include "commands/types/TypeCommand.cpp"

#include "commands/ReverseCommand.cpp"
#include "commands/string/StringCommand.cpp"
#include "commands/LengthCommand.cpp"
#include "commands/RandomCommand.cpp"

#include "commands/flow/BlockCommand.cpp"
#include "commands/flow/ReturnableCommand.cpp"
#include "commands/flow/CallCommand.cpp"
#include "commands/flow/IfCommand.cpp"
#include "commands/flow/RepeatCommand.cpp"
#include "commands/flow/StartCommand.cpp"
#include "commands/flow/WhileCommand.cpp"
#include "commands/flow/ForCommand.cpp"

#include "commands/list/ListCommand.cpp"
#include "commands/list/ListGetTokenCommand.cpp"
#include "commands/list/ListSetTokenCommand.cpp"
#include "commands/list/ListAddTokenCommand.cpp"
#include "commands/list/ListTokenIndexCommand.cpp"

#include "commands/string/AppendCommand.cpp"
#include "commands/string/ConcatCommand.cpp"
#include "commands/string/ListToStringCommand.cpp"
#include "commands/string/RegexEqCommand.cpp"
#include "commands/string/RegexParseCommand.cpp"
#include "commands/string/StringToPrimitiveCommand.cpp"
#include "commands/string/TokenizeCommand.cpp"
#include "commands/string/WildcardEqCommand.cpp"
#include "commands/string/WildcardParseCommand.cpp"

#include "commands/fileio/FolderListCommand.cpp"
#include "commands/fileio/FolderListRecursiveCommand.cpp"
#include "commands/fileio/FileReadBytesCommand.cpp"
#include "commands/fileio/FileReadLinesCommand.cpp"
#include "commands/fileio/FileWriteBytesCommand.cpp"
#include "commands/fileio/FileAppendBytesCommand.cpp"
#include "commands/fileio/FileAppendStringCommand.cpp"
#include "commands/fileio/FileTypeCommand.cpp"

#include "commands/net/NetReadCommand.cpp"

namespace jasl {
    void registerAll(){}
}