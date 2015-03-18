//
//  Command.hpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "../Function.hpp"
#include "../VarCache.hpp"
#include <boost/optional.hpp>
#include <ostream>
#include <string>

namespace jasl
{
    class Command
    {
    public:
        /// for capturing any output
        typedef ::boost::optional<std::ostream&> OptionalOutputStream;

        Command(Function &func_,
                OptionalOutputStream const &output = OptionalOutputStream())
        : m_func(func_) 
        , m_outputStream(output)
        {
        }

        virtual bool execute() = 0;

    protected:
        Function &m_func;

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

        void setLastErrorMessage(std::string const &error)
        {
            VarCache::lastKnownError = error;
        }

    };

}