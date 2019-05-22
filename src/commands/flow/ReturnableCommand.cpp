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
#include "core/RegisterCommand.hpp"
#include "caching/GlobalCache.hpp"
#include <vector>

bool jasl::ReturnableCommand::m_registered = 
registerCommand<jasl::ReturnableCommand>();

namespace jasl 
{

    namespace {

        Type getForArrayTypes(std::string const & theType,
                              std::string const & returnSymbol,
                              SharedCacheStack & sharedCache) {
            if(theType == "ints") {
                // Fix scoping issue by setting a default value
                (void)sharedCache->setVar(returnSymbol, IntArray(), Type::IntArray);
                return Type::IntArray;
            } else if(theType == "reals") {
                (void)sharedCache->setVar(returnSymbol, RealArray(), Type::RealArray);
                return Type::RealArray;
            } else if(theType == "bytes") {
                (void)sharedCache->setVar(returnSymbol, ByteArray(), Type::ByteArray);
                return Type::ByteArray;
            } else if(theType == "strings") {
                (void)sharedCache->setVar(returnSymbol, StringArray(), Type::StringArray);
                return Type::StringArray;
            } else {
                throw std::runtime_error("fn: can't derive sub type");
            }
        }

        Type getReturnType(Function &func,
                           std::string const & returnSymbol,
                           SharedCacheStack & sharedCache)
        {
            std::string type;
            (void)func.getValueA<std::string>(type, sharedCache);
            if(type == "int") {
                // Fixing scoping issue by setting a default value here
                (void)sharedCache->setVar(returnSymbol, (int64_t)0, Type::Int);
                return Type::Int;
            } else if(type == "real") {
                (void)sharedCache->setVar(returnSymbol, (double)0, Type::Real);
                return Type::Real;
            } else if(type == "string") {
                (void)sharedCache->setVar(returnSymbol, std::string(""), Type::String);
                return Type::String;
            } else if(type == "bool") {
                (void)sharedCache->setVar(returnSymbol, false, Type::Bool);
                return Type::Bool;
            } else if(type == "list") {
                (void)sharedCache->setVar(returnSymbol, List(), Type::List);
                return Type::List;
            } else if(type == "nil") {
                return Type::None;
            } else {
                return getForArrayTypes(type, returnSymbol, sharedCache);
            }
        }

    }

    ReturnableCommand::ReturnableCommand(Function &func_,
                                         SharedCacheStack const &sharedCache,
                                         OptionalOutputStream const &output)
        : Command(func_, std::make_shared<CacheStack>(), output)
        , m_functionName()
        , m_returnSymbol()
        , m_returnType(getReturnType(func_, m_returnSymbol, m_sharedCache))
    {
        if(m_returnType != Type::None) {
            (void)m_func.getValueD<std::string>(m_returnSymbol, m_sharedCache);



        }
        (void)m_func.getValueB<std::string>(m_functionName, m_sharedCache);    
    }

    ReturnableCommand::~ReturnableCommand() = default;

    std::vector<std::string> ReturnableCommand::getCommandNames()
    {
        return {"fn","fnfn"};
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
