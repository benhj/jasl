//
//  ReturnableCommand.hpp
//  jasl
//
//  Created by Ben Jones on 10/10/2015.
//  Copyright (c) 2015-present Ben Jones. All rights reserved.
//

#include "ReturnableCommand.hpp"
#include "commands/ParamExtractor.hpp"
#include "core/CommandInterpretor.hpp"
#include "caching/GlobalCache.hpp"
#include <vector>

namespace jasl 
{

    namespace {

        Type getForArrayTypes(std::string const & theType) {
            if(theType == "ints") {
                return Type::IntArray;
            } else if(theType == "reals") {
                return Type::RealArray;
            } else if(theType == "bytes") {
                return Type::ByteArray;
            } else if(theType == "strings") {
                return Type::StringArray;
            } else {
                throw std::runtime_error("fn: can't derive sub type");
            }
        }

        Type getReturnType(Function &func, 
                           SharedCacheStack const &sharedCache)
        {
            std::string type;
            (void)func.getValueA<std::string>(type, sharedCache);
            if(type == "int") {
                return Type::Int;
            } else if(type == "real") {
                return Type::Real;
            } else if(type == "string") {
                return Type::String;
            } else if(type == "bool") {
                return Type::Bool;
            } else if(type == "list") {
                return Type::List;
            } else if(type == "nil") {
                return Type::None;
            } else {
                return getForArrayTypes(type);
            }
        }
    }

    ReturnableCommand::ReturnableCommand(Function &func_,
                                         SharedCacheStack const &sharedCache,
                                         OptionalOutputStream const &output)
        : Command(func_, std::make_shared<CacheStack>(), output)
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
        
        popParams(m_func.paramC, m_sharedCache);
        interpretFunctionBody();

        // Now set return param in GlobalCache
        if(m_returnType == Type::Int) {
            int64_t value;
            (void)m_sharedCache->getVar_(m_returnSymbol, value, m_returnType);
            GlobalCache::pushReturnValue(m_returnType, value);
        } else if(m_returnType == Type::Bool) {
            bool value;
            (void)m_sharedCache->getVar_(m_returnSymbol, value, m_returnType);
            GlobalCache::pushReturnValue(m_returnType, value);
        } else if(m_returnType == Type::Real) {
            double value;
            (void)m_sharedCache->getVar_(m_returnSymbol, value, m_returnType);
            GlobalCache::pushReturnValue(m_returnType, value);
        } else if(m_returnType == Type::String) {
            std::string value;
            (void)m_sharedCache->getVar_(m_returnSymbol, value, m_returnType);
            GlobalCache::pushReturnValue(m_returnType, value);
        } else if(m_returnType == Type::List) {
            List value;
            (void)m_sharedCache->getVar_(m_returnSymbol, value, m_returnType);
            GlobalCache::pushReturnValue(m_returnType, value);
        } else if(m_returnType == Type::IntArray) {
            IntArray value;
            (void)m_sharedCache->getVar_(m_returnSymbol, value, m_returnType);
            GlobalCache::pushReturnValue(m_returnType, value);
        } else if(m_returnType == Type::RealArray) {
            RealArray value;
            (void)m_sharedCache->getVar_(m_returnSymbol, value, m_returnType);
            GlobalCache::pushReturnValue(m_returnType, value);
        } else if(m_returnType == Type::ByteArray) {
            ByteArray value;
            (void)m_sharedCache->getVar_(m_returnSymbol, value, m_returnType);
            GlobalCache::pushReturnValue(m_returnType, value);
        } else if(m_returnType == Type::StringArray) {
            StringArray value;
            (void)m_sharedCache->getVar_(m_returnSymbol, value, m_returnType);
            GlobalCache::pushReturnValue(m_returnType, value);
        }

        return true;
    }

    bool ReturnableCommand::interpretFunctionBody()
    {
        std::vector<Function> innerFuncs;

        bool success = VarExtractor::tryAnyCast<std::vector<Function>>(innerFuncs, 
                                                                       m_func.paramE);
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