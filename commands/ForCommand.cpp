//
//  ForCommand.cpp
//  jasl
//
//  Created by Ben Jones on 03/10/15
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#include "ForCommand.hpp"
#include "ArrayGetCommand.hpp"
#include "ListGetTokenCommand.hpp"
#include "ReleaseCommand.hpp"
#include "../CommandInterpretor.hpp"
#include <string>

namespace jasl {

    ForCommand::ForCommand(Function &func_,
                           SharedVarCache const &sharedCache,
                           OptionalOutputStream const &output)
        : Command(func_, sharedCache, output)
    {

    }

    bool ForCommand::execute() 
    {

        // try and pull out var list
        {
            std::string listSymbol;
            if(VarExtractor::tryAnyCast(listSymbol, m_func.paramB)) {

                // try list
                {
                    auto list = m_sharedCache->getList(listSymbol);
                    if(list) {
                        return processList(*list, listSymbol);
                    }
                }

                // try int array
                {
                    auto array = m_sharedCache->getIntArray(listSymbol);
                    if(array) {
                        processArray(*array, listSymbol);
                    }
                }

                // try real array
                {
                    auto array = m_sharedCache->getDoubleArray(listSymbol);
                    if(array) {
                        processArray(*array, listSymbol);
                    }
                }
            }
        }
        // try and pull out raw list
        // {
        //     ValueArray list;
        //     if(VarExtractor::tryAnyCast(list, m_func.paramB)) {
        //         return processList(list);
        //     }
        // }

        // some failure occured
        setLastErrorMessage("for: problem processing elements");
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
                        setLastErrorMessage("repeat: Error interpreting for's body");
                        return false;
                    }
                }

                // make sure variable storing token is released before iterating
                Function relFunc;
                relFunc.name = "release";
                relFunc.paramA = indexSymbol;
                ReleaseCommand rc(relFunc, m_sharedCache, m_outputStream);
                (void)rc.execute();
            }

        }
        setLastErrorMessage("for: problem getting index symbol");
        return false;
    }

    bool ForCommand::processList(ValueArray const &va, 
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
            for(int i = 0; i < va.size(); ++i) {
                Function f;
                f.name = "get_token";
                f.paramA = (int64_t)i;
                f.paramB = listSymbol;
                f.paramC = indexSymbol;
                ListGetTokenCommand tc(f, m_sharedCache, m_outputStream);                
                if(tc.execute()) {

                    // do other commands
                    // parse inner functions
                    if (success) {
                        success = parseCommands(innerFuncs);
                    } else {
                        setLastErrorMessage("repeat: Error interpreting for's body");
                        return false;
                    }

                }

                // make sure variable storing token is released before iterating
                Function relFunc;
                relFunc.name = "release";
                relFunc.paramA = indexSymbol;
                ReleaseCommand rc(relFunc, m_sharedCache, m_outputStream);
                (void)rc.execute();
            }

        }
        setLastErrorMessage("for: problem getting index symbol");
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