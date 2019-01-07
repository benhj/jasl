//
//  Command.hpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015-2016 Ben Jones. All rights reserved.
//

#pragma once

#include "core/Function.hpp"
#include "caching/CacheStack.hpp"
#include <boost/optional.hpp>
#include <ostream>
#include <string>

namespace jasl
{
    class Command
    {
      public:
        /// for capturing any output
        using OptionalOutputStream = ::boost::optional<std::ostream&>;

        Command(Function func_,
                SharedCacheStack const &sharedCache,
                OptionalOutputStream const &output = OptionalOutputStream());

        virtual bool execute() = 0;

      protected:
        Function m_func;

        /// Cache of variables
        SharedCacheStack m_sharedCache;

        /// for optionally capturing output
        ::boost::optional<std::ostream&> m_outputStream;

        template <typename T>
        void appendToOutput(T const &message) 
        {
            if(m_outputStream) {
                *m_outputStream << message;
            }
        }

        template <typename T>
        void appendToOutputWithNewLine(T const &message) 
        {
            if(m_outputStream) {
                *m_outputStream << message << std::endl;
            }
        }

        void setLastErrorMessage(std::string const &error);

    };

}