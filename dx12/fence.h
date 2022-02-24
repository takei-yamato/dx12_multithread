#pragma once

#include "dx12/device.h"

#include "utility/noncopyable.h"

namespace dx12 {
	//---------------------------------------------------------------------------------
	/**
	 * @brief
	 * フェンス
	 */
	class Fence final : utility::Noncopyable {

	public:
		//---------------------------------------------------------------------------------
		/**
		 * @brief	コンストラクタ
		 */
		Fence() = default;

		//---------------------------------------------------------------------------------
		/**
		 * @brief	デストラクタ
		 */
		~Fence() = default;

		//---------------------------------------------------------------------------------
		/**
		 * @brief	フェンスを作成する
		 * @return	作成に成功した場合は true
		 */
		bool create() noexcept;

		//---------------------------------------------------------------------------------
		/**
		 * @brief	フェンスを取得する
		 */
		[[nodiscard]] ID3D12Fence* get() const noexcept;

	private:
		Microsoft::WRL::ComPtr<ID3D12Fence>	fence_;	///< フェンス

	};
}
