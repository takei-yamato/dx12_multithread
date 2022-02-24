#include "dx12/swap_chain.h"
#include "dx12/device.h"

#include "window/window.h"

#pragma comment(lib,"d3d12.lib")

namespace dx12 {

	using namespace Microsoft::WRL;

	//---------------------------------------------------------------------------------
	/**
	 * @brief
	 * スワップチェイン制御のインプリメントクラス
	 */
	class SwapChain::Impl {

	public:
		//---------------------------------------------------------------------------------
		/**
		 * @brief	コンストラクタ
		 */
		Impl() = default;

		//---------------------------------------------------------------------------------
		/**
		 * @brief	デストラクタ
		 */
		~Impl() = default;

		//---------------------------------------------------------------------------------
		/**
		 * @brief	スワップチェインを作成する
		 * @param	commandQueue	作成に利用するコマンドキュー
		 * @param	frameBuffer		フレームバッファ
		 * @return	正しく作成できた場合は true
		 */
		bool create(const CommandQueue& commandQueue, resource::FrameBuffer& frameBuffer) noexcept {

			frameBufferNum_ = frameBuffer.bufferNum();

			if (!createSwapChain(commandQueue)) {
				return false;
			}

			if (!setFrameBuffer(frameBuffer)) {
				return false;
			}

			return true;
		}

		//---------------------------------------------------------------------------------
		/**
		 * @brief	現在のバッファインデックスを取得する
		 * @return	バッファインデックス
		 */
		uint32_t currentBufferIndex() const noexcept {
			return swapChain_->GetCurrentBackBufferIndex();
		}

		//---------------------------------------------------------------------------------
		/**
		 * @brief	プレゼンテーション
		 */
		void present() noexcept {
			swapChain_->Present(0, 0);
		}

	private:
		//---------------------------------------------------------------------------------
		/**
		 * @brief	スワップチェインを作成する
		 * @param	commandQueue	作成に利用するコマンドキュー
		 * @return	作成に成功した場合は true
		 */
		bool createSwapChain(const CommandQueue& commandQueue) noexcept {

			DXGI_SWAP_CHAIN_DESC1 desc	= {};
			desc.BufferCount			= frameBufferNum_;
			desc.Width					= window::width();
			desc.Height					= window::Height();
			desc.Format					= DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.BufferUsage			= DXGI_USAGE_RENDER_TARGET_OUTPUT;
			desc.SwapEffect				= DXGI_SWAP_EFFECT_FLIP_DISCARD;
			desc.SampleDesc.Count		= 1;

			auto res = Device::instance().dxgiFactory()->CreateSwapChainForHwnd(
				commandQueue.get(),
				window::Window::instance().handle(),
				&desc,
				NULL,
				NULL,
				(IDXGISwapChain1**)swapChain_.GetAddressOf());
			if (FAILED(res)) {
				ASSERT(false, "スワップチェイン作成に失敗");
				return false;
			}

			return true;
		}

		//---------------------------------------------------------------------------------
		/**
		 * @brief	フレームバッファを設定する
		 * @param	frameBuffer		フレームバッファ
		 * @return	作成に成功した場合は true
		 */
		bool setFrameBuffer(resource::FrameBuffer& frameBuffer) noexcept {

			// スワップチェインが持つバッファを元にフレームバッファの RTV を作成する
			if (!frameBuffer.createRenderTargetView(swapChain_.Get())) {
				ASSERT(false, "フレームバッファのレンダーターゲットビュー作成に失敗");
				return false;
			}
			return true;
		}

	private:
		ComPtr<IDXGISwapChain3>	swapChain_		= {};	///< スワップチェイン
		uint32_t				frameBufferNum_	= {};	///< フレームバッファの数
	};

	//---------------------------------------------------------------------------------
	/**
	 * @brief	デストラクタ
	 */
	SwapChain::~SwapChain() {
		impl_.reset();
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief	スワップチェインを作成する
	 * @param	commandQueue	作成に利用するコマンドキュー
	 * @param	frameBuffer		フレームバッファ
	 * @return	正しく作成できた場合は true
	 */
	bool SwapChain::create(const CommandQueue& commandQueue, resource::FrameBuffer& frameBuffer) noexcept {
		return impl_->create(commandQueue, frameBuffer);
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief	現在のバッファインデックスを取得する
	 * @return	バッファインデックス
	 */
	uint32_t SwapChain::currentBufferIndex() const noexcept {
		return impl_->currentBufferIndex();
	}


	//---------------------------------------------------------------------------------
	/**
	 * @brief	プレゼンテーション
	 */
	void SwapChain::present() noexcept {
		impl_->present();
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief	コンストラクタ
	 */
	SwapChain::SwapChain() {
		impl_.reset(new SwapChain::Impl());
	}
}

