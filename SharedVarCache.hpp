//
//  SharedVarCache.hpp
//  jasl
//
//  Created by Ben Jones on 21/03/2015
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "ScopedVarCache.hpp"

#include <memory>

namespace jasl {

    using SharedVarCache = std::shared_ptr<ScopedVarCache>;

}