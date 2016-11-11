//
//  FileReadBytesCommand.cpp
//  jasl
//
//  Copyright (c) 2016 Ben Jones. All rights reserved.
//

#include "FileReadBytesCommand.hpp"
#include "../../caching/VarExtractor.hpp"
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

#include <fstream>
#include <algorithm>

namespace jasl
{

    FileReadBytesCommand::FileReadBytesCommand(Function &func_,
                                               SharedCacheStack const &sharedCache,
                                               OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {

    }

    bool FileReadBytesCommand::execute() 
    {
        // now try and extract the actual words
        std::string filename;
        if(VarExtractor::trySingleStringExtraction(m_func.paramA, filename, m_sharedCache)) {
            std::string bytesArrayName;
            if(!m_func.getValueB<std::string>(bytesArrayName, m_sharedCache)) {
                setLastErrorMessage("file_read_bytes: couldn't parse name");
                return false;
            }

            // open the file:
            std::ifstream file(filename, std::ios::binary);

            // Stop eating new lines in binary mode!!!
            file.unsetf(std::ios::skipws);

            // get its size:
            std::streampos fileSize;

            file.seekg(0, std::ios::end);
            fileSize = file.tellg();
            file.seekg(0, std::ios::beg);

            // reserve capacity
            ByteArray ba;
            ba.reserve(fileSize);

            // read the data:
            ba.insert(std::begin(ba), 
                      std::istream_iterator<uint8_t>(file),
                      std::istream_iterator<uint8_t>());

            m_sharedCache->setVar(bytesArrayName, ba, Type::ByteArray);

        } else {
            setLastErrorMessage("file_read_bytes: unknown file");
            return false;
        }
        
        return true;
    }
}