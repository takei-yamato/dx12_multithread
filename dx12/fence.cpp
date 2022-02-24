#include "dx12/fence.h"

namespace dx12 {

	//---------------------------------------------------------------------------------
	/**
	 * @brief	フェンスを作成する
	 * @return	作成に成功した場合は true
	 */
	bool Fence::create() noexcept {

		auto initValue = 0;

		// フェンス作成
		auto res = Device::instance().device()->CreateFence(
			initValue,
			D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(fence_.GetAddressOf()));
		if (FAILED(res)) {
			ASSERT(false, "フェンス作成に失敗");
			return false;
		}

		return true;
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief	フェンスを取得する
	 */
	ID3D12Fence* Fence::get() const noexcept {
		return fence_.Get();
	}

}

