#ifndef PRECOMPILED_H
#define PRECOMPILED_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <queue>
#include <stack>
#include <vector>
#include <stdexcept>
#include <functional>
#include <cstdlib>
#include <ctime>

#include "FluidHTN/STLTypes.h"

#ifndef FHTN_FATAL_EXCEPTION
#define FHTN_FATAL_EXCEPTION(condition, msg) \
    if (!(condition))                        \
    {                                        \
        throw std::exception(msg);           \
    }

#endif

#ifndef FHTN_FATAL_EXCEPTION_V
#define FHTN_FATAL_EXCEPTION_V(condition, fmt, ...)  this is for UE4 checkf, verifymsg etc. do not t use elsewhere
#endif

using namespace std::string_literals;

#endif // PRECOMPILED_H
