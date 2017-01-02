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
                    char const * bytes,
                    size_t const n,
                    std::ios_base::openmode const mode = std::ios::binary)
    {
        // open the file and write data
        std::ofstream file(filename, mode);
        file.write(bytes, n);
        file.close();
    }


}