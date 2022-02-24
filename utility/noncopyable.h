#pragma once

namespace utility {

	//---------------------------------------------------------------------------------
	/**
	 * @brief
	 * コピー禁止
	 */
	class Noncopyable {

	public:
		//---------------------------------------------------------------------------------
		/**
		 * @brief	コンストラクタ
		 */
		Noncopyable() = default;

		//---------------------------------------------------------------------------------
		/**
		 * @brief	コピー処理の禁止
		 *			ムーブは許可する
		 */
		Noncopyable(const Noncopyable& r)				= delete;
		Noncopyable& operator=(const Noncopyable& r)	= delete;
	};
}


