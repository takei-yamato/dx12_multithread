#include "dx12/resource/constant_buffer.h"

namespace {

	//---------------------------------------------------------------------------------
	/** @def
	 * アラインメント
	 */
	#define ALIGN(size)	((size + D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1) & ~(D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1))
}

namespace dx12::resource {

	//---------------------------------------------------------------------------------
	/**
	 * @brief	コンスタントバッファを生成する
	 * @param	data		データの先頭アドレス
	 * @param	stride		バッファのストライド
	 * @param	num			バッファの数
	 * @return	作成に成功した場合は true
	 */
	bool ConstantBufferResource::create(void* data, uint32_t stride, uint32_t num) noexcept
	{
		// コンスタントバッファ用ディスクリプタヒープ作成
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.Type						= D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;	// このヒープはコンスタントバッファのビュー（CBV）として利用する
		heapDesc.NumDescriptors				= num;									
		heapDesc.Flags						= D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		dx12::Device::instance().device()->CreateDescriptorHeap(
			&heapDesc,
			IID_PPV_ARGS(heap_.GetAddressOf()));

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
		resourceDesc.Width					= ALIGN(stride) * num;
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
			ASSERT(false, "コンスタントバッファの作成に失敗");
			return false;
		}

		for (auto i = 0; i < num; ++i) {
			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
			cbvDesc.BufferLocation	= gpuResource_->GetGPUVirtualAddress() + i * ALIGN(stride);
			cbvDesc.SizeInBytes		= ALIGN(stride);

			auto size	= dx12::Device::instance().device()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			auto handle = heap_->GetCPUDescriptorHandleForHeapStart();
			handle.ptr += i * size;
			dx12::Device::instance().device()->CreateConstantBufferView(&cbvDesc, handle);
		}

		gpuResource_->Map(0, nullptr, reinterpret_cast<void**>(data));

		stride_ = stride;
		num_	= num;

		return true;
	}


	//---------------------------------------------------------------------------------
	/**
	 * @brief	コマンドリストに設定する
	 * @param	commandList		設定先のコマンドリスト
	 * @param	index			コンスタントバッファのインデックス
	 */
	void ConstantBufferResource::setToCommandList(dx12::CommandList& commandList, uint32_t index) noexcept {
		// ヒープの設定
		ID3D12DescriptorHeap* p[] = { heap_.Get() };
		commandList.get()->SetDescriptorHeaps(_countof(p), p);

		// コンスタントバッファビューの設定
		auto size	= dx12::Device::instance().device()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		auto handle = heap_->GetGPUDescriptorHandleForHeapStart();
		handle.ptr += index * size;
		commandList.get()->SetGraphicsRootDescriptorTable(0, handle);
	}


	//---------------------------------------------------------------------------------
	/**
	 * @brief	オフセットを取得する
	 * @param	index			コンスタントバッファのインデックス
	 * @return	インデックスに対応するオフセット
	 */
	uint64_t ConstantBufferResource::offset(uint32_t index) const noexcept {
		ASSERT(index < num_, "バッファサイズが不正です");
		return ALIGN(stride_) * index;
	}


}

