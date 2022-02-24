#pragma once

#include <thread>

#include "utility/noncopyable.h"

namespace utility {

	//---------------------------------------------------------------------------------
	/**
	 * @brief
	 * スレッド制御クラス
	 */
	class Thread final : Noncopyable {
	private:
		using WorkerFunc = std::function<uint32_t(void*)>;

	public:
		//---------------------------------------------------------------------------------
		/**
		 * @brief	コンストラクタ
		 */
		Thread();

		//---------------------------------------------------------------------------------
		/**
		 * @brief	デストラクタ
		 */
		~Thread();

		//---------------------------------------------------------------------------------
		/**
		 * @brief	スレッドの開始
		 * @param	func		スレッドのワーカー関数
		 * @param	arg			スレッドに渡す情報
		 */
		void start(const WorkerFunc& func, void* arg);

		//---------------------------------------------------------------------------------
		/**
		 * @brief	スレッドの終了待機
		 */
		void wait();

	private:
		class Impl;
		std::unique_ptr<Impl> impl_;
	};
}

