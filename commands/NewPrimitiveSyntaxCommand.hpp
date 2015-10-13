//
//  NewPrimitiveSyntaxCommand.hpp
//  jasl
//
//  Created by Ben Jones on 13/03/15
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "Command.hpp"
#include "StringToPrimitiveCommand.hpp"

namespace jasl {
    class NewPrimitiveSyntaxCommand : public Command
    {
    public:
        NewPrimitiveSyntaxCommand(Function &func_,
                                  SharedVarCache const &sharedCache = SharedVarCache(),
                                  OptionalOutputStream const &output = OptionalOutputStream())
        : Command(func_, sharedCache, output)
        {
        }

        bool execute() override
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

    private:
        bool handleInt(std::string const &varName)
        {
            auto a = VarExtractor::trySingleIntExtraction(m_func.paramA, m_sharedCache);
            if (!a) {
                // try converting a string to an integer
                return StringToPrimitiveCommand(m_func, m_sharedCache, m_outputStream).execute();
            } 
            // see if should update global cache, in the case that we
            // have a return statement and return value should go in 
            // to global cache
            if(GlobalCache::getInt(varName)) {
                GlobalCache::setInt(varName, *a);
            }
            m_sharedCache->setInt(varName, *a);
            

            return true;
        }

        bool handleDouble(std::string const &varName)
        {
            auto a = VarExtractor::trySingleDoubleExtraction(m_func.paramA, m_sharedCache);
            if (!a) {
                // try converting a string to a double
                return StringToPrimitiveCommand(m_func, m_sharedCache, m_outputStream).execute();
            } 

            // see if should update global cache, in the case that we
            // have a return statement and return value should go in 
            // to global cache
            if(GlobalCache::getDouble(varName)) {
                GlobalCache::setDouble(varName, *a);
            }
            m_sharedCache->setDouble(varName, *a);
            

            return true;
        }

        bool handleBool(std::string const &varName)
        {
            auto a = VarExtractor::trySingleBoolExtraction(m_func.paramA, m_sharedCache);
            if (!a) {
                return false;
            } 

            // see if should update global cache, in the case that we
            // have a return statement and return value should go in 
            // to global cache
            if(GlobalCache::getBool(varName)) {
                GlobalCache::setBool(varName, *a);
            }
            m_sharedCache->setBool(varName, *a);
            

            return true;
        }
    };
}

