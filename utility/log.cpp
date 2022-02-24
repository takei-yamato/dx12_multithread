#include "log.h"

namespace utility
{
	//---------------------------------------------------------------------------------
	/**
	 * @brief デバッグトレース表示
	 * @param	str	表示文字列
	 * @return
	 */
	void debugMsg(const char* str, ...) {
		std::string temp("[ TRACE ] : ");
		temp = temp + str + "\n";

		TCHAR msg[1024];

		va_list args;
		va_start(args, str);
		vsprintf_s(msg, temp.c_str(), args);
		va_end(args);

		OutputDebugString(msg);
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief アサート
	 * @param	condition	アサート条件
	 * @param	str			表示文字列
	 * @return
	 */
	void assertMsg(bool condition, const char* str, ...) {

		if (!condition) {
			std::string temp("!! ");
			temp = temp + str + "\n";
			TCHAR msg[1024] = {};

			OutputDebugString("=============================    Assert   =============================\n");

			va_list args;
			va_start(args, str);
			vsprintf_s(msg, temp.c_str(), args);
			va_end(args);
			OutputDebugString(msg);

			OutputDebugString("=======================================================================\n");

			_CrtDbgBreak();
		}
	}
}

