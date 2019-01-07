//
//  FileTypeCommand.cpp
//  jasl
//
//  Copyright (c) 2017 Ben Jones. All rights reserved.
//

#include "FileTypeCommand.hpp"
#include "caching/VarExtractor.hpp"
#include "core/RegisterCommand.hpp"
#include <boost/filesystem.hpp>

bool jasl::FileTypeCommand::m_registered = 
registerCommand<jasl::FileTypeCommand>();

namespace jasl
{

    FileTypeCommand::FileTypeCommand(Function &func_,
                                     SharedCacheStack const &sharedCache,
                                     OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {
    }

    std::vector<std::string> FileTypeCommand::getCommandNames()
    {
        return {"file_type"};
    }

    bool FileTypeCommand::execute()
    {
        // now try and extract the actual words
        std::string name;
        if(VarExtractor::trySingleStringExtraction(m_func.paramA, name, m_sharedCache)) {
            std::string varName;
            if(!m_func.getValueB<std::string>(varName, m_sharedCache)) {
                setLastErrorMessage("file_type: couldn't parse variable name.");
                return false;
            }

            std::string type;
            namespace fs = ::boost::filesystem;
            auto path = fs::path(name);
            if (fs::is_directory(path)) {
                type = "directory";
            } else if(fs::is_regular_file(path)) {
                type = "file";
            } else {
                type = "unknown";
            }

            m_sharedCache->setVar(varName, type, Type::String);

        } else {
            setLastErrorMessage("file_type: path not found.");
            return false;
        }

        return true;
    }
}
