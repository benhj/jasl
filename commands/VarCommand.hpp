#pragma once

#include "../Function.hpp"
#include "../VarExtractor.hpp"
#include "../VarCache.hpp"
#include <string>

namespace lightlang
{

    struct SetParamCommand
    {
        VarCommand(CommandFunction::Function &func_)
        : func(func_) 
        {

        }

        void operator()()
        {
            std::string type; 
            (void)func.getValueA<std::string>(type);
            std::string key; 
            (void)func.getValueB<std::string>(key);

            if (type == "bool") {
                if (!tryBool(key)) {
                    // TODO: throw
                }
            } else if (type == "int") {
                if (!tryInt(key)) {
                    // TODO: throw
                }
            } else if (type == "double") {
                if (!tryDouble(key)) {
                    // TODO: throw
                }
            } else if (type == "string") {
                if (!tryString(key)) {
                    // TODO: throw
                }
            } else {
                // TODO: throw
            }
        }

        bool tryDouble(std::string const &key)
        {
            double val;
            bool extracted = VarExtractor::tryToGetADouble(val, func.paramC);
            if (extracted) {
                VarCache::doubleCache[key] = val;
            }
            return extracted;
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
            bool val;
            bool extracted = VarExtractor::trySingleBoolExtraction(val, func.paramC);
            if (extracted) {
                VarCache::boolCache[key] = val;
            }
            return extracted;
        }

        bool tryInt(std::string const &key)
        {
            int val;
            bool extracted = VarExtractor::trySingleIntExtraction(val, func.paramC);
            if (extracted) {
                VarCache::intCache[key] = val;
            }
            return extracted;
        }
        CommandFunction::Function &func;
    };

}