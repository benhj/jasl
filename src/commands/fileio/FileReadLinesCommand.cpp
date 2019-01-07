//
//  FileReadLinesCommand.cpp
//  jasl
//
//  Copyright (c) 2016 Ben Jones. All rights reserved.
//

#include "FileReadLinesCommand.hpp"
#include "caching/VarExtractor.hpp"
#include "core/RegisterCommand.hpp"
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

#include <fstream>
#include <string>

namespace jasl
{

    FileReadLinesCommand::FileReadLinesCommand(Function &func_,
                                               SharedCacheStack const &sharedCache,
                                               OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {

    }

    bool FileReadLinesCommand::execute() 
    {
        // now try and extract the actual words
        std::string filename;
        if(VarExtractor::trySingleStringExtraction(m_func.paramA, filename, m_sharedCache)) {
            std::string stringArrayName;
            if(!m_func.getValueB<std::string>(stringArrayName, m_sharedCache)) {
                setLastErrorMessage("file_read_lines: couldn't parse name");
                return false;
            }

            // reserve capacity
            StringArray sa;

            // read the data:
            std::ifstream input(filename);
            for( std::string line; getline( input, line ); )
            {
                sa.push_back(line);
            }

            m_sharedCache->setVar(stringArrayName, sa, Type::StringArray);

        } else {
            setLastErrorMessage("file_read_lines: unknown file");
            return false;
        }
        
        return true;
    }
}