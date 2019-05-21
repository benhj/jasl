//
//  ForCommand.cpp
//  jasl
//
//  Created by Ben Jones on 03/10/15
//  Copyright (c) 2015-present Ben Jones. All rights reserved.
//

#include "ForCommand.hpp"
#include "caching/WithNewCache.hpp"
#include "commands/list/ListGetTokenCommand.hpp"
#include "commands/types/ArrayGetCommand.hpp"
#include "core/CommandInterpretor.hpp"
#include "core/RegisterCommand.hpp"
#include <string>

bool jasl::ForCommand::m_registered = 
registerCommand<jasl::ForCommand>();

namespace jasl {

    ForCommand::ForCommand(Function &func_,
                           SharedCacheStack const &sharedCache,
                           OptionalOutputStream const &output)
        : Command(func_, withNewCache(sharedCache), output)
    {
    }

    ForCommand::~ForCommand() = default;

    std::vector<std::string> ForCommand::getCommandNames()
    {
        return {"for"};
    }

    bool ForCommand::execute() 
    {

        // try and pull out var list
        {
            std::string listSymbol;
            if(VarExtractor::tryAnyCast(listSymbol, m_func.paramB)) {

                // try list
                {
                    auto list = m_sharedCache->getVar<List>(listSymbol, Type::List);
                    if(list) {
                        return processList(*list);
                    }
                }

                // try int array
                {
                    auto array = m_sharedCache->getVar<IntArray>(listSymbol, Type::IntArray);
                    if(array) {
                        processArray(*array, listSymbol);
                    }
                }

                // try real array
                {
                    auto array = m_sharedCache->getVar<RealArray>(listSymbol, Type::RealArray);
                    if(array) {
                        processArray(*array, listSymbol);
                    }
                }

                // try byte array
                {
                    auto array = m_sharedCache->getVar<ByteArray>(listSymbol, Type::ByteArray);
                    if(array) {
                        processArray(*array, listSymbol);
                    }
                }

                // try string array
                {
                    auto array = m_sharedCache->getVar<StringArray>(listSymbol, Type::StringArray);
                    if(array) {
                        processArray(*array, listSymbol);
                    }
                }
                
                // try string
                {
                    auto str = m_sharedCache->getVar<std::string>(listSymbol, Type::String);
                    if(str) {
                        processString(*str, listSymbol);
                    }
                }

            }
        }
        // try and pull out raw list
        {
            List list;
            if(VarExtractor::tryAnyCast(list, m_func.paramB)) {
                return processList(list);
            }
        }

        // some failure occured
        setLastErrorMessage("for: error");
        return false;
    }

    template <typename T>
    bool ForCommand::processArray(T const &array, std::string const & listSymbol)
    {
        std::string indexSymbol;
        if(VarExtractor::tryAnyCast(indexSymbol, m_func.paramA)) {

            std::vector<Function> innerFuncs;
            bool success = VarExtractor::tryAnyCast<std::vector<Function>>(innerFuncs, m_func.paramC);
            if(!success) {
                return false;
            }

            // Process tokens one by one using get_token command
            CommandInterpretor ci;
            for(int i = 0; i < array.size(); ++i) {
                Function f;
                f.name = "get";
                f.paramA = listSymbol;
                f.paramB = (int64_t)i;
                f.paramC = indexSymbol;
                ArrayGetCommand tc(f, m_sharedCache, m_outputStream);                
                if(tc.execute()) {

                    // do other commands
                    // parse inner functions
                    if (success) {
                        success = parseCommands(innerFuncs);
                    } else {
                        setLastErrorMessage("for: error");
                        return false;
                    }
                }
                // make sure variable storing token is released before iterating
                m_sharedCache->eraseValue(indexSymbol);
            }

        }
        setLastErrorMessage("for: error");
        return false;
    }

    bool ForCommand::processList(List const &va)
    {
        std::string indexSymbol;
        if(VarExtractor::tryAnyCast(indexSymbol, m_func.paramA)) {

            std::vector<Function> innerFuncs;
            bool success = VarExtractor::tryAnyCast<std::vector<Function>>(innerFuncs, m_func.paramC);
            if(!success) {
                return false;
            }

            // Process tokens one by one using get_token command
            CommandInterpretor ci;
            for(int i = 0; i < va.size(); ++i) {

                std::string toStoreString;
                if(VarExtractor::tryAnyCast(toStoreString, va[i])) {
                    m_sharedCache->setVar(indexSymbol, toStoreString, Type::String);
                } else {
                    List list;
                    if(VarExtractor::tryAnyCast(list, va[i])) {
                        m_sharedCache->setVar(indexSymbol, list, Type::List);
                    } else {
                        success = false;
                    }
                }

                // do other commands
                // parse inner functions
                if (success) {
                    (void)parseCommands(innerFuncs);
                    m_sharedCache->eraseValue(indexSymbol);
                } else {
                    setLastErrorMessage("for: error");
                    return false;
                }
            }
            return true;
        }
        setLastErrorMessage("for: error.");
        return false;
    }

    bool ForCommand::processString(std::string const &str, 
                                   std::string const & listSymbol)
    {
        std::string indexSymbol;
        if(VarExtractor::tryAnyCast(indexSymbol, m_func.paramA)) {

            std::vector<Function> innerFuncs;
            bool success = VarExtractor::tryAnyCast<std::vector<Function>>(innerFuncs, m_func.paramC);
            if(!success) {
                return false;
            }

            // Process tokens one by one using get_token command
            CommandInterpretor ci;
            for(auto & v : str) {
                m_sharedCache->setVar(indexSymbol, (uint8_t)v, Type::Byte);
                success = parseCommands(innerFuncs);
                // make sure variable storing token is released before iterating
                m_sharedCache->eraseValue(indexSymbol);
            }

        }
        setLastErrorMessage("for: error");
        return false;
    }

    bool ForCommand::parseCommands(std::vector<Function> &functions) 
    {
        CommandInterpretor ci;
        for(auto & f : functions) {
            (void)ci.interpretFunc(f, m_sharedCache, m_outputStream);
        }
        return true;
    }
}
