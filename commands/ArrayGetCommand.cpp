//
//  ArrayGetCommand.cpp
//  jasl
//
//  Created by Ben Jones on 30/06/15
//  Copyright (c) 2016 Ben Jones. All rights reserved.
//

#include "ArrayGetCommand.hpp"
#include "../VarExtractor.hpp"

namespace jasl
{
    ArrayGetCommand::ArrayGetCommand(Function &func_,
                                     SharedVarCache const &sharedCache,
                                     OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    , m_arrayName()
    , m_varName()
    , m_type()
    {
        // get q(0) -> a;
        (void)m_func.getValueA<std::string>(m_arrayName, m_sharedCache);
        (void)m_func.getValueC<std::string>(m_varName, m_sharedCache);
    }

    bool ArrayGetCommand::execute()
    {
        int64_t index;
        if(!getIndex(index)) {
            setLastErrorMessage("get: problem getting array index");
            return false;
        }

        // try int and then double
        {
            auto value = m_sharedCache->getIntArrayValue(m_arrayName, index);
            if(value) {
                m_sharedCache->setInt(m_varName, *value);
                return true;
            }
        }
        {
            auto value = m_sharedCache->getDoubleArrayValue(m_arrayName, index);
            if(value) {
                m_sharedCache->setDouble(m_varName, *value);
                return true;
            }
        }
        {
            auto value = m_sharedCache->getByteArrayValue(m_arrayName, index);
            if(value) {
                m_sharedCache->setByte(m_varName, *value);
                return true;
            }
        }

        setLastErrorMessage("get: problem getting array value");
        return false;
    }

    bool ArrayGetCommand::getIndex(int64_t &value)
    {
        return VarExtractor::trySingleIntExtraction(m_func.paramB, value, m_sharedCache);
    }

}