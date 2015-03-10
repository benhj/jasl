//
//  VarCommand.cpp
//  lightlang
//
//  Created by Ben Jones 
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "Command.hpp"
#include "../VarExtractor.hpp"
#include "../VarCache.hpp"

namespace lightlang
{
    class VarCommand : public Command
    {
    public:
        VarCommand(Function &func_,
                   OptionalOutputStream const &output = OptionalOutputStream())
        : Command(func_, output)
        {

        }

        bool execute() override
        {
            std::string type; 
            (void)m_func.getValueA<std::string>(type);
            std::string key; 
            (void)m_func.getValueB<std::string>(key);

            if (type == "bool") {
                if (!tryBool(key)) {
                    m_errorMessage = "Error setting bool var";
                    appendToOutput(m_errorMessage);
                }
            } else if (type == "int") {
                if (!tryInt(key)) {
                    m_errorMessage = "Error setting int var";
                    appendToOutput(m_errorMessage);
                }
            } else if (type == "double") {
                if (!tryDouble(key)) {
                    m_errorMessage = "Error setting double var";
                    appendToOutput(m_errorMessage);
                }
            } else if (type == "string") {
                if (!tryString(key)) {
                    m_errorMessage = "Error setting string var";
                    appendToOutput(m_errorMessage);
                }
            } else {
                m_errorMessage = "Error setting unknown type var";
                appendToOutput(m_errorMessage);
                return false;
            }
            return true;
        }

    private:
        bool tryDouble(std::string const &key)
        {
            auto extracted = VarExtractor::tryToGetADouble(m_func.paramC);
            if (extracted) {
                VarCache::doubleCache[key] = *extracted;
            }
            return (extracted != OptionalDouble());
        }

        bool tryString(std::string const &key)
        {
            std::string val;
            bool extracted = m_func.getValueC<std::string>(val);
            if (extracted) {
                VarCache::stringCache[key] = val;
            }
            return extracted;
        }

        bool tryBool(std::string const &key)
        {
            auto extracted = VarExtractor::trySingleBoolExtraction(m_func.paramC);
            if (extracted) {
                VarCache::boolCache[key] = *extracted;
            }
            return (extracted != OptionalBool());
        }

        bool tryInt(std::string const &key)
        {
            auto extracted = VarExtractor::trySingleIntExtraction(m_func.paramC);
            if (extracted) {
                VarCache::intCache[key] = *extracted;
            }
            return (extracted != OptionalInt());
        }

    };

}