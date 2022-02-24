#include "time_counter.h"
#include "utility/log.h"
#include <chrono>

namespace utility {
	using namespace std;

	//---------------------------------------------------------------------------------
	/**
	 * @brief	コンストラクタ（計測開始）
	 * @param	tag			識別タグ
	 */
	Time::Time(std::string_view tag) {
		auto time = chrono::system_clock::now().time_since_epoch();
		startTime_ = chrono::duration_cast<chrono::microseconds>(time).count();

		tag_ = tag;
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief	デストラクタ（計測終了）
	 */
	Time::~Time() {
		auto time = chrono::system_clock::now().time_since_epoch();
		auto endTime = chrono::duration_cast<chrono::microseconds>(time).count();

		auto microsec = endTime - startTime_;
		TIME_CONTAINER().add(tag_.c_str(), static_cast<double>(microsec));
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief	デストラクタ
	 */
	TimeContainer::~TimeContainer() {
		container_.clear();
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief	計測対象の追加
	 * @param	tag			識別タグ
	 * @param	microsec	加算するマイクロ秒
	 * @return
	 */
	void TimeContainer::add(std::string_view tag, double microsec) noexcept {
		const auto& it = container_.find(tag.data());
		if (it == container_.end()) {
			container_.insert(std::make_pair(tag, microsec));
		}
		else {
			it->second += microsec;
		}
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief	計測対象の削除
	 * @param	tag			識別タグ
	 */
	void TimeContainer::remove(std::string_view tag) noexcept {
		container_.erase(tag.data());
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief	計測結果の表示
	 * @param	tag		表示する識別タグ（空の場合はすべての識別タグを表示）
	 * @return
	 */
	void TimeContainer::print(std::string_view tag) const noexcept {
		if (tag.empty()) {
			for (auto& x : container_) {
				TRACE("tag [ %s ] : millisec [ %f ]", x.first.c_str(), x.second / 1000.0f);
			}
		}
		else {
			auto it = container_.find(tag.data());
			if (it != container_.end()) {
				TRACE("tag [ %s ] : millisec [ %f ]", it->first.c_str(), it->second / 1000.0f);
			}
		}
	}


	//---------------------------------------------------------------------------------
	/**
	 * @brief	コンストラクタ
	 */
	TimeContainer::TimeContainer() {
		container_.clear();
	}

}
