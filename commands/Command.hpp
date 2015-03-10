//
//  Command.hpp
//  lightlang
//
//  Created by Ben Jones 
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "../Function.hpp"
#include <boost/optional.hpp>
#include <ostream>
#include <string>

namespace lightlang
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
        , m_errorMessage("")
        {
        }

        std::string getErrorMessage() const 
        {
            return m_errorMessage;
        }

        virtual bool execute() = 0;

    protected:
        Function &m_func;

        /// for optionally capturing output
        ::boost::optional<std::ostream&> m_outputStream;

        /// for setting an error message that can be later queried
        std::string m_errorMessage;

        void appendToOutput(std::string const &message) 
        {
            if(m_outputStream) {
                *m_outputStream << message.c_str();
            }
        }

        void appendToOutputWithNewLine(std::string const &message) 
        {
            if(m_outputStream) {
                *m_outputStream << message << std::endl;
            }
        }

    };

}