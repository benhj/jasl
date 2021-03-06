//
//  CallCommand.hpp
//  jasl
//
//  Created by Ben Jones on on 10/03/2015.
//  Copyright (c) 2015-present Ben Jones. All rights reserved.
//


#include "CallCommand.hpp"
#include "commands/ParamExtractor.hpp"
#include "caching/GlobalCache.hpp"
#include "core/CommandInterpretor.hpp"
#include "core/RegisterCommand.hpp"
#include <vector>

bool jasl::CallCommand::m_registered = 
registerCommand<jasl::CallCommand>();

namespace jasl
{
    CallCommand::CallCommand(Function &func_,
                             SharedCacheStack const &sharedCache,
                             OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    , m_functionName()
    , m_returnable(false)
    , m_returnableType()
    , m_returnableSymbol()
    {
        // get the name of the function
        (void)m_func.getValueA<std::string>(m_functionName, m_sharedCache);

        // check if call statement expecting a returnable in
        // which case we search for returnable rather than block
        // functions
        if(m_func.getValueC<std::string>(m_returnSymbol, m_sharedCache)) {
            m_returnable = true;
        }
        setFunction();

        // set returnable type and symbol
        if(m_returnable) {
            // what is the return type?
            (void)m_functionFunc.getValueA<std::string>(m_returnableType, m_sharedCache);
        }

        // Try and get the name of the return variable if we have one
        if(m_returnableType != "nil" && m_returnableType != "") {
            if(m_returnableType != "array") {
                (void)m_functionFunc.getValueD<std::string>(m_returnableSymbol, m_sharedCache);
            } else {
                (void)m_functionFunc.getValueE<std::string>(m_returnableSymbol, m_sharedCache);
            }
        }
    }

    CallCommand::~CallCommand() = default;

    std::vector<std::string> CallCommand::getCommandNames()
    {
        return {"call"};
    }

    void CallCommand::setFunction()
    {
        // First get all subroutines
        std::string &scriptString = GlobalCache::script;
        CommandInterpretor ci;
        auto functions = ci.parseStringCollection(scriptString);

        // Find all functions that are 'fn' or 'block'
        decltype(functions) matched;
        for(auto &f : functions) {
            std::string funcName;
            if (m_returnable) {
                if (f.name.find("fn") != std::string::npos) {
                    matched.push_back(f);
                }
            } else {
                if (f.name.find("fn") != std::string::npos ||
                    f.name.find("block") != std::string::npos) {
                    matched.push_back(f);
                }
            }
        }

        // Second, search for block with given name
        auto it = std::find_if(std::begin(matched), std::end(matched), [&](Function &f) {

            std::string functionName;
            if(f.name.find("block") != std::string::npos) {
                (void)f.getValueA<std::string>(functionName, m_sharedCache);
            } else if(f.name.find("fn") != std::string::npos) {

                std::string type;
                (void)f.getValueA<std::string>(type, m_sharedCache);
                if(type != "array") {
                    (void)f.getValueB<std::string>(functionName, m_sharedCache);
                } else {
                    (void)f.getValueC<std::string>(functionName, m_sharedCache);
                }
            }
            return functionName == m_functionName;
        });

        if (it != std::end(matched)) {
            m_functionFunc = *it;
        }
    }

    bool CallCommand::execute()
    {
        // try to extract input arguments, if there are any
        List array;
        if(m_func.getValueB<ValueArray>(array, m_sharedCache)) {
            pushParams(array, m_sharedCache);
        }

        return parseCommand(m_returnable ? "fn" : "block");
    }

    bool CallCommand::tryArrayTypes(std::string const theType)
    {
        // In the following, theType may either be a sub-type in
        // the case that array:int style was used, or a non sub-type
        // in the cade the ints style was used.
        if(theType == "int" || theType == "ints") {
            IntArray value;
            auto cv = GlobalCache::popReturnValue();
            m_sharedCache->setVar(m_returnSymbol, ::boost::get<decltype(value)>(cv.cv), cv.type);
            return true;
        } else if(theType == "real" || theType == "reals") {
            RealArray value;
            auto cv = GlobalCache::popReturnValue();
            m_sharedCache->setVar(m_returnSymbol, ::boost::get<decltype(value)>(cv.cv), cv.type);
            return true;
        } else if(theType == "byte" || theType == "bytes") {
            ByteArray value;
            auto cv = GlobalCache::popReturnValue();
            m_sharedCache->setVar(m_returnSymbol, ::boost::get<decltype(value)>(cv.cv), cv.type);
            return true;
        } else if(theType == "string" || theType == "strings") {
            StringArray value;
            auto cv = GlobalCache::popReturnValue();
            m_sharedCache->setVar(m_returnSymbol, ::boost::get<decltype(value)>(cv.cv), cv.type);
            return true;
        }
        return false;
        
    }

    bool CallCommand::parseCommand(std::string const &kind)
    {
        CommandInterpretor ci;
        (void)ci.interpretFunc(m_functionFunc, m_sharedCache, m_outputStream);

        // now set result of function if returnable type
        if(kind == "fn") {

            // now set the returned variable in the m_returnSymbol
            // and erase the original function result
            // Note, values reurned from the function are on
            // the global stack so need to access from GlobalCache
            if(m_returnableType == "int") {
                int64_t value;
                auto cv = GlobalCache::popReturnValue();
                m_sharedCache->setVar(m_returnSymbol, ::boost::get<decltype(value)>(cv.cv), cv.type);
            } else if(m_returnableType == "real") {
                double value;
                auto cv = GlobalCache::popReturnValue();
                m_sharedCache->setVar(m_returnSymbol, ::boost::get<decltype(value)>(cv.cv), cv.type);
            } else if(m_returnableType == "string") {
                std::string value;
                auto cv = GlobalCache::popReturnValue();
                m_sharedCache->setVar(m_returnSymbol, ::boost::get<decltype(value)>(cv.cv), cv.type);
            } else if(m_returnableType == "bool") {
                bool value;
                auto cv = GlobalCache::popReturnValue();
                m_sharedCache->setVar(m_returnSymbol, ::boost::get<decltype(value)>(cv.cv), cv.type);
            } else if(m_returnableType == "list") {
                List value;
                auto cv = GlobalCache::popReturnValue();
                m_sharedCache->setVar(m_returnSymbol, ::boost::get<decltype(value)>(cv.cv), cv.type);
            } else if(m_returnableType == "byte") {
                uint8_t value;
                auto cv = GlobalCache::popReturnValue();
                m_sharedCache->setVar(m_returnSymbol, ::boost::get<decltype(value)>(cv.cv), cv.type);
            } else if(m_returnableType == "nil") {
                // don't do anything for return type 'nil'
            } else if (m_returnableType == "array") {
                std::string subType;
                (void)m_functionFunc.getValueB<std::string>(subType, m_sharedCache);
                (void)tryArrayTypes(subType);
            } else {
                if(!tryArrayTypes(m_returnableType)) {
                    setLastErrorMessage("call returnable: unknown return type");
                    return false;
                }
            }
        }

        return true;
    }
}

