#include "dx12/resource/mesh.h"
#include "dx12/device.h"

namespace dx12::resource {


	//---------------------------------------------------------------------------------
	/**
	 * @brief	頂点バッファを作成する
	 * @param	data		データの先頭アドレス
	 * @param	stride		バッファのストライド
	 * @param	num			バッファの数
	 * @return	作成に成功した場合は true
	 */
	bool VertexBufferResource::create(void* data, uint32_t stride, uint32_t num) noexcept {

		// GPUリソース作成
		D3D12_HEAP_PROPERTIES heapProperty = {};
		heapProperty.Type					= D3D12_HEAP_TYPE_UPLOAD;
		heapProperty.CPUPageProperty		= D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProperty.MemoryPoolPreference	= D3D12_MEMORY_POOL_UNKNOWN;
		heapProperty.CreationNodeMask		= 1;
		heapProperty.VisibleNodeMask		= 1;

		D3D12_RESOURCE_DESC resourceDesc = {};
		resourceDesc.Dimension				= D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Alignment				= 0;
		resourceDesc.Width					= stride * num;
		resourceDesc.Height					= 1;
		resourceDesc.DepthOrArraySize		= 1;
		resourceDesc.MipLevels				= 1;
		resourceDesc.Format					= DXGI_FORMAT_UNKNOWN;
		resourceDesc.SampleDesc.Count		= 1;
		resourceDesc.SampleDesc.Quality		= 0;
		resourceDesc.Layout					= D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resourceDesc.Flags					= D3D12_RESOURCE_FLAG_NONE;

		auto res = dx12::Device::instance().device()->CreateCommittedResource(
			&heapProperty,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(gpuResource_.GetAddressOf()));
		if (FAILED(res)) {
			ASSERT(false, "頂点バッファの作成に失敗");
			return false;
		}

		byte* ptr = {};
		gpuResource_->Map(0, nullptr, reinterpret_cast<void**>(&ptr));
		memcpy(ptr, data, stride * num);
		gpuResource_->Unmap(0, nullptr);

		// 頂点バッファビュー作成
		view_.BufferLocation = gpuResource_->GetGPUVirtualAddress();
		view_.StrideInBytes	 = stride;
		view_.SizeInBytes	 = stride * num;

		return true;
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief	コマンドリストに設定する
	 * @param	commandList		設定先のコマンドリスト
	 */
	void VertexBufferResource::setToCommandList(dx12::CommandList& commandList) noexcept {

		// ポリゴントポロジーの指定
		commandList.get()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		// 頂点バッファをセット
		commandList.get()->IASetVertexBuffers(0, 1, &view_);
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief	インデックスバッファを作成する
	 * @param	data		データの先頭アドレス
	 * @param	stride		バッファのストライド
	 * @param	num			バッファの数
	 * @return	作成に成功した場合は true
	 */
	bool IndexBufferResource::create(void* data, uint32_t stride, uint32_t num) noexcept {

		// GPUリソース作成
		D3D12_HEAP_PROPERTIES heapProperty = {};
		heapProperty.Type					= D3D12_HEAP_TYPE_UPLOAD;
		heapProperty.CPUPageProperty		= D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProperty.MemoryPoolPreference	= D3D12_MEMORY_POOL_UNKNOWN;
		heapProperty.CreationNodeMask		= 1;
		heapProperty.VisibleNodeMask		= 1;

		D3D12_RESOURCE_DESC resourceDesc = {};
		resourceDesc.Dimension				= D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Alignment				= 0;
		resourceDesc.Width					= stride * num;
		resourceDesc.Height					= 1;
		resourceDesc.DepthOrArraySize		= 1;
		resourceDesc.MipLevels				= 1;
		resourceDesc.Format					= DXGI_FORMAT_UNKNOWN;
		resourceDesc.SampleDesc.Count		= 1;
		resourceDesc.SampleDesc.Quality		= 0;
		resourceDesc.Layout					= D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resourceDesc.Flags					= D3D12_RESOURCE_FLAG_NONE;

		auto res = dx12::Device::instance().device()->CreateCommittedResource(
			&heapProperty,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(gpuResource_.GetAddressOf()));
		if (FAILED(res)) {
			ASSERT(false, "インデックスバッファの作成に失敗");
			return false;
		}

		byte* ptr = {};
		gpuResource_->Map(0, nullptr, reinterpret_cast<void**>(&ptr));
		memcpy(ptr, data, stride * num);
		gpuResource_->Unmap(0, nullptr);

		// インデックスバッファビュー作成
		view_.BufferLocation	= gpuResource_->GetGPUVirtualAddress();
		view_.Format			= stride == 4 ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;
		view_.SizeInBytes		= stride * num;

		return true;
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief	コマンドリストに設定する
	 * @param	commandList		設定先のコマンドリスト
	 */
	void IndexBufferResource::setToCommandList(dx12::CommandList& commandList) noexcept {

		// インデックスバッファをセット
		commandList.get()->IASetIndexBuffer(&view_);
	}

}

