#pragma once

#include <unordered_map>

#include "singleton.h"

namespace utility {

	//---------------------------------------------------------------------------------
	/**
	 * @brief
	 * 時間計測クラス
	 *
	 * スコープ内でインスタンス化される事を想定
	 */
	class Time final {
	public:

		//---------------------------------------------------------------------------------
		/**
		 * @brief	コンストラクタ（計測開始）
		 * @param	tag			識別タグ
		 */
		Time(std::string_view tag);
		Time() = delete;

		//---------------------------------------------------------------------------------
		/**
		 * @brief	デストラクタ（計測終了）
		 */
		~Time();

	private:
		uint64_t		startTime_;	///< 開始時間
		std::string		tag_;		///< 識別タグ
	};


	//---------------------------------------------------------------------------------
	/**
	 * @brief
	 * 時間計測コンテナ
	 *
	 * シングルトンによる制御
	 */
	class TimeContainer : public Singleton<TimeContainer> {
	private:
		friend class Singleton<TimeContainer>;

		struct Data {
			double	total_ = {};
			double	count_ = {};
		};

	public:
		//---------------------------------------------------------------------------------
		/**
		 * @brief	デストラクタ
		 */
		~TimeContainer();

		//---------------------------------------------------------------------------------
		/**
		 * @brief	計測対象の追加
		 * @param	tag			識別タグ
		 * @param	microsec	加算するマイクロ秒
		 */
		void add(std::string_view tag, double microsec) noexcept;

		//---------------------------------------------------------------------------------
		/**
		 * @brief	計測対象の削除
		 * @param	tag			識別タグ
		 */
		void remove(std::string_view tag) noexcept;

		//---------------------------------------------------------------------------------
		/**
		 * @brief	計測結果の表示
		 * @param	tag		表示する識別タグ（空の場合はすべての識別タグを表示）
		 */
		void print(std::string_view tag) const noexcept;

	private:
		//---------------------------------------------------------------------------------
		/**
		 * @brief	コンストラクタ
		 */
		TimeContainer();


	private:
		std::unordered_map<std::string, Data>	container_;	///< 識別タグと計測時間のコンテナ
	};
}

#define TIME_CONTAINER()		utility::TimeContainer::instance()

#if _DEBUG
#define TIME_PRINT(tag)			utility::TimeContainer::instance().print(tag)
#define TIME_CLEAR(tag)			utility::TimeContainer::instance().remove(tag)
#define TIME_CHECK_SCORP(tag)	utility::Time timeCheck(tag)
#else
#define TIME_PRINT(tag)
#define TIME_CLEAR(tag)
#define TIME_CHECK_SCORP(tag)
#endif
