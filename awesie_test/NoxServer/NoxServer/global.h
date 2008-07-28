// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

// standard libraries
#include <string>

// date_time will include windows.h, so we need to have this
#define WIN32_LEAN_AND_MEAN

// timer functions
#include "boost/date_time/posix_time/posix_time_types.hpp"

// singletons
#include "boost/utility/singleton.hpp"

// threading
#include "boost/thread/thread.hpp"
#include "boost/bind.hpp"

// math
#include <boost/numeric/ublas/vector.hpp>

// containers
#include <boost/foreach.hpp>

#include <boost/function.hpp>

// physics
#include "Box2D.h"
#define SCALING_FACTOR 0.1f

#include "opcodes.h"