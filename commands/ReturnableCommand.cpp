//
//  ReturnableCommand.hpp
//  jasl
//
//  Created by Ben Jones on on 10/10/2015.
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#include "ReturnableCommand.hpp"
#include "ParamExtractor.hpp"
#include "../CommandInterpretor.hpp"
#include "../GlobalCache.hpp"
#include <vector>

namespace jasl 
{
    ReturnableCommand::ReturnableCommand(Function &func_,
                                         SharedVarCache const &sharedCache,
                                         OptionalOutputStream const &output)
    : Command(func_, std::make_shared<ScopedVarCache>(), output)
    {
    }

    bool ReturnableCommand::execute()
    {
        std::string type;
        (void)m_func.getValueA<std::string>(type, m_sharedCache);
        std::string returnName;
        (void)m_func.getValueD<std::string>(returnName, m_sharedCache);
        std::string functionName; 
        (void)m_func.getValueB<std::string>(functionName, m_sharedCache);

        extractAndUpdateParams(m_func.paramC, m_sharedCache);

        if(type == "integer") {
            m_returnSymbol = returnName;
            m_returnType = Type::Int;
        } else if(type == "decimal") {
            m_returnSymbol = returnName;
            m_returnType = Type::Double;
        } else if(type == "string") {
            m_returnSymbol = returnName;
            m_returnType = Type::String;
        } else if(type == "boolean") {
            m_returnSymbol = returnName;
            m_returnType = Type::Bool;
        } else if(type == "list") {
            m_returnSymbol = returnName;
            m_returnType = Type::ValueArray;
        } else {
            setLastErrorMessage("returnable: unknown return type");
            return false;
        }

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