#pragma once

#include "../Function.hpp"
#include "../VarCache.hpp"
#include <string>

namespace lightlang {
    struct CVarCommand
    {
        CVarCommand(Function &func_)
        : func(func_)
        {
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
            return false;
        }

        Function &func;
        std::string errorMessage;
    };
}

