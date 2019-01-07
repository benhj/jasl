//
//  FileAppendBytesCommand.cpp
//  jasl
//
//  Copyright (c) 2017 Ben Jones. All rights reserved.
//

#include "FileAppendBytesCommand.hpp"
#include "IOFunctions.hpp"
#include "caching/VarExtractor.hpp"
#include "core/RegisterCommand.hpp"
#include <boost/filesystem.hpp>
#include <fstream>

bool jasl::FileAppendBytesCommand::m_registered = 
registerCommand<jasl::FileAppendBytesCommand>();

namespace jasl
{

    FileAppendBytesCommand::FileAppendBytesCommand(Function &func_,
                                                   SharedCacheStack const &sharedCache,
                                                   OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {
    }

    std::vector<std::string> FileAppendBytesCommand::getCommandNames()
    {
        return {"file_append_bytes"};
    }

    bool FileAppendBytesCommand::execute() 
    { 
         std::string filename;
        if(VarExtractor::trySingleStringExtraction(m_func.paramB, filename, m_sharedCache)) {

            ByteArray value;
            if(!VarExtractor::trySingleArrayExtraction(m_func.paramA, value, m_sharedCache, Type::ByteArray)) {
                setLastErrorMessage("file_append_bytes: problem with bytes array");
                return false;
            }
            // open the file and write data
            writeBytes(filename, 
                       reinterpret_cast<char const*>(&value.front()), 
                       value.size(),
                       std::ios::binary | std::ios::app);
        } else {
            setLastErrorMessage("file_append_bytes: couldn't parse filename");
            return false;
        }
        return true;
    }
}