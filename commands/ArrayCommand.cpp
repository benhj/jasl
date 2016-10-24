//
//  ArrayCommand.cpp
//  jasl
//
//  Created by Ben Jones on 30/06/16
//  Copyright (c) 2016 Ben Jones. All rights reserved.
//

#include "ArrayCommand.hpp"
#include "../caching/VarExtractor.hpp"

namespace jasl
{

    ArrayCommand::ArrayCommand(Function &func_,
                               SharedCacheStack const &sharedCache,
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
            // might be that we have array:byte str -> bytes; type syntax.
            // this is for when we implicitly convert a string to a byte array
            std::string str;
            if (VarExtractor::trySingleStringExtraction(m_func.paramB, str, m_sharedCache)) {
                ByteArray array;
                for (auto const & c : str) {
                    array.push_back(c);
                }
                m_sharedCache->setVar(m_varName, array, Type::ByteArray);
                return true;
            }
            setLastErrorMessage("array: couldn't get index");
            return false;
        } 

        if (m_type == "int") {

            IntArray array(value, 0);
            m_sharedCache->setVar(m_varName, array, Type::IntArray);
            return true;
            
        } else if (m_type == "real") {

            RealArray array(value, 0.0);
            m_sharedCache->setVar(m_varName, array, Type::RealArray);
            return true;

        } else if (m_type == "byte") {

            ByteArray array(value, 0.0);
            m_sharedCache->setVar(m_varName, array, Type::ByteArray);
            return true;

        } else if (m_type == "string") {

            StringArray array(value, "");
            m_sharedCache->setVar(m_varName, array, Type::StringArray);
            return true;

        }

        setLastErrorMessage("array: type not supported");
        return false;
    }
}