//
//  ArrayGetCommand.cpp
//  jasl
//
//  Created by Ben Jones on 30/06/15
//  Copyright (c) 2016-present Ben Jones. All rights reserved.
//

#include "ArrayGetCommand.hpp"
#include "caching/VarExtractor.hpp"
#include "core/RegisterCommand.hpp"

bool jasl::ArrayGetCommand::m_registered = 
registerCommand<jasl::ArrayGetCommand>();

namespace jasl
{
    ArrayGetCommand::ArrayGetCommand(Function &func_,
                                     SharedCacheStack const &sharedCache,
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
    
    ArrayGetCommand::~ArrayGetCommand() = default;

    std::vector<std::string> ArrayGetCommand::getCommandNames()
    {
        return {"get"};
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
            auto value = m_sharedCache->getArrayValue<IntArray>(m_arrayName, index, Type::IntArray);
            if(value) {
                m_sharedCache->setVar(m_varName, *value, Type::Int);
                return true;
            }
        }
        {
            auto value = m_sharedCache->getArrayValue<RealArray>(m_arrayName, index, Type::RealArray);
            if(value) {
                m_sharedCache->setVar(m_varName, *value, Type::Real);
                return true;
            }
        }
        {
            auto value = m_sharedCache->getArrayValue<ByteArray>(m_arrayName, index, Type::ByteArray);
            if(value) {
                m_sharedCache->setVar(m_varName, *value, Type::Byte);
                return true;
            }
        }
        {
            auto value = m_sharedCache->getArrayValue<StringArray>(m_arrayName, index, Type::StringArray);
            if(value) {
                m_sharedCache->setVar(m_varName, *value, Type::String);
                return true;
            }
        }
        {
            auto value = m_sharedCache->getArrayValue<List>(m_arrayName, index, Type::List);
            if(value) {
                // Value could be a string or a list
                {
                    std::string element;
                    if(VarExtractor::tryAnyCast(element, *value)) {
                        m_sharedCache->setVar(m_varName, element, Type::String);
                        return true;
                    }
                }
                {
                    List element;
                    if(VarExtractor::tryAnyCast(element, *value)) {
                        m_sharedCache->setVar(m_varName, element, Type::List);
                        return true;
                    }
                }
            }
        }

        setLastErrorMessage("get: problem getting array or list value");
        return false;
    }

    bool ArrayGetCommand::getIndex(int64_t &value)
    {
        return VarExtractor::trySingleIntExtraction(m_func.paramB, value, m_sharedCache);
    }

}
