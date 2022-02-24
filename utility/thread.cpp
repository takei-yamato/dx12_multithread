#include "thread.h"
#include <process.h>

namespace utility {

	using namespace std;

	// --------------------------------------------------------------
	/**
	*	スレッドインプリメントクラス
	*	stdスレッド
	*/
	class Thread::Impl {
	public:
		//---------------------------------------------------------------------------------
		/**
		 * @brief	コンストラクタ
		 */
		Impl() {
			thread_.reset();
		}

		//---------------------------------------------------------------------------------
		/**
		 * @brief	デストラクタ
		 */
		~Impl() {
			if (thread_) {
				TRACE("スレッドの終了を待つ");
				thread_->join();
				thread_.reset();
			}
		}

		//---------------------------------------------------------------------------------
		/**
		 * @brief	スレッドの開始
		 * @param	func		スレッドのワーカー関数
		 * @param	arg			スレッドに渡す情報
		 */
		void start(const WorkerFunc& func, void* arg) {
			ASSERT(!thread_, "すでにスレッドが開始しています");
			// スレッド開始
			thread_.reset(new std::thread(std::move(func), arg));
		}

		//---------------------------------------------------------------------------------
		/**
		 * @brief	スレッドの終了待機
		 */
		void wait() {
			if (thread_) {
				ASSERT(thread_->joinable(), "すでにスレッドが終了しています");

				thread_->join();
				thread_.reset();
			}
		}
	private:
		std::unique_ptr<std::thread>	thread_;
	};

	//---------------------------------------------------------------------------------
	/**
	 * @brief	コンストラクタ
	 */
	Thread::Thread() {
		impl_.reset(new Impl());
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief	デストラクタ
	 */
	Thread::~Thread() {
		impl_.reset();
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief	スレッドの開始
	 * @param	func		スレッドのワーカー関数
	 * @param	arg			スレッドに渡す情報
	 */
	void Thread::start(const WorkerFunc& func, void* arg) {
		impl_->start(func, arg);
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief	スレッドの終了待機
	 * @return
	 */
	void Thread::wait() {
		impl_->wait();
	}
}
