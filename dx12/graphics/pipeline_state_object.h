#pragma once

#include "dx12/command_list.h"
#include "dx12/graphics/shader.h"

#include "utility/noncopyable.h"

namespace dx12::graphics {
	//---------------------------------------------------------------------------------
	/**
	 * @brief
	 * パイプラインステートオブジェクト
	 */
	class PipelineStateObject final : public utility::Noncopyable {

	public:
		//---------------------------------------------------------------------------------
		/**
		 * @brief	コンストラクタ
		 */
		PipelineStateObject() = default;

		//---------------------------------------------------------------------------------
		/**
		 * @brief	デストラクタ
		 */
		~PipelineStateObject() = default;

		//---------------------------------------------------------------------------------
		/**
		 * @brief	パイプラインステートオブジェクトを作成する
		 * @return	作成に成功した場合は true
		 */
		bool create() noexcept;

		//---------------------------------------------------------------------------------
		/**
		 * @brief	コマンドリストに設定する
		 * @param	commandList		設定先のコマンドリスト
		 */
		void setToCommandList(CommandList& commandList) noexcept;

	private:
		//---------------------------------------------------------------------------------
		/**
		 * @brief	パイプラインステートを作成する
		 * @return	作成に成功した場合は true
		 */
		bool createPipelineState() noexcept;

		//---------------------------------------------------------------------------------
		/**
		 * @brief	ルートシグネチャを作成する
		 * @return	作成に成功した場合は true
		 */
		bool createRootSignature() noexcept;


	private:
		Microsoft::WRL::ComPtr<ID3D12PipelineState>	pipelineState_ = {};	///< パイプラインステート
		Microsoft::WRL::ComPtr<ID3D12RootSignature>	rootSignature_ = {};	///< ルートシグネチャ
		std::unique_ptr<Shader>						shader_		   = {};	///< シェーダ

	};
}
