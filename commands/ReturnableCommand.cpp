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
            } else if(type == "array") {
                std::string subType;
                (void)func.getValueB<std::string>(subType, sharedCache);
                if(subType == "int") {
                    return Type::IntArray;
                } else if(subType == "real") {
                    return Type::DoubleArray;
                } else {
                    throw std::runtime_error("fn: can't derive sub type");
                }
            } else {
                throw std::runtime_error("fn: can't derive type");
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
            if(!(m_returnType == Type::IntArray || m_returnType == Type::DoubleArray)) {
                (void)m_func.getValueD<std::string>(m_returnSymbol, m_sharedCache);
            } else {
                (void)m_func.getValueE<std::string>(m_returnSymbol, m_sharedCache);
            }
        }
        if(!(m_returnType == Type::IntArray || m_returnType == Type::DoubleArray)) {
            (void)m_func.getValueB<std::string>(m_functionName, m_sharedCache);
        } else {
            (void)m_func.getValueC<std::string>(m_functionName, m_sharedCache);
        }
    }

    bool ReturnableCommand::execute()
    {
        
        if(!(m_returnType == Type::IntArray || m_returnType == Type::DoubleArray)) {
            extractAndUpdateParams(m_func.paramC, m_sharedCache);
        } else {
            extractAndUpdateParams(m_func.paramD, m_sharedCache);
        }

        interpretFunctionBody();

        // Now set return param in GlobalCache
        if(m_returnType == Type::Int) {
            int64_t value;
            (void)m_sharedCache->getVar_(m_returnSymbol, value, Type::Int);
            GlobalCache::setInt(m_returnSymbol, value);
        } else if(m_returnType == Type::Bool) {
            bool value;
            (void)m_sharedCache->getVar_(m_returnSymbol, value, Type::Bool);
            GlobalCache::setBool(m_returnSymbol, value);
        } else if(m_returnType == Type::Double) {
            double value;
            (void)m_sharedCache->getVar_(m_returnSymbol, value, Type::Double);
            GlobalCache::setDouble(m_returnSymbol, value);
        } else if(m_returnType == Type::String) {
            std::string value;
            (void)m_sharedCache->getVar_(m_returnSymbol, value, Type::String);
            GlobalCache::setString(m_returnSymbol, value);
        } else if(m_returnType == Type::ValueArray) {
            ValueArray value;
            (void)m_sharedCache->getVar_(m_returnSymbol, value, Type::ValueArray);
            GlobalCache::setList(m_returnSymbol, value);
        } else if(m_returnType == Type::IntArray) {
            IntArray value;
            (void)m_sharedCache->getVar_(m_returnSymbol, value, Type::IntArray);
            GlobalCache::setIntArray(m_returnSymbol, value);
        } else if(m_returnType == Type::DoubleArray) {
            DoubleArray value;
            (void)m_sharedCache->getVar_(m_returnSymbol, value, Type::DoubleArray);
            GlobalCache::setDoubleArray(m_returnSymbol, value);
        } else if(m_returnType == Type::ByteArray) {
            ByteArray value;
            (void)m_sharedCache->getVar_(m_returnSymbol, value, Type::ByteArray);
            GlobalCache::setByteArray(m_returnSymbol, value);
        }

        return true;
    }

    bool ReturnableCommand::interpretFunctionBody()
    {
        std::vector<Function> innerFuncs;
        bool success = VarExtractor::tryAnyCast<std::vector<Function>>(innerFuncs, 
                                                                       (!(m_returnType == Type::IntArray || 
                                                                          m_returnType == Type::DoubleArray)) ?
                                                                       m_func.paramE :
                                                                       m_func.paramF);
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