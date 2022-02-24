#include "window.h"
#include "utility/thread.h"
#include "input/input.h"

namespace window {

	namespace {
		constexpr uint32_t windowWidth = 1280;
		constexpr uint32_t windowHeight = 720;
		constexpr const char* appName = "ブログネタ";

		//---------------------------------------------------------------------------------
		/**
		 * @brief	ウィンドウスレッドのワーク関数
		 * @param	pParameter	インスタンスハンドル
		 * @return
		 */
		uint32_t windowWorker(void* pParameter) {
			Window::instance().worker((HINSTANCE*)pParameter);
			return 0;
		}

		//---------------------------------------------------------------------------------
		/**
		 * @brief	ウィンドウプロシージャのコールバック関数
		 * @return
		 */
		LRESULT CALLBACK wndProc(HWND windouHandle, UINT msg, WPARAM wParam, LPARAM lParam) {
			return Window::instance().msgProc(windouHandle, msg, wParam, lParam);
		}

	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief	ウィンドウの横幅を取得する
	 */
	uint32_t width() noexcept {
		return windowWidth;
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief	ウィンドウの縦幅を取得する
	 */
	uint32_t Height() noexcept {
		return windowHeight;
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief
	 * ウィンドウ制御インプリメントクラス
	 */
	class Window::Impl {
	public:

		/**
		 * @brief	ウィンドウスレッドのステータス列挙
		 */
		enum {
			INITIALIZE,
			WORK,
			END,
		};

	public:
		HWND 				windowHandle_;			///< ウィンドウハンドル
		HANDLE              createdEvent_;			///< 生成イベントシグナル
		uint8_t				status_;				///< 現在のステータス
		byte				keyState_[256];			///< キー入力情報
		utility::Thread		thread_;				///< ウィンドウ制御スレッド


	public:
		Impl()
			:windowHandle_(0)
			, status_(INITIALIZE) {
			createdEvent_ = CreateEvent(nullptr, true, false, "CREATE_WINDOW");
		}
		~Impl() = default;

		//---------------------------------------------------------------------------------
		/**
		 * @brief	ウィンドウスレッドの生成
		 * @param	instance	インスタンスハンドル
		 * @return	生成の成否
		 */
		HRESULT create(HINSTANCE instance) {

			thread_.start(windowWorker, &instance);

			return S_OK;
		}


		//---------------------------------------------------------------------------------
		/**
		 * @brief	ウィンドウの生成
		 * @param	instance	インスタンスハンドル
		 * @param	width		横幅
		 * @param	height		縦幅
		 * @param	name		ウィンドウ名
		 * @return	生成の成否
		 */
		HRESULT create(HINSTANCE instance, uint32_t width, uint32_t height, std::string_view name) noexcept {
			// ウィンドウの定義
			WNDCLASSEX  wc;
			ZeroMemory(&wc, sizeof(wc));
			wc.cbSize = sizeof(wc);
			wc.style = (CS_HREDRAW | CS_VREDRAW);
			wc.lpfnWndProc = wndProc;
			wc.hInstance = instance;
			wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
			wc.hCursor = LoadCursor(NULL, IDC_ARROW);
			wc.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
			wc.lpszClassName = name.data();
			wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
			RegisterClassEx(&wc);

			//ウィンドウの作成
			windowHandle_ = CreateWindow(wc.lpszClassName, wc.lpszClassName, WS_OVERLAPPEDWINDOW, 0, 0, width, height, 0, 0, instance, 0);
			if (!windowHandle_) { return E_FAIL; }

			//ウインドウの表示
			ShowWindow(windowHandle_, SW_SHOW);
			UpdateWindow(windowHandle_);

			// ウィンドウ作成終了イベントシグナルを設定
			SetEvent(createdEvent_);

			status_ = WORK;

			return S_OK;
		}

		//---------------------------------------------------------------------------------
		/**
		 * @brief	スレッドワーカー
		 * @param	instance	インスタンスハンドルのポインタ
		 */
		void worker(HINSTANCE* instance) {
			// メッセージループ
			MSG msg = {};
			while (msg.message != WM_QUIT) {
				if (Impl::INITIALIZE == status_) {
					create(*instance, windowWidth, windowHeight, appName);
				}
				else if (Impl::WORK == status_) {
					if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}

					GetKeyboardState(keyState_);
					input::Input::instance().updateKeyState(keyState_);

				}
			}

			status_ = END;
		}


		//---------------------------------------------------------------------------------
		/**
		 * @brief	ハンドルの取得
		 * @return	ウィンドウハンドル
		 */
		const HWND& Handle() const noexcept {
			return windowHandle_;
		}
	};

	//---------------------------------------------------------------------------------
	/**
	 * @brief	デストラクタ
	 */
	Window::~Window() {
		impl_.reset();
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief	ウィンドウプロシージャ
	 */
	LRESULT Window::msgProc(HWND handle, UINT msg, WPARAM wParam, LPARAM lParam) {
		switch (msg) {
		case WM_KEYDOWN:
			switch ((char)wParam) {
			case VK_ESCAPE://ESCキーで
				PostQuitMessage(0);
				break;
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		}
		return DefWindowProc(handle, msg, wParam, lParam);
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief	ウィンドウスレッドの生成
	 * @param	instance	インスタンスハンドル
	 * @return	生成の成否
	 */
	HRESULT Window::create(HINSTANCE instance) noexcept {
		return impl_->create(instance);
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief	スレッドワーカー
	 * @param	instance	インスタンスハンドルのポインタ
	 */
	void Window::worker(HINSTANCE* instance) noexcept {
		impl_->worker(instance);
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief	ウィンドウ生成終了までの処理待機
	 */
	void Window::wait() noexcept {
		// SetEventまで待機
		WaitForSingleObject(impl_->createdEvent_, INFINITE);
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief	ウィンドウハンドルの取得
	 * @return	ウィンドウハンドル
	 */
	const HWND& Window::handle() const noexcept {
		return impl_->Handle();
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief	ウィンドウが終了しているか否かの取得
	 * @return	終了していればtrue
	 */
	bool Window::isEnd() const noexcept {
		return (impl_->status_ == Impl::END);
	}

	//---------------------------------------------------------------------------------
	/**
	 * @brief	コンストラクタ
	 */
	Window::Window() {
		impl_.reset(new Window::Impl());
	}
}
