//
//  ExecCommand.cpp
//  jasl
//
//  Created by Ben Jones on 25/10/15
//  Copyright (c) 2015-2016 Ben Jones. All rights reserved.
//


#include "ExecCommand.hpp"
#include "caching/VarExtractor.hpp"
#include "core/RegisterCommand.hpp"
#include <sstream>

bool jasl::ExecCommand::m_registered = 
registerCommand<jasl::ExecCommand>();

namespace jasl
{
    ExecCommand::ExecCommand(Function &func_,
                             SharedCacheStack const &sharedCache,
                             OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {

    }

    std::vector<std::string> ExecCommand::getCommandNames()
    {
        return {"exec"};
    }

    bool ExecCommand::execute() 
    {
        if(tryLiteralExtraction()) { return true; }
        if(trySymbolExtraction()) { return true; }
        setLastErrorMessage("exec: couldn't parse");
        return false;
    }


    bool ExecCommand::tryLiteralExtraction() 
    {
        LiteralString literalString;
        if(m_func.getValueA<LiteralString>(literalString, m_sharedCache)) {
            system(literalString.literal.c_str());
            return true;
        }
        return false;
    }

    bool ExecCommand::trySymbolExtraction()
    {
        // Now try extracting a symbol
        std::string symbol;
        if(m_func.getValueA<std::string>(symbol, m_sharedCache)) {
        	auto result = m_sharedCache->getVar<std::string>(symbol, Type::String);
            if(result) {
                system((*result).c_str());
                return true;
            }
        }
        return false;
    }
}

/*
template<class T>
char RegisterType(){
  std::cout<<"exec"<<std::endl;
  return 0; // doesn't matter, never used.
}

// char jasl::Command::_enforce_registration = 
char jasl::Command::_enforce_registration = RegisterType<jasl::ExecCommand>();
*/