//
//  ReturnableCommand.hpp
//  jasl
//
//  Created by Ben Jones on on 10/10/2015.
//  Copyright (c) 2015-2016 Ben Jones. All rights reserved.
//

#include "ReturnableCommand.hpp"
#include "ParamExtractor.hpp"
#include "../CommandInterpretor.hpp"
#include "../GlobalCache.hpp"
#include <vector>

namespace jasl 
{

    namespace {
        Type getReturnType(Function &func, SharedVarCache const &sharedCache)
        {
            std::string type;
            (void)func.getValueA<std::string>(type, sharedCache);
            if(type == "int") {
                return Type::Int;
            } else if(type == "real") {
                return Type::Double;
            } else if(type == "string") {
                return Type::String;
            } else if(type == "bool") {
                return Type::Bool;
            } else if(type == "list") {
                return Type::ValueArray;
            } else if(type == "nil") {
                return Type::None;
            } else {
                throw std::runtime_error("Bollocks");
            }
        }
    }

    ReturnableCommand::ReturnableCommand(Function &func_,
                                         SharedVarCache const &sharedCache,
                                         OptionalOutputStream const &output)
        : Command(func_, std::make_shared<ScopedVarCache>(), output)
        , m_functionName()
        , m_returnSymbol()
        , m_returnType(getReturnType(func_, sharedCache))
    {
        if(m_returnType != Type::None) {
            (void)m_func.getValueD<std::string>(m_returnSymbol, m_sharedCache);
        }
        (void)m_func.getValueB<std::string>(m_functionName, m_sharedCache);
    }

    bool ReturnableCommand::execute()
    {
        
        extractAndUpdateParams(m_func.paramC, m_sharedCache);
        interpretFunctionBody();

        // Now set return param in GlobalCache
        if(m_returnType == Type::Int) {
            int64_t value;
            (void)m_sharedCache->getInt_(m_returnSymbol, value);
            GlobalCache::setInt(m_returnSymbol, value);
        } else if(m_returnType == Type::Bool) {
            bool value;
            (void)m_sharedCache->getBool_(m_returnSymbol, value);
            GlobalCache::setBool(m_returnSymbol, value);
        } else if(m_returnType == Type::Double) {
            double value;
            (void)m_sharedCache->getDouble_(m_returnSymbol, value);
            GlobalCache::setDouble(m_returnSymbol, value);
        } else if(m_returnType == Type::String) {
            std::string value;
            (void)m_sharedCache->getString_(m_returnSymbol, value);
            GlobalCache::setString(m_returnSymbol, value);
        } else if(m_returnType == Type::ValueArray) {
            ValueArray value;
            (void)m_sharedCache->getList_(m_returnSymbol, value);
            GlobalCache::setList(m_returnSymbol, value);
        }

        return true;
    }

    bool ReturnableCommand::interpretFunctionBody()
    {
        std::vector<Function> innerFuncs;
        bool success = VarExtractor::tryAnyCast<std::vector<Function>>(innerFuncs, m_func.paramE);
        if (success) {
            success = parseCommands(innerFuncs);
        } else {
            setLastErrorMessage("returnable: Error interpreting returnable's body");
            return false;
        }

        return true;
    }

    bool ReturnableCommand::parseCommands(std::vector<Function> &functions) 
    {
        CommandInterpretor ci;
        for(auto & f : functions) {
            (void)ci.interpretFunc(f, m_sharedCache, m_outputStream);
        }
        return true;
    }
}