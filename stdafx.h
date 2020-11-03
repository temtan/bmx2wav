// stdafx.h

#pragma once

// C
#include <errno.h>
#include <limits.h>
#include <process.h>
#include <stdio.h>
#include <time.h>

// C++
#include <algorithm>
#include <functional>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <vector>
#include <optional>

// Windows
#include "tt_windows_h_include.h"
#include <commctrl.h>
#pragma warning(push)
#  pragma warning( disable: 4005 )
#  include <shellapi.h>
#pragma warning(pop)
#pragma warning(push)
#  pragma warning(disable : 4005)
#  pragma warning(disable : 4917)
#  include <Shlobj.h>
#pragma warning(pop)

// ttl
#include "ttl_define.h"
