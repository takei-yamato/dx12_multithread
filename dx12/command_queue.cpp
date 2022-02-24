#include "dx12/command_queue.h"

//#pragma comment(lib,"d3d12.lib")

namespace dx12 {

	//---------------------------------------------------------------------------------
	/**
	 * @brief	コマンドキューを生成する
	 * @return	正しく生成できた場合は true
	 */
	bool CommandQueue::create() noexcept {
		// コマンドキュー作成
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		auto res = Device::instance().device()->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(commandQueue_.GetAddressOf()));
		if (FAILED(res)) {
			ASSERT(false, "コマンドキュー作成に失敗");
			return false;
		}
		return true;
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief	コマンドキューを取得する
	 */
	ID3D12CommandQueue* CommandQueue::get() const noexcept {
		return commandQueue_.Get();
	}

}

