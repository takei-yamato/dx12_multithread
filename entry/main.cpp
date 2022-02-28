
#include "window/window.h"

#include "utility/time_counter.h"

#include "dx12/command_queue.h"
#include "dx12/command_list.h"
#include "dx12/device.h"
#include "dx12/swap_chain.h"
#include "dx12/fence.h"

#include "dx12/graphics/pipeline_state_object.h"

#include "dx12/resource/constant_buffer.h"
#include "dx12/resource/mesh.h"
#include "dx12/resource/frame_buffer.h"

#include <array>
#include <random>
#include <ppl.h>

using namespace dx12;

namespace {
	// コマンドリスト数
	constexpr uint32_t commandListNum = 4;
	// オブジェクト数
	constexpr uint32_t objectNum = 24000;

	// コンスタントバッファのフォーマット
	struct ConstantBufferFormat {
		DirectX::XMMATRIX world = {};
		DirectX::XMMATRIX viewProj = {};
		DirectX::XMFLOAT4 color = {};
	};

	// 頂点フォーマット
	struct Vertex {
		DirectX::XMFLOAT3 pos = {};
		DirectX::XMFLOAT2 uv = {};
	};

	// 頂点データ
	Vertex vertexData[] = {
		{ {-0.5f, 0.5f ,  0.0f},	{0, 0}	},
		{ {0.5f,  0.5f ,  0.0f},	{1, 1}  },
		{ {-0.5f, -0.5f,  0.0f},	{0, 1}  },
		{ {0.5f,  -0.5f,  0.0f},	{1, 0}  }
	};

	// インデックスデータ
	uint16_t indexData[] = {
		0, 1, 2,
		2, 1, 3,
	};

	// カメラ
	DirectX::XMFLOAT3 eye(0.0f, 0.0f, -300.0f);
	DirectX::XMFLOAT3 dir(0.0f, 0.0f, 1.0f);
	DirectX::XMFLOAT3 up(0.0f, 1.0f, 0.0f);
	DirectX::XMMATRIX view = {};
	DirectX::XMMATRIX proj = {};

	// アスペクト比
	float aspect = static_cast<float>(window::width()) / static_cast<float>(window::Height());

	// 各オブジェクトのワールド行列
	DirectX::XMMATRIX world[objectNum] = {};
	
	// 各オブジェクトのカラー
	DirectX::XMFLOAT4 color[objectNum] = {};

	// フレームバッファ(二つ分)
	resource::FrameBuffer frameBuffer(2);

	// メッシュ
	resource::Mesh mesh = {};

	// コンスタントバッファ
	resource::ConstantBuffer<ConstantBufferFormat, objectNum> constantBuffer = {};

	// パイプラインステートオブジェクト
	graphics::PipelineStateObject pso = {};

	// コマンドキュー
	CommandQueue commandQueue = {};

	// フェンス
	Fence fence = {};

	// コマンドリスト
	CommandList commandListBegin				= {};
	CommandList commandLists[commandListNum]	= {};
	CommandList commandListEnd					= {};
}

namespace {
	//---------------------------------------------------------------------------------
	/**
	 * @brief	アプリケーションの更新処理を行う
	 */
	bool appUpdate() noexcept {
		if (window::Window::instance().isEnd()) {
			return false;
		}

		{
			TIME_CHECK_SCORP("更新時間");

			// 描画開始
			{
				commandListBegin.reset();
				frameBuffer.startRendering(commandListBegin);
				commandListBegin.get()->Close();
			}

			// 各メッシュ描画
			{
				concurrency::parallel_for<uint32_t>(0, commandListNum, [&](auto index) {
					commandLists[index].reset();

					frameBuffer.setToRenderTarget(commandLists[index]);
					pso.setToCommandList(commandLists[index]);
					mesh.setToCommandList(commandLists[index]);

					auto start = index * (objectNum / commandListNum);
					auto end = start + (objectNum / commandListNum);
					for (auto i = start; i < end; ++i) {
						// 描画
						constantBuffer.setToCommandList(commandLists[index], i);
						commandLists[index].get()->DrawIndexedInstanced(6, 1, 0, 0, 0);
					}
					commandLists[index].get()->Close();
					});
			}

			// 描画終了
			{
				commandListEnd.reset();
				frameBuffer.finishRendering(commandListEnd);
				commandListEnd.get()->Close();
			}

			// コマンドリスト実行
			{
				std::array<ID3D12CommandList*, commandListNum + 2> lists;

				// 描画開始
				lists[0] = commandListBegin.get();
				// 各メッシュ描画
				for (auto i = 0; i < commandListNum; ++i) {
					lists[i + 1] = commandLists[i].get();
				}
				// 描画終了
				lists[commandListNum + 1] = commandListEnd.get();

				// 実行
				commandQueue.get()->ExecuteCommandLists(lists.size(), static_cast<ID3D12CommandList**>(lists.data()));
				SwapChain::instance().present();

				// フレームバッファのインデックスを更新する
				frameBuffer.updateBufferIndex(SwapChain::instance().currentBufferIndex());
			}
		}

		// 時間表示
		TIME_PRINT("");

		// フェンス設定
		{
			fence.get()->Signal(0);
			commandQueue.get()->Signal(fence.get(), 1);
		}

		// GPU処理が全て終了するまでCPUを待たせる
		{
			auto event = CreateEvent(nullptr, false, false, "WAIT_GPU");
			fence.get()->SetEventOnCompletion(1, event);
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}

		return true;
	}

}


//---------------------------------------------------------------------------------
/**
 * @brief	エントリー関数
 */
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, INT) {

	std::random_device rd;
	std::default_random_engine re(rd());
	std::uniform_real_distribution<float> distr(0, 255);

	{
		// メモリリークチェック
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		{
			TRACE("アプリケーション開始");

			// ウィンドウ生成
			window::Window::instance().create(hInstance);
			window::Window::instance().wait();

			// dx12 デバイスを作成する
			Device::instance().create();

			// コマンドキューを作成する
			commandQueue.create();

			// スワップチェインを作成する
			SwapChain::instance().create(commandQueue, frameBuffer);

			// メッシュを作成する
			mesh.createVertexBuffer(vertexData);
			mesh.createIndexBuffer(indexData);

			// コンスタントバッファを作成する
			constantBuffer.createBuffer();

			// ビュー行列
			view = DirectX::XMMatrixLookToLH(XMLoadFloat3(&eye), XMLoadFloat3(&dir), XMLoadFloat3(&up));
			// プロジェクション行列
			proj = DirectX::XMMatrixPerspectiveFovLH(3.14159f / 4.f, aspect, 0.1f, 1000.0f);
			// ワールド行列
			for (auto& w : world) {
				w = DirectX::XMMatrixTranslation(distr(re) - 127.0f, distr(re) * 0.5f - 63.0f, 0);
			}
			// カラー
			for (auto& c : color) {
				c = DirectX::XMFLOAT4(distr(re) / 255.0f, distr(re) / 255.0f, distr(re) / 255.0f, 1.0f);
			}

			// コンスタントバッファの内容を予め設定しておく
			concurrency::parallel_for<uint32_t>(0, commandListNum, [&](auto index) {
				auto start	= index * (objectNum / commandListNum);
				auto end	= start + (objectNum / commandListNum);
				for (auto i = start; i < end; ++i) {
					constantBuffer[i].world		= DirectX::XMMatrixTranspose(world[i]);
					constantBuffer[i].viewProj	= DirectX::XMMatrixTranspose(view * proj);
					constantBuffer[i].color		= color[i];
				}
			});

			// フェンス（CPUとGPUの同期オブジェクト）を作成する
			fence.create();

			// 各コマンドリストを作成する
			commandListBegin.create();
			for (auto& commandList : commandLists) {
				commandList.create();
			}
			commandListEnd.create();

			// パイプラインステートオブジェクトを作成する
			pso.create();

			// アプリケーションループ
			while (appUpdate()) {
			}
		}
	}

	return 0;
}
