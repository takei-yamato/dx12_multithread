#include "dx12/graphics/shader.h"
#include "dx12/device.h"

#include <D3Dcompiler.h>
#pragma comment(lib,"d3dcompiler.lib")

namespace dx12::graphics {

	//---------------------------------------------------------------------------------
	/**
	 * @brief	コンストラクタ
	 * @param	filePath	ファイルパス( 「asset/」から開始の相対パス )
	 */
	Shader::Shader(std::string_view filePath) {
		create(filePath);
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief	頂点シェーダを取得する
	 * @return	頂点シェーダのデータ
	 */
	ID3DBlob* Shader::vertexShader() const noexcept {
		return vertexShader_.Get();
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief	ピクセルシェーダを取得する
	 * @return	ピクセルシェーダのデータ
	 */
	ID3DBlob* Shader::pixelShader() const noexcept {
		return pixelShader_.Get();
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief	シェーダを作成する
	 * @param	filePath	ファイルパス
	 * @return	作成に成功した場合は true
	 */
	bool Shader::create(std::string_view filePath) noexcept {

		// シェーダ作成
		Microsoft::WRL::ComPtr<ID3DBlob> error;

		auto temp = std::wstring(filePath.begin(), filePath.end());

		auto res = D3DCompileFromFile(temp.data(), nullptr, nullptr, "vs", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, vertexShader_.GetAddressOf(), error.GetAddressOf());
		if (FAILED(res)) {
			char* p = static_cast<char*>(error->GetBufferPointer());
			ASSERT(false, p);
			return false;
		}

		res = D3DCompileFromFile(temp.data(), nullptr, nullptr, "ps", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, pixelShader_.GetAddressOf(), error.GetAddressOf());
		if (FAILED(res)) {
			char* p = static_cast<char*>(error->GetBufferPointer());
			ASSERT(false, p);
			return false;
		}

		return true;
	}


}

