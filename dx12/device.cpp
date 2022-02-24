#include "dx12/device.h"
#include "window/window.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxgi.lib")

namespace dx12 {
	using namespace Microsoft::WRL;
	using namespace DirectX;

	//---------------------------------------------------------------------------------
	/**
	 * @brief
	 * DirectX12デバイス制御のインプリメントクラス
	 */
	class Device::Impl {
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
		~Impl() {
#if _DEBUG	
			ComPtr<ID3D12DebugDevice> debug;
			if (SUCCEEDED(device_->QueryInterface(debug.GetAddressOf())))
			{
				debug->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL);
			}
#endif
		}

		//---------------------------------------------------------------------------------
		/**
		 * @brief	デバイスを作成する
		 * @return	デバイスの作成に成功した場合は true
		 */
		bool create() noexcept {

#if _DEBUG	
			ComPtr<ID3D12Debug> debug;
			if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debug.GetAddressOf()))))
			{
				debug->EnableDebugLayer();
			}
#endif
			// ディスプレイアダプタ設定
			setDisplayAdapter();

			// デバイスの作成
			createDevice();

			// ディスプレイモードの確認
			checkDisplayMode();

			return true;
		}

		//---------------------------------------------------------------------------------
		/**
		 * @brief	ディスプレイアダプタの設定
		 * @return	ディスプレイの情報が正しく取得できた場合は true
		 */
		bool setDisplayAdapter() noexcept {
			ComPtr<IDXGIAdapter>	dxgiAdapter;
#if _DEBUG	
			CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(dxgiFactory_.GetAddressOf()));
#else
			CreateDXGIFactory1(IID_PPV_ARGS(dxgiFactory_.GetAddressOf()));
#endif

			// アダプタを列挙
			auto count = 0;
			while (dxgiFactory_->EnumAdapters(count, dxgiAdapter.GetAddressOf()) != DXGI_ERROR_NOT_FOUND) {
				DXGI_ADAPTER_DESC desc;
				dxgiAdapter->GetDesc(&desc);


				std::wstring wstr = desc.Description;
				auto wsize	= static_cast<int32_t>(wstr.size());
				auto size	= WideCharToMultiByte(CP_UTF8, 0, &wstr[0], wsize, nullptr, 0, nullptr, nullptr);
				std::string name(size, 0);
				WideCharToMultiByte(CP_UTF8, 0, &wstr[0], wsize, &name[0], size, nullptr, nullptr);
				TRACE(name.data());

				count++;
				dxgiAdapter.Reset();
			}

			if (count == 0) {
				ASSERT(false, "ディスプレイが見つからない");
				return false;
			}

			// ゼロ番目のアダプタ保存
			if (S_OK != dxgiFactory_->EnumAdapters(0, dxgiAdapter_.GetAddressOf())) {
				ASSERT(false, "ディスプレイの取得に失敗");
				return false;
			}

			return true;
		}

		//---------------------------------------------------------------------------------
		/**
		 * @brief	デバイス作成
		 * @return	作成出来た場合は true
		 */
		bool createDevice() noexcept {
			// デバイス作成
			auto res = D3D12CreateDevice(dxgiAdapter_.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(device_.GetAddressOf()));
			if (FAILED(res)) {
				ASSERT(false, "デバイス作成に失敗");
				return false;
			}

			return true;
		}

		//---------------------------------------------------------------------------------
		/**
		 * @brief	ディスプレイモードの確認
		 * @return	ディスプレイのモードが取得できれば true
		 */
		bool checkDisplayMode() noexcept {
			ComPtr<IDXGIOutput> output;
			auto hr = dxgiAdapter_->EnumOutputs(0, output.GetAddressOf());
			if (FAILED(hr)) { 
				ASSERT(false, "モニターのモード取得に失敗"); 
				return false;
			}

			uint32_t displayModeNum = 0;
			// ディスプレイモード数を取得
			hr = output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 0, &displayModeNum, 0);
			if (FAILED(hr)) { 
				ASSERT(false, "モニターのモード取得に失敗"); 
				return false;
			}

			// ディスプレイモードを列挙
			std::shared_ptr<DXGI_MODE_DESC> displayMode;
			displayMode.reset(new DXGI_MODE_DESC[displayModeNum], std::default_delete<DXGI_MODE_DESC[]>());
			hr = output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 0, &displayModeNum, displayMode.get());
			if (FAILED(hr)) { 
				ASSERT(false, "モニターのモード取得に失敗"); 
				return false;
			}

			for (uint32_t i = 0; i < displayModeNum; i++) {
				XMUINT2 size;
				size.x = displayMode.get()[i].Width;
				size.y = displayMode.get()[i].Height;

				auto find = std::find_if(adapterSizeList_.begin(), adapterSizeList_.end(),
					[&size](const auto& temp) {
						return (size.x == temp.x && size.y == temp.y);
					}
				);
				if (find == adapterSizeList_.end()) { adapterSizeList_.emplace_back(size); }
			}

			return true;
		}


		//---------------------------------------------------------------------------------
		/**
		 * @brief	デバイスを取得する
		 * @return	dx12 デバイスのポインタ
		 */
		ID3D12Device* device() const noexcept {
			return device_.Get();
		}

		//---------------------------------------------------------------------------------
		/**
		 * @brief	dxgi ファクトリーを取得する
		 * @return	dxgi ファクトリーのポインタ
		 */
		IDXGIFactory4* dxgiFactory() const noexcept {
			return dxgiFactory_.Get();
		}

		//---------------------------------------------------------------------------------
		/**
		 * @brief	ディスプレイアダプターを取得する
		 * @return	ディスプレイアダプターのポインタ
		 */
		IDXGIAdapter* displayAdapter() const noexcept {
			return dxgiAdapter_.Get();
		}

	private:
		ComPtr<ID3D12Device>	device_;				///< デバイス
		ComPtr<IDXGISwapChain3>	swapChain_;				///< スワップチェイン
		ComPtr<IDXGIFactory4>	dxgiFactory_;			///< DXGIを作成するファクトリー
		ComPtr<IDXGIAdapter>	dxgiAdapter_;			///< ディスプレイモード取得用アダプタ
		std::vector<XMUINT2>	adapterSizeList_;		///< 対応解像度のリスト
	};

	//---------------------------------------------------------------------------------
	/**
	 * @brief	デストラクタ
	 */
	Device::~Device() {
		impl_.reset();
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief	デバイスを作成する
	 * @return	正しく作成できた場合は true
	 */
	bool Device::create() noexcept {
		return impl_->create();
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief	デバイスを取得する
	 * @return	デバイスのポインタ
	 */
	ID3D12Device* Device::device() const noexcept {
		return impl_->device();
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief	dxgi ファクトリーを取得する
	 * @return	dxgi ファクトリーのポインタ
	 */
	IDXGIFactory4* Device::dxgiFactory() const noexcept {
		return impl_->dxgiFactory();
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief	ディスプレイアダプターを取得する
	 * @return	ディスプレイアダプターのポインタ
	 */
	IDXGIAdapter* Device::displayAdapter() const noexcept {
		return impl_->displayAdapter();
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief	コンストラクタ
	 */
	Device::Device() {
		impl_.reset(new Device::Impl());
	}


}

