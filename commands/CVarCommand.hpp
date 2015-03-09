#pragma once

#include "Command.hpp"
#include "../VarCache.hpp"

namespace lightlang {
    class CVarCommand : public Command
    {
    public:
        CVarCommand(Function &func_,
                    OptionalOutputStream const &output = OptionalOutputStream())
        : Command(func_, output)
        {
        }

        bool execute() override
        {

            std::string type = m_func.name;
            std::string varName; 
            (void)m_func.getValueA<std::string>(varName);

            if (type == "int") {

                return handleInt(varName);

            } else if (type == "double") {

                return handleDouble(varName);

            } else if (type == "bool") {

                return handleBool(varName);

            } 

            m_errorMessage = "cvar: type not supported";
            appendToOutput(m_errorMessage);
            return false;
        }

    private:
        bool handleInt(std::string const &varName)
        {
            auto a = VarExtractor::trySingleIntExtraction(m_func.paramB);
            if (!a) {
                return false;
            } 
            VarCache::intCache[varName] = *a;
            return true;
        }

        bool handleDouble(std::string const &varName)
        {
            auto a = VarExtractor::trySingleDoubleExtraction(m_func.paramB);
            if (!a) {
                return false;
            } 

            VarCache::doubleCache[varName] = *a;
            return true;
        }

        bool handleBool(std::string const &varName)
        {
            auto a = VarExtractor::trySingleBoolExtraction(m_func.paramB);
            if (!a) {
                return false;
            } 

            VarCache::boolCache[varName] = *a;
            return true;
        }
    };
}

