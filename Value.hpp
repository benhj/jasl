//
//  Value.hpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015-2016 Ben Jones. All rights reserved.
//

#pragma once

#include "any.hpp"
#include <vector>
#include <string>

using Value = simple_any::any;
using ValueArray = std::vector<Value>;
using StringArray = std::vector<std::string>;
