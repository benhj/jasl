#pragma once

#include "../Function.hpp"
#include "../VarExtractor.hpp"
#include "../VarCache.hpp"
#include <boost/optional.hpp>
#include <ostream>
#include <string>

namespace lightlang
{
    struct EchoCommand
    {

        /// for capturing any output
        typedef ::boost::optional<std::ostream&> OptionalOutputStream;

        explicit EchoCommand(Function &func_,
                             OptionalOutputStream const &output = OptionalOutputStream())
        : func(func_) 
        , outputStream(output)
        , errorMessage("")
        {

        }

        void appendToOutput(std::string const &message) 
        {
            if(outputStream) {
                *outputStream << message.c_str();
            }
        }

        bool execute()
        {
            std::string echoString;
            if(!func.getValueA<std::string>(echoString)) {
                errorMessage = "echo: problem getting string";
                appendToOutput(errorMessage);
                return false;
            }
            appendToOutput(echoString);
            return true;
        }

        Function &func;

        /// for optionally capturing output
        ::boost::optional<std::ostream&> outputStream;

        /// for setting an error message that can be later queried
        std::string errorMessage;
    };

}