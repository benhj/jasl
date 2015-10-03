//
//  ForCommand.cpp
//  jasl
//
//  Created by Ben Jones on 03/10/15
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#include "ForCommand.hpp"
#include "../CommandInterpretor.hpp"
#include <string>

namespace jasl {

    ForCommand::ForCommand(Function &func_, OptionalOutputStream const &output)
    : Command(func_, output)
    {

    }

    bool ForCommand::execute() 
    {

        // try and pull out var list
        {
            std::string listSymbol;
            if(VarExtractor::tryAnyCast(listSymbol, m_func.paramB)) {
                auto list = VarCache::getList(listSymbol);
                if(list) {
                    return processList(*list, listSymbol);
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
        setLastErrorMessage("for: problem processing list");
        return false;
    }

    bool ForCommand::processList(ValueArray const &va, 
                                 std::string const & listSymbol)
    {
        std::string indexSymbol;
        if(VarExtractor::tryAnyCast(indexSymbol, m_func.paramA)) {
            // Process tokens one by one using get_token command
            CommandInterpretor ci;
            for(int i = 0; i < va.size(); ++i) {
                std::string commandString("get_token(");
                commandString.append(std::to_string(i));
                commandString.append(",");
                commandString.append(listSymbol);
                commandString.append(") -> ");
                commandString.append(indexSymbol);
                commandString.append(";");
                ci.parseAndInterpretSingleCommand(commandString);

                // do other commands
                // parse inner functions
                std::vector<Function> innerFuncs;
                bool success = VarExtractor::tryAnyCast<std::vector<Function>>(innerFuncs, m_func.paramC);
                if (success) {
                    success = parseCommands(innerFuncs);
                } else {
                    setLastErrorMessage("repeat: Error interpreting while's body");
                    return false;
                }

                // make sure variable storing token is released before iterating
                std::string releaseString("release ");
                releaseString.append(indexSymbol);
                releaseString.append(";");
                ci.parseAndInterpretSingleCommand(releaseString);
            }

        }
        setLastErrorMessage("for: problem getting index symbol");
        return false;
    }

    bool ForCommand::parseCommands(std::vector<Function> &functions) 
    {
        CommandInterpretor ci;
        for(auto & f : functions) {
            (void)ci.interpretFunc(f, m_outputStream);
        }
        return true;
    }
}