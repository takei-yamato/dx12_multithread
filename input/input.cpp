#include "input.h"

namespace input {
	//---------------------------------------------------------------------------------
	/**
	 * @brief
	 * 入力情報管理のインプリメントクラス
	 */
	class Input::Impl {
	public:
		byte	keyState_[256] = {};		///< ハードから設定されたキー情報配列
		std::shared_mutex mtx_ = {};		///< 同期オブジェクト

	public:
		//---------------------------------------------------------------------------------
		/**
		 * @brief	コンストラクタ
		 */
		Impl() = default;

		//---------------------------------------------------------------------------------
		/**
		 * @brief	デストラクタ
		 */
		~Impl() = default;

		//---------------------------------------------------------------------------------
		/**
		 * @brief	キー情報の取得
		 * @param	key		キーの識別子
		 * @return	入力されていればtrue
		 */
		bool getKey(uint16_t sKey) noexcept {
			std::shared_lock<decltype(mtx_)> lock(mtx_);
			return (keyState_[sKey] & 0x80);
		}

		//---------------------------------------------------------------------------------
		/**
		 * @brief	キー情報の更新
		 * @param	pState		ハードから設定されたキー情報配列のポインタ
		 */
		void updateKeyState(void* pState) noexcept {
			std::lock_guard<decltype(mtx_)> lock(mtx_);
			memcpy_s(keyState_, 256, pState, 256);
		}
	};


	//---------------------------------------------------------------------------------
	/**
	 * @brief	デストラクタ
	 */
	Input::~Input() {
		impl_.reset();
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief	キー情報の取得
	 * @param	key		キーの識別子
	 * @return	入力されていればtrue
	 */
	bool Input::getKey(uint16_t key) noexcept {
		return impl_->getKey(key);
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief	キー情報の更新
	 * @param	pState		ハードから設定されたキー情報配列のポインタ
	 */
	void Input::updateKeyState(void* pState) noexcept {
		impl_->updateKeyState(pState);
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief	コンストラクタ
	 */
	Input::Input() {
		impl_.reset(new Input::Impl());
	}
}
