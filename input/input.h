#pragma once

#include "utility/singleton.h"

namespace input {
	//---------------------------------------------------------------------------------
	/**
	 * @brief
	 * 入力情報の管理
	 */
	class Input : public utility::Singleton<Input> {
	private:
		friend class utility::Singleton<Input>;

	public:
		//---------------------------------------------------------------------------------
		/**
		 * @brief	デストラクタ
		 */
		~Input();

	public:
		//---------------------------------------------------------------------------------
		/**
		 * @brief	キー情報の取得
		 * @param	key		キーの識別子
		 * @return	入力されていればtrue
		 */
		bool		getKey(uint16_t key) noexcept;

		//---------------------------------------------------------------------------------
		/**
		 * @brief	キー情報の更新
		 * @param	pState		ハードから設定されたキー情報配列のポインタ
		 */
		void		updateKeyState(void* pState) noexcept;

	private:
		//---------------------------------------------------------------------------------
		/**
		 * @brief	コンストラクタ
		 */
		Input();

	private:
		class Impl;
		std::unique_ptr<Impl> impl_;		///< インプリメントクラスポインタ

	};
}
