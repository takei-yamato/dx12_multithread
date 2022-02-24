#pragma once

#include "dx12/device.h"

#include "utility/noncopyable.h"

namespace dx12 {

	//---------------------------------------------------------------------------------
	/**
	 * @brief
	 * コマンドリスト
	 */
	class CommandList final : public utility::Noncopyable {

	public:
		//---------------------------------------------------------------------------------
		/**
		 * @brief	コンストラクタ
		 */
		CommandList() = default;

		//---------------------------------------------------------------------------------
		/**
		 * @brief	デストラクタ
		 */
		~CommandList();

		//---------------------------------------------------------------------------------
		/**
		 * @brief	コマンドリストを作成する
		 * @return	作成に成功した場合は true
		 */
		bool create() noexcept;

		//---------------------------------------------------------------------------------
		/**
		 * @brief	コマンドリストをリセットする
		 */
		void reset() noexcept;

		//---------------------------------------------------------------------------------
		/**
		 * @brief	コマンドリストを取得する
		 */
		[[nodiscard]] ID3D12GraphicsCommandList* get() const noexcept;

	private:
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator>		commandAllocator_;	///< コマンドアロケータ
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>	commandList_;		///< コマンドリスト

	};
}
