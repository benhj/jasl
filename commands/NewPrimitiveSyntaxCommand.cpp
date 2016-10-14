//
//  NewPrimitiveSyntaxCommand.cpp
//  jasl
//
//  Created by Ben Jones on 08/11/15
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#include "NewPrimitiveSyntaxCommand.hpp"
#include "StringToPrimitiveCommand.hpp"

#include <iostream>

namespace jasl {

    NewPrimitiveSyntaxCommand::NewPrimitiveSyntaxCommand(Function &func_,
                                                         SharedVarCache const &sharedCache,
                                                         OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    , m_type(func_.name)
    , m_varName()
    {
        (void)m_func.getValueB<std::string>(m_varName, m_sharedCache);
    }

    bool NewPrimitiveSyntaxCommand::execute()
    {

        if (m_type == "int") {
            return handleInt();
        } else if (m_type == "real") {
            return handleDouble();
        } else if (m_type == "bool") {
            return handleBool();
        } else if (m_type == "byte") {
            return handleByte();
        } 

        setLastErrorMessage("cvar: type not supported");
        return false;
    }

    bool NewPrimitiveSyntaxCommand::handleInt()
    {
        int64_t value;
        if (!VarExtractor::trySingleIntExtraction(m_func.paramA, value, m_sharedCache)) {
            // try converting a string to an int
            return StringToPrimitiveCommand(m_func, m_sharedCache, m_outputStream).execute();
        } 
        m_sharedCache->setInt(m_varName, value);
        return true;
    }

    bool NewPrimitiveSyntaxCommand::handleByte()
    {
        uint8_t value;
        if (!VarExtractor::trySingleByteExtraction(m_func.paramA, value, m_sharedCache)) {
            return StringToPrimitiveCommand(m_func, m_sharedCache, m_outputStream).execute();
        } 

        m_sharedCache->setByte(m_varName, value);
        return true;
    }

    bool NewPrimitiveSyntaxCommand::handleDouble()
    {
        double value;
        if (!VarExtractor::trySingleDoubleExtraction(m_func.paramA, value, m_sharedCache)) {
            // try converting a string to a double
            return StringToPrimitiveCommand(m_func, m_sharedCache, m_outputStream).execute();
        } 

        m_sharedCache->setDouble(m_varName, value);
        
        return true;
    }

    bool NewPrimitiveSyntaxCommand::handleBool()
    {
        bool value;
        if (!VarExtractor::trySingleBoolExtraction(m_func.paramA, value, m_sharedCache)) {
            return false;
        } 

        m_sharedCache->setBool(m_varName, value);
        return true;
    }
}