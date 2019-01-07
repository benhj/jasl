//
//  PutCommand.cpp
//  jasl
//
//  Created by Ben Jones on 06/07/15
//  Copyright (c) 2016 Ben Jones. All rights reserved.
//

#include "PutCommand.hpp"
#include "ArrayPutCommand.hpp"
#include "PutPrimitiveCommand.hpp"
#include "caching/VarExtractor.hpp"
#include "core/RegisterCommand.hpp"

namespace jasl
{
    PutCommand::PutCommand(Function &func_,
                           SharedCacheStack const &sharedCache,
                           OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    , m_primitive(false)
    {
        // examples
        // put 5 -> q(0);
        // put 1.1 -> d;
        std::string varName;
        (void)m_func.getValueB<std::string>(varName, m_sharedCache);
        auto type = sharedCache->getType(varName);
        if(!type) {
            setLastErrorMessage("put: couldn't determine type");
        } else if(!((*type == Type::IntArray)  || 
                    (*type == Type::RealArray) || 
                    (*type == Type::ByteArray) ||
                    (*type == Type::StringArray)))  {
            m_primitive = true;
        }
    }

    bool PutCommand::execute() 
    {

        Function specialistPut;
        specialistPut.name = "put";
        specialistPut.paramA = m_func.paramA;
        specialistPut.paramB = m_func.paramB;

        if(!m_primitive) {
            specialistPut.paramC = m_func.paramC;
            ArrayPutCommand apc(specialistPut, m_sharedCache, m_outputStream);
            if(!apc.execute()) {
                setLastErrorMessage("put: Error putting in to array");
                return false;
            }
        } else {
            PutPrimitiveCommand ppc(specialistPut, m_sharedCache, m_outputStream);
            if(!ppc.execute()) {
                setLastErrorMessage("put: Error putting in to primitive");
                return false;
            }
        }

        return true;
    }
}