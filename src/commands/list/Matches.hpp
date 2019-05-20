/// (c) 2019 Ben H. D. Jones

#pragma once

#include "caching/CacheStack.hpp"
#include "caching/Value.hpp"

namespace jasl {


    // To describe how list tokens match.
    enum class MatchType
    {
        Exact,     // [hello], [hello]
        QVar,      // [hello], [?var]
        QQVar,     // [hello], [??var]
        HVar,      // [hello], [^var]
        HHVar,     // [hello], [^^var]
        SingleEq,  // [hello], [=]
        DoubleEq,  // [hello], [==]
        NoMatch    // [hello], [goodbye]
    };

    class Matches
    {
      public:
        Matches(SharedCacheStack sharedCache);
        bool matches(List const & first,
                     List const & second) const;
      private:
        SharedCacheStack m_sharedCache;
        int mutable m_callCounter;

        bool doMatches(List const & first,
                       List const & second) const;

        MatchType doMatches(Value const & first,
                            Value const & second) const;

        bool handleExact(List const & first,
                         List const & second,
                         List::const_iterator & itFirst,
                         List::const_iterator & itSecond) const;

        bool handleQVar(List const & first,
                        List const & second,
                        List::const_iterator & itFirst,
                        List::const_iterator & itSecond) const;

        bool handleQQVar(List const & first,
                         List const & second,
                         List::const_iterator & itFirst,
                         List::const_iterator & itSecond) const;

        bool handleHVar(List const & first,
                        List const & second,
                        List::const_iterator & itFirst,
                        List::const_iterator & itSecond) const;

        bool handleHHVar(List const & first,
                         List const & second,
                         List::const_iterator & itFirst,
                         List::const_iterator & itSecond) const;

        bool handleDoubleEq(List const & first,
                            List const & second,
                            List::const_iterator & itFirst,
                            List::const_iterator & itSecond) const;

        bool handleEq(List const & first,
                      List const & second,
                      List::const_iterator & itFirst,
                      List::const_iterator & itSecond) const;
    };
}