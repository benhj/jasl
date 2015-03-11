//
//  AppendCommand.cpp
//  jasl
//
//  Created by Ben Jones on 11/03/15
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "Command.hpp"
#include "../VarExtractor.hpp"
#include "../VarCache.hpp"
#include <sstream>

namespace jasl
{
    class AppendCommand : public Command
    {
    public:
        AppendCommand(Function &func_,
                      OptionalOutputStream const &output = OptionalOutputStream())
        : Command(func_, output)
        {

        }

        bool execute() override
        {
            std::string stringName;
            if(!m_func.getValueA<std::string>(stringName)) {
                m_errorMessage = "string: couldn't parse";
                appendToOutput(m_errorMessage);
                return false;
            }

            if(tryLiteralExtraction(stringName)) { return true; }
            if(trySymbolExtraction(stringName)) { return true; }
            if(tryNumericExtraction(stringName)) { return true; }
            m_errorMessage = "echo: couldn't parse";
            appendToOutput(m_errorMessage);
            return false;
        }

    private:
        bool tryLiteralExtraction(std::string const &key) 
        {

            // gracefully fail if string with name cannot be found
            auto it = VarCache::stringCache.find(key);
            if(it == std::end(VarCache::stringCache)) {
                return false;
            }
            
            LiteralString literalString;
            if(m_func.getValueB<LiteralString>(literalString)) {
                VarCache::stringCache[key] = VarCache::stringCache[key].append(literalString.literal);
                return true;
            }
            return false;
        }

        bool trySymbolExtraction(std::string const &key)
        {

            // gracefully fail if string with name cannot be found
            auto it = VarCache::stringCache.find(key);
            if(it == std::end(VarCache::stringCache)) {
                return false;
            }

            // Now try extracting a symbol
            std::string symbol;
            if(m_func.getValueB<std::string>(symbol)) {
                {
                    auto result = VarExtractor::searchInt(symbol);
                    if(result) {
                        VarCache::stringCache[key] = VarCache::stringCache[key].append(std::to_string(*result));
                        return true;
                    }
                }
                {
                    auto result = VarExtractor::searchDouble(symbol);
                    if(result) {
                        VarCache::stringCache[key] = VarCache::stringCache[key].append(std::to_string(*result));
                        return true;
                    }
                }
                {
                    auto result = VarExtractor::searchBool(symbol);
                    if(result) {
                        VarCache::stringCache[key] = VarCache::stringCache[key].append(std::to_string(*result));
                        return true;
                    }
                }
                {
                    auto result = VarExtractor::searchString(symbol);
                    if(result) {
                        VarCache::stringCache[key] = VarCache::stringCache[key].append(*result);
                        return true;
                    }
                }
                return true;
            }

            return false;
        }

        bool tryNumericExtraction(std::string const &key)
        {

            // gracefully fail if string with name cannot be found
            auto it = VarCache::stringCache.find(key);
            if(it == std::end(VarCache::stringCache)) {
                return false;
            }

            {
                auto result = VarExtractor::trySingleIntExtractionNoMath(m_func.paramB);
                if(result) {
                    VarCache::stringCache[key] = VarCache::stringCache[key].append(std::to_string(*result));
                    return true;
                }
            }

            {
                auto result = VarExtractor::tryToGetADouble(m_func.paramB);
                if(result) {
                    std::ostringstream ss;
                    ss << *result;
                     VarCache::stringCache[key] = VarCache::stringCache[key].append(ss.str());
                    return true;
                }
            }

            {
                auto result = VarExtractor::trySingleBoolExtraction(m_func.paramB);
                if(result) {
                    VarCache::stringCache[key] = VarCache::stringCache[key].append(std::to_string(*result));
                    return true;
                }
            }

            return false;

        }

    };

}