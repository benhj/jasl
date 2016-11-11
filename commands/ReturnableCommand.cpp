//
//  ReturnableCommand.hpp
//  jasl
//
//  Created by Ben Jones on 10/10/2015.
//  Copyright (c) 2015-2016 Ben Jones. All rights reserved.
//

#include "ReturnableCommand.hpp"
#include "ParamExtractor.hpp"
#include "../CommandInterpretor.hpp"
#include "../caching/GlobalCache.hpp"
#include <vector>

namespace jasl 
{

    namespace {

        Type getForArrayTypes(std::string const & theType) {
            if(theType == "int" || theType == "ints") {
                return Type::IntArray;
            } else if(theType == "real" || theType == "reals") {
                return Type::RealArray;
            } else if(theType == "byte" || theType == "bytes") {
                return Type::ByteArray;
            } else if(theType == "string" || theType == "string") {
                return Type::StringArray;
            } else {
                throw std::runtime_error("fn: can't derive sub type");
            }
        }

        Type getReturnType(Function &func, 
                           SharedCacheStack const &sharedCache,
                           std::string & type)
        {
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
            } else if(type == "array") {
                std::string subType;
                (void)func.getValueB<std::string>(subType, sharedCache);
                return getForArrayTypes(subType);
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
        , m_returnType(getReturnType(func_, sharedCache, m_stringType))
    {
        if(m_returnType != Type::None) {
            if(!(m_returnType == Type::IntArray  || 
                 m_returnType == Type::RealArray ||
                 m_returnType == Type::ByteArray ||
                 m_returnType == Type::StringArray)) {
                (void)m_func.getValueD<std::string>(m_returnSymbol, m_sharedCache);
            } else {
                if(m_stringType == "array") {
                    (void)m_func.getValueE<std::string>(m_returnSymbol, m_sharedCache);
                } else {
                    (void)m_func.getValueD<std::string>(m_returnSymbol, m_sharedCache);
                }
            }
        }
        if(!(m_returnType == Type::IntArray  || 
             m_returnType == Type::RealArray ||
             m_returnType == Type::ByteArray ||
             m_returnType == Type::StringArray)) {
            (void)m_func.getValueB<std::string>(m_functionName, m_sharedCache);
        } else {
            if(m_stringType == "array") {
                (void)m_func.getValueC<std::string>(m_functionName, m_sharedCache);
            } else {
                (void)m_func.getValueB<std::string>(m_functionName, m_sharedCache);
            }
            
        }
    }

    bool ReturnableCommand::execute()
    {
        
        if(!(m_returnType == Type::IntArray  || 
             m_returnType == Type::RealArray ||
             m_returnType == Type::ByteArray ||
             m_returnType == Type::StringArray)) {
            popParams(m_func.paramC, m_sharedCache);
        } else {
            if(m_stringType == "array") {
                popParams(m_func.paramD, m_sharedCache);
            } else {
                popParams(m_func.paramC, m_sharedCache);
            }
        }

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
                                                                       (!(m_returnType == Type::IntArray  || 
                                                                          m_returnType == Type::RealArray ||
                                                                          m_returnType == Type::ByteArray ||
                                                                          m_returnType == Type::StringArray)) ?
                                                                       m_func.paramE :
                                                                       (m_stringType == "array" ? m_func.paramF : m_func.paramE));
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