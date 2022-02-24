#pragma once
#include <tchar.h>

namespace utility
{
	//---------------------------------------------------------------------------------
	/**
	 * @brief デバッグトレース表示
	 * @param	str	表示文字列
	 * @return
	 */
	void debugMsg(const char* str, ...);


	//---------------------------------------------------------------------------------
	/**
	 * @brief アサート
	 * @param	condition	アサート条件
	 * @param	str			表示文字列
	 * @return
	 */
	void assertMsg(bool condition, const char* str, ...);
}


#if _DEBUG
#define TRACE(str, ...)				utility::debugMsg(str, __VA_ARGS__)
#define ASSERT(condition, str, ...)	utility::assertMsg((condition), str, __VA_ARGS__)
#else
#define TRACE(str, ...)
#define ASSERT(condition, str, ...)
#endif