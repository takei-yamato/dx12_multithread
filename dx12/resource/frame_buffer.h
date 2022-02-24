#pragma once

#include "dx12/command_list.h"

#include "utility/noncopyable.h"

namespace dx12::resource {

	//---------------------------------------------------------------------------------
	/**
	 * @brief
	 * フレームバッファ
	 */
	class FrameBuffer final : utility::Noncopyable {
	public:
		//---------------------------------------------------------------------------------
		/**
		 * @brief	コンストラクタ
		 * @param	bufferNum	フレームバッファ数
		 */
		FrameBuffer(uint32_t bufferNum);

		//---------------------------------------------------------------------------------
		/**
		 * @brief	デストラクタ
		 */
		~FrameBuffer() = default;

		//---------------------------------------------------------------------------------
		/**
		 * @brief	フレームバッファ用のレンダーターゲットビューを作成する
		 * @param	swapChain	バッファ本体を持っているスワップチェイン
		 * @return	作成に成功した場合は true
		 */
		[[nodiscard]] bool createRenderTargetView(IDXGISwapChain3* swapChain) noexcept;

		//---------------------------------------------------------------------------------
		/**
		 * @brief	フレームバッファへのレンダリングを開始する
		 * @param	commandList	利用するコマンドリスト
		 */
		void startRendering(CommandList& commandList) noexcept;

		//---------------------------------------------------------------------------------
		/**
		 * @brief	フレームバッファをレンダーターゲットに設定する
		 * @param	commandList	利用するコマンドリスト
		 */
		void setToRenderTarget(CommandList& commandList) noexcept;

		//---------------------------------------------------------------------------------
		/**
		 * @brief	フレームバッファへのレンダリングを終了する
		 * @param	commandList	利用するコマンドリスト
		 */
		void finishRendering(CommandList& commandList) noexcept;

		//---------------------------------------------------------------------------------
		/**
		 * @brief	フレームバッファのインデックスを更新する
		 * @return	新しいインデックス
		 */
		void updateBufferIndex(uint32_t index) noexcept;

		//---------------------------------------------------------------------------------
		/**
		 * @brief	フレームバッファの数を取得する
		 * @return	フレームバッファの数
		 */
		uint32_t bufferNum() const noexcept;


	private:
		//---------------------------------------------------------------------------------
		/**
		 * @brief	レンダーターゲットビューを取得する
		 * @return	レンダーターゲットビュー（ハンドル）
		 */
		D3D12_CPU_DESCRIPTOR_HANDLE renderTargetView() const noexcept;


	private:
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>		heap_				= {};	///< ディスクリプタヒープ
		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>	resources_			= {};	///< リソース
		const uint32_t										bufferNum_			= {};	///< バッファ数
		uint32_t											size_				= {};	///< ディスクリプタサイズ
		uint32_t											currentBufferIndex_	= {};	///< 現在のバッファインデックス

	};
}
