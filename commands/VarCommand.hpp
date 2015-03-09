#pragma once

#include "../Function.hpp"
#include "../VarExtractor.hpp"
#include "../VarCache.hpp"
#include <boost/optional.hpp>
#include <ostream>
#include <string>

namespace lightlang
{
    struct VarCommand
    {

        /// for capturing any output
        typedef ::boost::optional<std::ostream&> OptionalOutputStream;

        explicit VarCommand(Function &func_,
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

        bool execute()
        {
            std::string type; 
            (void)func.getValueA<std::string>(type);
            std::string key; 
            (void)func.getValueB<std::string>(key);

            if (type == "bool") {
                if (!tryBool(key)) {
                    errorMessage = "Error setting bool var";
                    appendToOutput(errorMessage);
                }
            } else if (type == "int") {
                if (!tryInt(key)) {
                    errorMessage = "Error setting int var";
                    appendToOutput(errorMessage);
                }
            } else if (type == "double") {
                if (!tryDouble(key)) {
                    errorMessage = "Error setting double var";
                    appendToOutput(errorMessage);
                }
            } else if (type == "string") {
                if (!tryString(key)) {
                    errorMessage = "Error setting string var";
                    appendToOutput(errorMessage);
                }
            } else {
                errorMessage = "Error setting unknown type var";
                appendToOutput(errorMessage);
                return false;
            }
            return true;
        }

        bool tryDouble(std::string const &key)
        {
            auto extracted = VarExtractor::tryToGetADouble(func.paramC);
            if (extracted) {
                VarCache::doubleCache[key] = *extracted;
            }
            return (extracted != OptionalDouble());
        }

        bool tryString(std::string const &key)
        {
            std::string val;
            bool extracted = func.getValueC<std::string>(val);
            if (extracted) {
                VarCache::stringCache[key] = val;
            }
            return extracted;
        }

        bool tryBool(std::string const &key)
        {
            auto extracted = VarExtractor::trySingleBoolExtraction(func.paramC);
            if (extracted) {
                VarCache::boolCache[key] = *extracted;
            }
            return (extracted != OptionalBool());
        }

        bool tryInt(std::string const &key)
        {
            auto extracted = VarExtractor::trySingleIntExtraction(func.paramC);
            if (extracted) {
                VarCache::intCache[key] = *extracted;
            }
            return (extracted != OptionalInt());
        }

        Function &func;

        /// for optionally capturing output
        ::boost::optional<std::ostream&> outputStream;

        /// for setting an error message that can be later queried
        std::string errorMessage;
    };

}