#pragma once

#include "../Function.hpp"
#include "../VarCache.hpp"
#include <boost/optional.hpp>
#include <ostream>
#include <string>

namespace lightlang {
    struct CVarCommand
    {

        /// for capturing any output
        typedef ::boost::optional<std::ostream&> OptionalOutputStream;

        CVarCommand(Function &func_,
                    OptionalOutputStream output = OptionalOutputStream())
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

        bool handleInt(std::string const &varName)
        {
            auto a = VarExtractor::trySingleIntExtraction(func.paramB);
            if (!a) {
                return false;
            } 

            VarCache::intCache[varName] = *a;
            return true;
            
        }

        bool handleDouble(std::string const &varName)
        {
            auto a = VarExtractor::trySingleDoubleExtraction(func.paramB);
            if (!a) {
                return false;
            } 

            VarCache::doubleCache[varName] = *a;
            return true;
        }

        bool handleBool(std::string const &varName)
        {
            auto a = VarExtractor::trySingleBoolExtraction(func.paramB);
            if (!a) {
                return false;
            } 

            VarCache::boolCache[varName] = *a;
            return true;
        }

        bool execute()
        {

            std::string type = func.name;
            std::string varName; 
            (void)func.getValueA<std::string>(varName);

            if (type == "int") {

                return handleInt(varName);

            } else if (type == "double") {

                return handleDouble(varName);

            } else if (type == "bool") {

                return handleBool(varName);

            } 

            errorMessage = "cvar: type not supported";
            appendToOutput(errorMessage);
            return false;
        }

        Function &func;

        /// for optionally capturing output
        ::boost::optional<std::ostream&> outputStream;

        /// for setting an error message that can be later queried
        std::string errorMessage;
    };
}

