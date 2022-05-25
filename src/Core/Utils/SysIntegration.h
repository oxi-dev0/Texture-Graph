#pragma once

#include <windows.h>
#include <atlstr.h>

namespace Utility {
	extern CStringA ExecCmd(const wchar_t* cmd);
}