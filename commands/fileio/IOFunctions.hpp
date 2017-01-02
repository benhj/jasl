//
//  IOFunctions.hpp
//  jasl
//
//  Copyright (c) 2017 Ben Jones. All rights reserved.
//

#include "../../caching/VarExtractor.hpp"
#include <boost/filesystem.hpp>
#include <fstream>
#include <string>

namespace jasl
{
    inline
    void writeBytes(std::string const & filename, 
                    ByteArray const & bytes,
                    std::ios_base::openmode const mode = std::ios::binary)
    {
        // open the file and write data
        std::ofstream file(filename, mode);
        auto const fileSize = bytes.size();
        file.write(reinterpret_cast<char const*>(&bytes.front()), fileSize);
        file.close();
    }


}