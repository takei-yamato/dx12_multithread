#pragma once

#include "dx12/command_list.h"

#include "utility/noncopyable.h"

namespace dx12::resource {

	//---------------------------------------------------------------------------------
	/**
	 * @brief
	 * 頂点バッファリソース
	 */
	class VertexBufferResource final : public utility::Noncopyable {
	public:
		//---------------------------------------------------------------------------------
		/**
		 * @brief	コンストラクタ
		 */
		VertexBufferResource() = default;

		//---------------------------------------------------------------------------------
		/**
		 * @brief	デストラクタ
		 */
		~VertexBufferResource() = default;

		//---------------------------------------------------------------------------------
		/**
		 * @brief	頂点バッファを作成する
		 * @param	data		データの先頭アドレス
		 * @param	stride		バッファのストライド
		 * @param	num			バッファの数
		 * @return	作成に成功した場合は true
		 */
		bool create(void* data, uint32_t stride, uint32_t num) noexcept;

		//---------------------------------------------------------------------------------
		/**
		 * @brief	コマンドリストに設定する
		 * @param	commandList		設定先のコマンドリスト
		 */
		void setToCommandList(dx12::CommandList& commandList) noexcept;

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource>	gpuResource_	= {};	///< 頂点バッファリソース
		D3D12_VERTEX_BUFFER_VIEW				view_			= {};	///< 頂点バッファビュー
		uint32_t								stride_			= {};	///< バッファのストライド
		uint32_t								num_			= {};	///< バッファ数
	};

	//---------------------------------------------------------------------------------
	/**
	 * @brief
	 * インデックスバッファリソース
	 */
	class IndexBufferResource final : public utility::Noncopyable {
	public:
		//---------------------------------------------------------------------------------
		/**
		 * @brief	コンストラクタ
		 */
		IndexBufferResource() = default;

		//---------------------------------------------------------------------------------
		/**
		 * @brief	デストラクタ
		 */
		~IndexBufferResource() = default;

		//---------------------------------------------------------------------------------
		/**
		 * @brief	インデックスバッファを作成する
		 * @param	data		データの先頭アドレス
		 * @param	stride		バッファのストライド
		 * @param	num			バッファの数
		 * @return	作成に成功した場合は true
		 */
		bool create(void* data, uint32_t stride, uint32_t num) noexcept;

		//---------------------------------------------------------------------------------
		/**
		 * @brief	コマンドリストに設定する
		 * @param	commandList		設定先のコマンドリスト
		 */
		void setToCommandList(dx12::CommandList& commandList) noexcept;

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource>	gpuResource_	= {};	///< インデックスバッファリソース
		D3D12_INDEX_BUFFER_VIEW					view_			= {};	///< インデックスバッファビュー
		uint32_t								stride_			= {};	///< バッファのストライド
		uint32_t								num_			= {};	///< バッファ数
	};


	//---------------------------------------------------------------------------------
	/**
	 * @brief
	 * メッシュ
	 */
	class Mesh final : utility::Noncopyable {
	public:
		//---------------------------------------------------------------------------------
		/**
		 * @brief	コンストラクタ
		 */
		Mesh() {
			vertexBufferResource_.reset(new VertexBufferResource());
			indexBufferResource_.reset(new IndexBufferResource());
		}

		//---------------------------------------------------------------------------------
		/**
		 * @brief	デストラクタ
		 */
		~Mesh() = default;

		//---------------------------------------------------------------------------------
		/**
		 * @brief	頂点バッファを作成する
		 * @param	data		データの先頭アドレス
		 */
		template<class vertexFormat, uint32_t Num>
		void createVertexBuffer(vertexFormat (&data)[Num]) noexcept {
			vertexBufferResource_->create(reinterpret_cast<void*>(data), sizeof(vertexFormat), Num);
		}

		//---------------------------------------------------------------------------------
		/**
		 * @brief	インデックスバッファを作成する
		 * @param	data		データの先頭アドレス
		 */
		template<class indexFormat, uint32_t Num>
		void createIndexBuffer(indexFormat	(&data)[Num]) noexcept {
			indexBufferResource_->create(reinterpret_cast<void*>(data), sizeof(indexFormat), Num);
		}

		//---------------------------------------------------------------------------------
		/**
		 * @brief	コマンドリストに設定する
		 * @param	commandList		設定先のコマンドリスト
		 */
		void setToCommandList(CommandList& commandList) noexcept {
			vertexBufferResource_->setToCommandList(commandList);
			indexBufferResource_->setToCommandList(commandList);
		}

	private:
		std::unique_ptr<VertexBufferResource>	vertexBufferResource_ = {};		///< 頂点バッファリソース
		std::unique_ptr<IndexBufferResource>	indexBufferResource_  = {};		///< インデックスバッファリソース
	};
}
