#include "RepeatCommand.hpp"
#include "../CommandInterpretor.hpp"
#include <string>

namespace lightlang {

    RepeatCommand::RepeatCommand(Function &func_, OptionalOutputStream const &output)
    : Command(func_, output)
    {

    }

    bool RepeatCommand::execute() 
    {

        // how many time should repeat loop for?
        auto extracted = VarExtractor::trySingleIntExtraction(m_func.paramA);
        if(!extracted) {
            m_errorMessage = "repeat: problem extracting integer";
            appendToOutput(m_errorMessage);
            return false;
        }
        return doLoop(*extracted);
    }

    bool RepeatCommand::doLoop(int const loopCount)
    {
        for (int loop = 0; loop < loopCount; ++loop) {

            // parse commands here
            std::vector<Function> innerFuncs;
            bool success = VarExtractor::tryAnyCast<std::vector<Function>>(innerFuncs, m_func.paramB);
            if (success) {
                success = parseCommands(innerFuncs);
            } else {
                m_errorMessage = "repeat: Error interpreting repeat's body";
                appendToOutput(m_errorMessage);
                return false;
            }
        }
        return true;
    }

    bool RepeatCommand::parseCommands(std::vector<Function> &functions) 
    {
        CommandInterpretor ci;
        for(auto & f : functions) {
            (void)ci.interpretFunc(f, m_outputStream);
        }
        return true;
    }
}