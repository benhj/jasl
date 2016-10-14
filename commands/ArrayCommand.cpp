//
//  ArrayCommand.cpp
//  jasl
//
//  Created by Ben Jones on 30/06/16
//  Copyright (c) 2016 Ben Jones. All rights reserved.
//

#include "ArrayCommand.hpp"
#include "../VarExtractor.hpp"

namespace jasl
{

    ArrayCommand::ArrayCommand(Function &func_,
                               SharedVarCache const &sharedCache,
                               OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    , m_type()
    , m_varName()
    {
        // array:int(5) -> a;
        // get type and var name
        (void)m_func.getValueA<std::string>(m_type, m_sharedCache);
        (void)m_func.getValueC<std::string>(m_varName, m_sharedCache);
    }

    bool ArrayCommand::execute() 
    {
        // Find out how big the array should be
        int64_t value;
        if (!VarExtractor::trySingleIntExtraction(m_func.paramB, value, m_sharedCache)) {
            // try converting a string to an int
            setLastErrorMessage("array: couldn't get index");
            return false;
        } 

        if (m_type == "int") {

            IntArray array(value, 0);
            m_sharedCache->setIntArray(m_varName, array);
            return true;
            
        } else if (m_type == "real") {

            DoubleArray array(value, 0.0);
            m_sharedCache->setDoubleArray(m_varName, array);
            return true;

        } else if (m_type == "byte") {

            ByteArray array(value, 0.0);
            m_sharedCache->setByteArray(m_varName, array);
            return true;
            
        }

        setLastErrorMessage("array: type not supported");
        return false;
    }
}