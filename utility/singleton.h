#pragma once

namespace utility {

	//---------------------------------------------------------------------------------
	/**
	 * @brief
	 * シングルトン化制御
	 *
	 * このクラスを継承したクラスをシングルトン化させる
	 */
	template <class T>
	class Singleton {

	public:
		using instance_ptr = std::unique_ptr<T>;

	public:

		//---------------------------------------------------------------------------------
		/**
		 * @brief	シングルトンインスタンスを取得する
		 * @return	シングルトンインスタンス
		 */
		static T& instance() noexcept {
			if (!instance_) {
				instance_.reset(new T());
			}
			return *instance_;
		}

		//---------------------------------------------------------------------------------
		/**
		 * @brief	シングルトンインスタンスを破棄する
		 */
		static void release() noexcept {
			instance_.reset();
		}

	protected:
		//---------------------------------------------------------------------------------
		/**
		 * @brief	コンストラクタ
		 */
		Singleton() noexcept = default;

	private:
		Singleton(const Singleton& r) = delete;
		Singleton& operator=(const Singleton& r) = delete;
		Singleton(Singleton&& r) noexcept = delete;
		Singleton& operator=(Singleton&& r) noexcept = delete;

	private:
		static instance_ptr instance_;		///< インスタンス
	};

	template <class T> typename Singleton<T>::instance_ptr Singleton<T>::instance_;
}


