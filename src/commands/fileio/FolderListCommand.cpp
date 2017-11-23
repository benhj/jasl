//
//  FolderListCommand.cpp
//  jasl
//
//  Copyright (c) 2016 Ben Jones. All rights reserved.
//

#include "FolderListCommand.hpp"
#include "caching/VarExtractor.hpp"
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

namespace jasl
{

    FolderListCommand::FolderListCommand(Function &func_,
                                         SharedCacheStack const &sharedCache,
                                         OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {

    }

    bool FolderListCommand::execute()
    {
        // now try and extract the actual words
        std::string folderName;
        if(VarExtractor::trySingleStringExtraction(m_func.paramA, folderName, m_sharedCache)) {
            std::string stringArrayName;
            if(!m_func.getValueB<std::string>(stringArrayName, m_sharedCache)) {
                setLastErrorMessage("folder_list: couldn't parse name");
                return false;
            }

            StringArray sa;
            using ::boost::filesystem::directory_iterator;
            auto path = ::boost::filesystem::path(folderName);
            for(auto & entry : ::boost::make_iterator_range(directory_iterator(path), {})) {
                auto const pathString = entry.path().string();
                if(pathString == "." || pathString == "..") { continue; }
                sa.emplace_back(pathString);
            }

            m_sharedCache->setVar(stringArrayName, sa, Type::StringArray);

        } else {
            setLastErrorMessage("folder_list: unknown folder");
            return false;
        }

        return true;
    }
}
