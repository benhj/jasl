//
//  NewPrimitiveSyntaxCommand.cpp
//  jasl
//
//  Created by Ben Jones on 08/11/15
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#include "NewPrimitiveSyntaxCommand.hpp"
#include "StringToPrimitiveCommand.hpp"

namespace jasl {

    NewPrimitiveSyntaxCommand::NewPrimitiveSyntaxCommand(Function &func_,
                                                         SharedVarCache const &sharedCache,
                                                         OptionalOutputStream const &output)
    : Command(func_, std::move(sharedCache), std::move(output))
    {
    }

    bool NewPrimitiveSyntaxCommand::execute()
    {

        std::string type = m_func.name;
        std::string varName; 
        (void)m_func.getValueB<std::string>(varName, m_sharedCache);

        if (type == "integer") {

            return handleInt(varName);

        } else if (type == "decimal") {

            return handleDouble(varName);

        } else if (type == "boolean") {

            return handleBool(varName);

        } 

        setLastErrorMessage("cvar: type not supported");
        return false;
    }

    bool NewPrimitiveSyntaxCommand::handleInt(std::string const &varName)
    {
        auto a = VarExtractor::trySingleIntExtraction(m_func.paramA, m_sharedCache);
        if (!a) {
            // try converting a string to an integer
            return StringToPrimitiveCommand(m_func, m_sharedCache, m_outputStream).execute();
        } 
        m_sharedCache->setInt(varName, *a);
        

        return true;
    }

    bool NewPrimitiveSyntaxCommand::handleDouble(std::string const &varName)
    {
        auto a = VarExtractor::trySingleDoubleExtraction(m_func.paramA, m_sharedCache);
        if (!a) {
            // try converting a string to a double
            return StringToPrimitiveCommand(m_func, m_sharedCache, m_outputStream).execute();
        } 

        m_sharedCache->setDouble(varName, *a);
        

        return true;
    }

    bool NewPrimitiveSyntaxCommand::handleBool(std::string const &varName)
    {
        auto a = VarExtractor::trySingleBoolExtraction(m_func.paramA, m_sharedCache);
        if (!a) {
            return false;
        } 

        m_sharedCache->setBool(varName, *a);
      
        return true;
    }
}