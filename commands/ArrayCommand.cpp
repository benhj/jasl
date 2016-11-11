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
        // OR
        // ints 5 -> a;
        // reals 5 -> a;
        // etc.
        // get type and var name
        if(m_func.name != "array") {
            m_type = m_func.name;
            (void)m_func.getValueB<std::string>(m_varName, m_sharedCache);
        } else {
            (void)m_func.getValueA<std::string>(m_type, m_sharedCache);
            (void)m_func.getValueC<std::string>(m_varName, m_sharedCache);
        }

        
    }

    bool ArrayCommand::execute() 
    {
        // Find out how big the array should be
        auto & param = (m_func.name == "array" ? m_func.paramB : m_func.paramA);
        int64_t value;
        if (!VarExtractor::trySingleIntExtraction(param, 
                                                  value, 
                                                  m_sharedCache)) {
            // might be that we have array:byte str -> bytes; type syntax.
            // this is for when we implicitly convert a string to a byte array
            std::string str;
            if (VarExtractor::trySingleStringExtraction(param, str, m_sharedCache)) {
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

        if (m_type == "int" || m_type == "ints") {

            IntArray array(value, 0);
            m_sharedCache->setVar(m_varName, array, Type::IntArray);
            return true;
            
        } else if (m_type == "real" || m_type == "reals") {

            RealArray array(value, 0.0);
            m_sharedCache->setVar(m_varName, array, Type::RealArray);
            return true;

        } else if (m_type == "byte" || m_type == "bytes") {

            ByteArray array(value, 0.0);
            m_sharedCache->setVar(m_varName, array, Type::ByteArray);
            return true;

        } else if (m_type == "string" || m_type == "strings") {

            StringArray array(value, "");
            m_sharedCache->setVar(m_varName, array, Type::StringArray);
            return true;

        }

        setLastErrorMessage("array: type not supported");
        return false;
    }
}