#pragma once

#include "utility/singleton.h"
#include "dx12/command_queue.h"
#include "dx12/resource/frame_buffer.h"

namespace dx12 {
	//---------------------------------------------------------------------------------
	/**
	 * @brief
	 * スワップチェイン制御
	 */
	class SwapChain final : public utility::Singleton<SwapChain> {
	private:
		friend class utility::Singleton<SwapChain>;

	public:
		//---------------------------------------------------------------------------------
		/**
		 * @brief	デストラクタ
		 */
		~SwapChain();

		//---------------------------------------------------------------------------------
		/**
		 * @brief	スワップチェインを作成する
		 * @param	commandQueue	作成に利用するコマンドキュー
		 * @param	frameBuffer		フレームバッファ
		 * @return	正しく作成できた場合は true
		 */
		bool create(const CommandQueue& commandQueue, resource::FrameBuffer& frameBuffer) noexcept;

		//---------------------------------------------------------------------------------
		/**
		 * @brief	現在のバッファインデックスを取得する
		 * @return	バッファインデックス
		 */
		[[nodiscard]] uint32_t currentBufferIndex() const noexcept;

		//---------------------------------------------------------------------------------
		/**
		 * @brief	プレゼンテーション
		 */
		void present() noexcept;


	private:
		//---------------------------------------------------------------------------------
		/**
		 * @brief	コンストラクタ
		 */
		SwapChain();

	private:
		class Impl;
		std::unique_ptr<Impl> impl_;		///< インプリメントクラスポインタ

	};
}
