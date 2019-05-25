/// Ben H.D. Jones 2019

#include <iostream>

#include "BreakControlFlowException.hpp"

namespace jasl {
    void handleException() {
        try {
            throw;
        } catch (BreakControlFlowException const &) {
            throw;
        } catch (...) {
            // swallow
        }
    }
}