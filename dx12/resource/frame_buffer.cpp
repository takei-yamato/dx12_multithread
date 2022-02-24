#include "dx12/resource/frame_buffer.h"

#include "window/window.h"

namespace dx12::resource {

	namespace {
		constexpr float	clearColor[] = { 0.0f, 0.0f, 1.0f, 1.0f };	///< 画面をクリアする際の色
	}

	namespace {
		//---------------------------------------------------------------------------------
		/**
		 * @brief	リソースにバリアを設定する
		 * @param	commandList	コマンドリスト
		 * @param	resource	バリアを張るリソース
		 * @param	from		変更前のリソースステート
		 * @param	to			変更後のリソースステート
		 */
		void resourceBarrier(CommandList& commandList, ID3D12Resource* resource, D3D12_RESOURCE_STATES from, D3D12_RESOURCE_STATES to) noexcept {

			D3D12_RESOURCE_BARRIER barrier = {};
			barrier.Type						= D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags						= D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource		= resource;
			barrier.Transition.StateBefore		= from;
			barrier.Transition.StateAfter		= to;
			barrier.Transition.Subresource		= D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

			commandList.get()->ResourceBarrier(1, &barrier);
		}
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief	コンストラクタ
	 */
	FrameBuffer::FrameBuffer(uint32_t bufferNum)
	: bufferNum_(bufferNum) {
		resources_.resize(bufferNum_);
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief	バックバッファ用のレンダーターゲットビューを作成する
	 * @param	swapChain	バッファ本体を持っているスワップチェイン
	 * @return	作成に成功した場合は true
	 */
	bool FrameBuffer::createRenderTargetView(IDXGISwapChain3* swapChain) noexcept {

		// ビューを格納するディスクリプタヒープを作成する
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type						= D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.NumDescriptors				= bufferNum_;
		desc.Flags						= D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		
		auto res = Device::instance().device()->CreateDescriptorHeap(
			&desc,
			IID_PPV_ARGS(heap_.GetAddressOf()));
		if (FAILED(res)) {
			ASSERT(false, "バックバッファ用のヒープ確保に失敗");
		}
		
		// 利用するディスクリプタのサイズを取得する
		size_= Device::instance().device()->GetDescriptorHandleIncrementSize(desc.Type);
		
		for (auto i = 0; i < bufferNum_; ++i) {
			swapChain->GetBuffer(i, IID_PPV_ARGS(resources_[i].GetAddressOf()));
			auto handle = heap_->GetCPUDescriptorHandleForHeapStart();
			handle.ptr += i * size_;
			Device::instance().device()->CreateRenderTargetView(resources_[i].Get(), nullptr, handle);
		}
		
		return true;
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief	フレームバッファへのレンダリングを開始する
	 * @param	commandList	利用するコマンドリスト
	 */
	void FrameBuffer::startRendering(CommandList& commandList) noexcept {
		resourceBarrier(commandList, resources_[currentBufferIndex_].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

		// レンダーターゲット
		D3D12_CPU_DESCRIPTOR_HANDLE handles[] = { renderTargetView() };
		commandList.get()->OMSetRenderTargets(1, handles, false, nullptr);

		// レンダーターゲットクリア
		commandList.get()->ClearRenderTargetView(renderTargetView(), clearColor, 0, nullptr);
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief	フレームバッファをレンダーターゲットに設定する
	 * @param	commandList	利用するコマンドリスト
	 */
	void FrameBuffer::setToRenderTarget(CommandList& commandList) noexcept {
		// レンダーターゲット
		D3D12_CPU_DESCRIPTOR_HANDLE handles[] = { renderTargetView() };
		commandList.get()->OMSetRenderTargets(1, handles, false, nullptr);

		// ビューポート
		D3D12_VIEWPORT viewport = {};
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		viewport.Width = static_cast<float>(window::width());
		viewport.Height = static_cast<float>(window::Height());
		viewport.MinDepth = D3D12_MIN_DEPTH;
		viewport.MaxDepth = D3D12_MAX_DEPTH;
		commandList.get()->RSSetViewports(1, &viewport);

		// シザー矩形
		D3D12_RECT rect = {};
		rect.left = 0;
		rect.top = 0;
		rect.right = window::width();
		rect.bottom = window::Height();
		commandList.get()->RSSetScissorRects(1, &rect);
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief	フレームバッファへのレンダリングを終了する
	 * @param	commandList	利用するコマンドリスト
	 */
	void FrameBuffer::finishRendering(CommandList& commandList) noexcept {
		resourceBarrier(commandList, resources_[currentBufferIndex_].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief	バックバッファのインデックスを更新する
	 * @return	更新後のインデックス
	 */
	void FrameBuffer::updateBufferIndex(uint32_t index) noexcept {
		currentBufferIndex_ = index;
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief	フレームバッファの数を取得する
	 * @return	フレームバッファの数
	 */
	uint32_t FrameBuffer::bufferNum() const noexcept {
		return bufferNum_;
	}


	//---------------------------------------------------------------------------------
	/**
	 * @brief	レンダーターゲットビューを取得する
	 * @return	レンダーターゲットビュー（ハンドル）
	 */
	D3D12_CPU_DESCRIPTOR_HANDLE FrameBuffer::renderTargetView() const noexcept {
		return { heap_->GetCPUDescriptorHandleForHeapStart().ptr + (currentBufferIndex_ * size_) };
	}

}

