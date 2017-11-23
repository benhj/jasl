//
//  FolderListRecursiveCommand.cpp
//  jasl
//
//  Copyright (c) 2017 Ben Jones. All rights reserved.
//

#include "FolderListRecursiveCommand.hpp"
#include "caching/VarExtractor.hpp"
#include <boost/filesystem.hpp>

namespace jasl
{

    FolderListRecursiveCommand::FolderListRecursiveCommand(Function &func_,
                                                           SharedCacheStack const &sharedCache,
                                                           OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {

    }

    bool FolderListRecursiveCommand::execute()
    {
        // now try and extract the actual words
        std::string folderName;
        if(VarExtractor::trySingleStringExtraction(m_func.paramA, folderName, m_sharedCache)) {
            std::string stringArrayName;
            if(!m_func.getValueB<std::string>(stringArrayName, m_sharedCache)) {
                setLastErrorMessage("folder_list_recursive: couldn't parse name");
                return false;
            }

            StringArray sa;
            using ::boost::filesystem::recursive_directory_iterator;
            auto path = ::boost::filesystem::path(folderName);
            recursive_directory_iterator it(path), end;
            while(it != end) {
                auto const filename = it->path().filename();
                if(filename == "." || filename == "..") { continue; }
                sa.emplace_back(it->path().string());
                ++it;
            }
            m_sharedCache->setVar(stringArrayName, sa, Type::StringArray);

        } else {
            setLastErrorMessage("folder_list_recursive: unknown folder");
            return false;
        }

        return true;
    }
}
