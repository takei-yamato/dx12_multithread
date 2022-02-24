#include "dx12/command_list.h"

namespace dx12 {

	//---------------------------------------------------------------------------------
	/**
	 * @brief	デストラクタ
	 */
	CommandList::~CommandList() {
		reset();
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief	コマンドリストを作成する
	 * @return	作成に成功した場合は true
	 */
	bool CommandList::create() noexcept {

		// アロケータ作成
		auto res = Device::instance().device()->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(commandAllocator_.GetAddressOf()));
		if (FAILED(res)) {
			ASSERT(false, "コマンドアロケータ作成に失敗");
			return false;
		}

		// コマンドリスト作成
		res = Device::instance().device()->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			commandAllocator_.Get(), nullptr, IID_PPV_ARGS(commandList_.GetAddressOf()));
		if (FAILED(res)) {
			ASSERT(false, "コマンドリスト作成に失敗");
			return false;
		}

		commandList_->SetName(L"AA");

		commandList_->Close();

		return true;
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief	コマンドリストをリセットする
	 */
	void CommandList::reset() noexcept {
		// コマンドアロケータをリセット
		commandAllocator_->Reset();

		// コマンドリセット
		commandList_->Reset(commandAllocator_.Get(), nullptr);
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief	コマンドリストを取得する
	 */
	ID3D12GraphicsCommandList* CommandList::get() const noexcept {
		return commandList_.Get();
	}

}

