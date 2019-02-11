// stdafx.cpp : source file that includes just the standard includes
// HideMyApps.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

#if _MSC_VER > 1500
#    define COMPILE_MULTIMON_STUBS

#    pragma warning(push)
#    pragma warning(disable : 4996)

#    include "multimon.h"

#    pragma warning(pop)
#endif
