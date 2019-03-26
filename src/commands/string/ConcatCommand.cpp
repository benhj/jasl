//
//  ConcatCommand.cpp
//  jasl
//
//  Copyright (c) 2016-present Ben Jones. All rights reserved.
//

#include "ConcatCommand.hpp"
#include "caching/VarExtractor.hpp"
#include "core/RegisterCommand.hpp"
#include <sstream>

bool jasl::ConcatCommand::m_registered = 
registerCommand<jasl::ConcatCommand>();

namespace jasl
{
    ConcatCommand::ConcatCommand(Function &func_,
                                 SharedCacheStack const &sharedCache,
                                 OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {
    }

    ConcatCommand::~ConcatCommand() = default;

    std::vector<std::string> ConcatCommand::getCommandNames()
    {
        return {"concat"};
    }

    bool ConcatCommand::execute() 
    {
        std::string stringName;
        if(!m_func.getValueB<std::string>(stringName, m_sharedCache)) {
            setLastErrorMessage("concat: couldn't parse");
            return false;
        }

        if(tryConcatExtraction(stringName)) { return true; }
        
        setLastErrorMessage("concat: couldn't parse");
        return false;
    }

    /// Note, currently, this only works with concatenating strings
    /// from string variables and literals:
    /// concat a,b,c -> str;
    /// concat a,b,"world" -> str;
    /// etc.
    bool ConcatCommand::tryConcatExtraction(std::string const & key)
    {
        ValueArray array;
        if(m_func.getValueA<ValueArray>(array, m_sharedCache)) {
            std::string result("");
            for (auto & v : array) {
                // try extracting symbols representing
                // strings and then concatenate them
                {    
                    {
                        std::string value;
                        if(VarExtractor::trySingleStringExtraction(v, value, m_sharedCache)) {
                            result.append(value);
                            continue;
                        }
                    }
                    {
                        LiteralString value;
                        if(VarExtractor::tryAnyCast<LiteralString>(value, v)) {
                            result.append(value.literal);
                            continue;
                        }
                    }
                    return false;

                }
            }
            m_sharedCache->setVar(key, result, Type::String);
            return true;
        }
        return false;
    }
}
