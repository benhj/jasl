//
//  SharedVarCache.hpp
//  jasl
//
//  Created by Ben Jones on 21/03/2015
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "LocalVarCache.hpp"

#include <memory>

namespace jasl {

    typedef std::shared_ptr<LocalVarCache> SharedVarCache;

}