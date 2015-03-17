//
//  NewPrimitiveSyntaxCommand.hpp
//  jasl
//
//  Created by Ben Jones on 13/03/15
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "Command.hpp"
#include "StringToPrimitiveCommand.hpp"
#include "../VarCache.hpp"

namespace jasl {
    class NewPrimitiveSyntaxCommand : public Command
    {
    public:
        NewPrimitiveSyntaxCommand(Function &func_,
                                  OptionalOutputStream const &output = OptionalOutputStream())
        : Command(func_, output)
        {
        }

        bool execute() override
        {

            std::string type = m_func.name;
            std::string varName; 
            (void)m_func.getValueB<std::string>(varName);

            if (type == "integer") {

                return handleInt(varName);

            } else if (type == "decimal") {

                return handleDouble(varName);

            } else if (type == "boolean") {

                return handleBool(varName);

            } 

            m_errorMessage = "cvar: type not supported";
            appendToOutput(m_errorMessage);
            return false;
        }

    private:
        bool handleInt(std::string const &varName)
        {
            auto a = VarExtractor::trySingleIntExtraction(m_func.paramA);
            if (!a) {
                // try converting a string to an integer
                return StringToPrimitiveCommand(m_func, m_outputStream).execute();
            } 
            VarCache::intCache[varName] = *a;
            return true;
        }

        bool handleDouble(std::string const &varName)
        {
            auto a = VarExtractor::trySingleDoubleExtraction(m_func.paramA);
            if (!a) {
                // try converting a string to a double
                return StringToPrimitiveCommand(m_func, m_outputStream).execute();
            } 

            VarCache::doubleCache[varName] = *a;
            return true;
        }

        bool handleBool(std::string const &varName)
        {
            auto a = VarExtractor::trySingleBoolExtraction(m_func.paramA);
            if (!a) {
                return false;
            } 

            VarCache::boolCache[varName] = *a;
            return true;
        }
    };
}

