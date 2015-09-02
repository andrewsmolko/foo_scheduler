#pragma once

#include "targetver.h"

#pragma warning(disable: 4996)

#include "foobar2000.h"
#include "ATLHelpers.h"

#include <Windows.h>
#include <PowrProf.h>
#include <uxtheme.h>

#include <atlbase.h>
#include <atlstr.h>

// #define _WTL_NO_AUTOMATIC_NAMESPACE
#define _WTL_NO_CSTRING

#define BOOST_ALL_NO_LIB

#include <atlapp.h>
#include <atlctrls.h>
#include <atlframe.h>
//#include <atlmisc.h>
#include <atlcrack.h>
#include <atlddx.h>
#include <atlctrlx.h>
#include <atldlgs.h>

#include <string>
#include <vector>
#include <map>

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include <boost/intrusive_ptr.hpp>
#include <boost/detail/atomic_count.hpp>

#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/signals2.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/type_traits.hpp>
#include <boost/enable_shared_from_this.hpp>


//#ifdef UNICODE
//#if defined _M_IX86
//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
//#elif defined _M_IA64
//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
//#elif defined _M_X64
//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
//#else
//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
//#endif
//#endif
