//
//  PutPrimitiveCommand.cpp
//  jasl
//
//  Created by Ben Jones on 06/07/16
//  Copyright (c) 2016 Ben Jones. All rights reserved.
//

#include "PutPrimitiveCommand.hpp"
#include "caching/VarExtractor.hpp"
#include "other/CommandInterpretor.hpp"

namespace jasl
{
    PutPrimitiveCommand::PutPrimitiveCommand(Function &func_,
                                             SharedCacheStack const &sharedCache,
                                             OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    , m_type()
    , m_varName()
    {
        // example
        // put 5 -> q;
        // where q is an int, a real, a string or whatever
        (void)m_func.getValueB<std::string>(m_varName, m_sharedCache);

        auto type = sharedCache->getType(m_varName);
        if (*type == Type::Int) {
            m_type = "int";
        } else if (*type == Type::Real) {
            m_type = "real";
        } else if (*type == Type::Bool) {
            m_type = "bool";
        } else if (*type == Type::String) {
            m_type = "string";
        } else if (*type == Type::Byte) {
            m_type = "byte";
        } else if (*type == Type::List) {
            m_type = "list";
        }

        setLastErrorMessage("put: couldn't determine type");
    }

    bool PutPrimitiveCommand::execute() 
    {
        if(m_type.empty()) {
            return false;
        }

        Function primitiveFunc;
        primitiveFunc.name = m_type;
        primitiveFunc.paramA = m_func.paramA;
        primitiveFunc.paramB = m_func.paramB;
        CommandInterpretor ci;
        (void)ci.interpretFunc(primitiveFunc, m_sharedCache, m_outputStream);

        return true;
    }
}