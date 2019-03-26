//
//  FileAppendStringCommand.cpp
//  jasl
//
//  Copyright (c) 2017 Ben Jones. All rights reserved.
//

#include "FileAppendStringCommand.hpp"
#include "IOFunctions.hpp"
#include "caching/VarExtractor.hpp"
#include "core/RegisterCommand.hpp"
#include <boost/filesystem.hpp>
#include <fstream>
#include <string>

bool jasl::FileAppendStringCommand::m_registered = 
registerCommand<jasl::FileAppendStringCommand>();

namespace jasl
{

    FileAppendStringCommand::FileAppendStringCommand(Function &func_,
                                                     SharedCacheStack const &sharedCache,
                                                     OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {
    }
    
    FileAppendStringCommand::~FileAppendStringCommand() = default;
    
    std::vector<std::string> FileAppendStringCommand::getCommandNames()
    {
        return {"file_append_line"};
    }

    bool FileAppendStringCommand::execute() 
    { 
        std::string filename;
        if(VarExtractor::trySingleStringExtraction(m_func.paramB, filename, m_sharedCache)) {

            std::string value;
            if(!VarExtractor::trySingleStringExtraction(m_func.paramA, value, m_sharedCache)) {
                setLastErrorMessage("file_append_line: problem with string");
                return false;
            }
            // open the file and write data
            writeBytes(filename, 
                       reinterpret_cast<char const*>(&value.front()), 
                       value.size(),
                       std::ios::binary | std::ios::app);
        } else {
            setLastErrorMessage("file_append_line: couldn't parse filename");
            return false;
        }
        return true;
    }
}
