#pragma once

#include <d3d12.h>
#include <dxgi1_4.h>
#include <DirectXMath.h>
#include <wrl/client.h>

#include "utility/singleton.h"

namespace dx12 {
	//---------------------------------------------------------------------------------
	/**
	 * @brief
	 * DirectX12デバイスの制御
	 */
	class Device final : public utility::Singleton<Device> {
	private:
		friend class utility::Singleton<Device>;

	public:
		//---------------------------------------------------------------------------------
		/**
		 * @brief	デストラクタ
		 */
		~Device();

		//---------------------------------------------------------------------------------
		/**
		 * @brief	デバイスを作成する
		 * @return	正しく作成できた場合は true
		 */
		bool create() noexcept;

		//---------------------------------------------------------------------------------
		/**
		 * @brief	デバイスを取得する
		 * @return	デバイスのポインタ
		 */
		[[nodiscard]] ID3D12Device* device() const noexcept;

		//---------------------------------------------------------------------------------
		/**
		 * @brief	dxgi ファクトリーを取得する
		 * @return	dxgi ファクトリーのポインタ
		 */
		[[nodiscard]] IDXGIFactory4* dxgiFactory() const noexcept;

		//---------------------------------------------------------------------------------
		/**
		 * @brief	ディスプレイアダプターを取得する
		 * @return	ディスプレイアダプターのポインタ
		 */
		[[nodiscard]] IDXGIAdapter* displayAdapter() const noexcept;

	private:
		//---------------------------------------------------------------------------------
		/**
		 * @brief	コンストラクタ
		 */
		Device();

	private:
		class Impl;
		std::unique_ptr<Impl> impl_;		///< インプリメントクラスポインタ

	};
}
