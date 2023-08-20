#include "SDK.h"


int WINAPI WinMain ( HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow )
{
    // Create application window
    WNDCLASSEXW wc = { sizeof ( wc ), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle ( nullptr ), nullptr, nullptr, nullptr, nullptr, L"LoaderWSclass", nullptr};
    ::RegisterClassExW ( &wc );
	HWND hwnd = ::CreateWindowW ( wc.lpszClassName, S_HWIND_NAME, WS_POPUP, 100, 100, GUI_WINDOW_WIDTH, GUI_WINDOW_HEIGHT, nullptr, nullptr, wc.hInstance, nullptr );

    // Initialize Direct3D
     if( !CreateDeviceD3D ( hwnd  )) {
        CleanupDeviceD3D ( );
        ::UnregisterClassW ( wc.lpszClassName, wc.hInstance );
       return 1;
    }
	
    // Show the window
    ::ShowWindow ( hwnd, SW_SHOWDEFAULT );
    ::UpdateWindow ( hwnd );

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION ( );
    ImGui::CreateContext ( );
    ImGuiIO& io = ImGui::GetIO ( ); (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	
    // Setup Dear ImGui style
    ImGui::StyleColorsDark ( );
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init ( hwnd );
    ImGui_ImplDX9_Init ( g_pd3dDevice );

	Render ( );
	
	return 0;
}


LRESULT CALLBACK WndProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) {
	if ( ImGui_ImplWin32_WndProcHandler ( hWnd, msg, wParam, lParam ) )
		return true;

	switch ( msg ) {
	case WM_SIZE:
		if ( wParam == SIZE_MINIMIZED )
			return 0;
		if ( g_pD3D ) {
			g_d3dpp.BackBufferWidth = LOWORD ( lParam );
			g_d3dpp.BackBufferHeight = HIWORD ( wParam );
		}
		return 0;
	case WM_SYSCOMMAND:
		if ( (wParam & 0xfff0) == SC_KEYMENU ) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage ( 0 );
		return 0;
	case WM_MOUSEMOVE:
		if ( wParam == MK_LBUTTON ) {
			const auto points = MAKEPOINTS ( lParam );
			auto rect = RECT {};
			GetWindowRect ( hWnd, &rect );
			rect.left += points.x - position.x;
			rect.top += points.y - position.y;

			if ( position.x >= 0 &&
				position.x <= (SHORT) g_d3dpp.BackBufferWidth &&
				position.y >= 0 &&
				position.y <= 25 ) //if you want to move only by the top put 30 or lessg_d3dpp.BackBufferHeight 
			{
				SetWindowPos ( hWnd, HWND_TOPMOST, rect.left, rect.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER );
			}
		}
		return 0;
	case WM_LBUTTONDOWN:
		position = MAKEPOINTS ( lParam );
		return 0;
	}
	return ::DefWindowProcW ( hWnd, msg, wParam, lParam );
}

bool CreateDeviceD3D ( HWND hWnd ) {
    if ( (g_pD3D = Direct3DCreate9 ( D3D_SDK_VERSION )) == nullptr )
        return false;

    // Create the D3DDevice
    ZeroMemory ( &g_d3dpp, sizeof ( g_d3dpp ) );
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
    if ( g_pD3D->CreateDevice ( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice ) < 0 )
        return false;

    return true;
}

void CleanupDeviceD3D ( ) {
    if ( g_pd3dDevice ) { g_pd3dDevice->Release ( ); g_pd3dDevice = nullptr; }
    if ( g_pD3D ) { g_pD3D->Release ( ); g_pD3D = nullptr; }
}

void ResetDevice ( ) {
    ImGui_ImplDX9_InvalidateDeviceObjects ( );
    HRESULT hr = g_pd3dDevice->Reset ( &g_d3dpp );
    if ( hr == D3DERR_INVALIDCALL )
        IM_ASSERT ( 0 );
    ImGui_ImplDX9_CreateDeviceObjects ( );
}


bool Render ( ) {
	// Main loop
	OBF_BEGIN
	bool done = false;
	bool showWindow = true;
	bool showLogin = true;
	int selected = -1;
	ImVec4 clearColor = ImVec4 ( 0.45f, 0.55f, 0.60f, 1.00f );
	std::string login;
	std::string password;
	static ImGuiWindowFlags winFlags = {
	   ImGuiWindowFlags_NoTitleBar |
	   ImGuiWindowFlags_NoResize |
	   ImGuiWindowFlags_NoSavedSettings |
	   ImGuiWindowFlags_NoMove |
	   ImGuiWindowFlags_NoCollapse };
	
	WHILE ( !done ) 
		// Poll and handle messages (inputs, window resize, etc.)
		// See the WndProc() function below for our to dispatch events to the Win32 backend.
		MSG msg;
		WHILE ( ::PeekMessage ( &msg, nullptr, 0U, 0U, PM_REMOVE ) ) 
			::TranslateMessage ( &msg );
			::DispatchMessage ( &msg );
			IF ( msg.message == WM_QUIT )
				done = true;
			ENDIF

		ENDWHILE;

		IF ( done )
			BREAK;
		ENDIF

		// Start the Dear ImGui frame
		ImGui_ImplDX9_NewFrame ( );
		ImGui_ImplWin32_NewFrame ( );
		ImGui::NewFrame ( );

		ImGui::SetNextWindowPos ( { 0,  0 } );
		ImGui::SetNextWindowSize ( { GUI_WINDOW_WIDTH,  GUI_WINDOW_HEIGHT } );

		IF ( (ImGui::Begin ( S_MENU_BEGIN_NAME, &showWindow, winFlags )) ) {
			ImGui::TextCentered ( S_MENU_HEAD_TEXT_NAME );
			ImGui::SetCursorPos ( { ImGui::GetContentRegionAvail ( ).x,3 } );
			ImGui::TextDisabled ( S_MENU_CLOSE_TEXT_NAME );
			IF ( ImGui::IsItemClicked ( ) ) {
				exit ( 0 );
			}
			ENDIF
			ImGui::Dummy ( ImVec2 ( 0.0f, 2.0f ) );
			ImGui::Separator ( );

			IF ( showLogin || !apiConnector->isConnected ) {
				ImGui::SetCursorPosY ( ImGui::GetContentRegionAvail ( ).y / 3 );
				ImGui::BeginChild ( S_MENU_CHILD1_NAME, { 0 ,ImGui::GetContentRegionAvail ( ).y / 2 }, true );
				{
					ImGui::SetCursorPosX ( ImGui::GetContentRegionAvail ( ).x * 0.2f );
					ImGui::Text ( S_MENU_LOGIN_NAME );

					ImGui::SetCursorPosX ( ImGui::GetContentRegionAvail ( ).x * 0.2f );
					ImGui::InputText ( S_MENU_INPUT1_NAME, &dto->loginReq->login );

					ImGui::Dummy ( ImVec2 ( 0.0f, 7.0f ) );

					ImGui::SetCursorPosX ( ImGui::GetContentRegionAvail ( ).x * 0.2f );
					ImGui::Text ( S_MENU_PASSWORD_NAME );

					ImGui::SetCursorPosX ( ImGui::GetContentRegionAvail ( ).x * 0.2f );
					ImGui::InputText ( S_MENU_INPUT2_NAME, & dto->loginReq->password, ImGuiInputTextFlags_Password );

					ImGui::Dummy ( ImVec2 ( 0.0f, 4.0f ) );
					ImGui::SetCursorPosX ( (ImGui::GetContentRegionAvail ( ).x * 0.5f) - 90 );
					IF ( ImGui::Button ( S_MENU_BTN1_NAME, { 200,0 } ) ) {
						showLogin = !apiConnector->login ( );
					}
					ENDIF
				}
				ImGui::EndChild ( );
			}
			ELSE {
				ImGui::BeginChild ( S_MENU_CHILD2_NAME, { 0,ImGui::GetWindowHeight ( ) - 60 } );
				{
					ImGui::BeginChild ( S_MENU_CHILD3_NAME, { ImGui::GetContentRegionAvail ( ).x / 2.0f,ImGui::GetContentRegionAvail ( ).y } );
					{
						ImGui::Text ( S_MENU_PRODUCT_NAME );
						ImGui::Dummy ( ImVec2 ( 0.0f, 5.0f ) );
						for ( auto item : apiConnector->getUserSubs ( ) ) {
							IF ( ImGui::Selectable ( item.second.c_str ( ), selected == item.first ) ) {
								selected = item.first;
							}
							ENDIF
							ImGui::Separator ( );
						}
					}
					ImGui::EndChild ( );

					ImGui::SameLine ( );
					ImGui::SeparatorEx ( ImGuiSeparatorFlags_Vertical );
					ImGui::SameLine ( );

					ImGui::BeginChild ( S_MENU_CHILD4_NAME, { ImGui::GetContentRegionAvail ( ).x / 1.0f,ImGui::GetContentRegionAvail ( ).y } );
					{
						ImGui::Text ( S_MENU_INFO_NAME );
						ImGui::Dummy ( ImVec2 ( 0.0f, 5.0f ) );

						Sub* curSub = apiConnector->getSubInfo ( selected );
						IF ( curSub ) {
							ImGui::Text ( S_MENU_EX_DATE_NAME, curSub->expirationDate.c_str ( ) );
							ImGui::TextColored ( curSub->active ? ImColor ( 0, 255, 0 ) : ImColor ( 255, 0, 0 ),  curSub->active ? S_MENU_ACTIVE_NAME : S_MENU_EXPIRED_NAME );
							ImGui::Text ( S_MENU_DESCR_NAME, curSub->product->description.c_str ( ) );
							ImGui::SetCursorPosY ( ImGui::GetContentRegionMax ( ).y - ImGui::GetTextLineHeight ( ) - 15.f );

							IF ( !apiConnector->isConnected ) {
								ImGui::PushItemFlag ( ImGuiItemFlags_Disabled, true );
								ImGui::PushStyleVar ( ImGuiStyleVar_Alpha, ImGui::GetStyle ( ).Alpha * 0.5f );
							}
							ENDIF
							IF ( ImGui::Button ( S_MENU_INJECT_NAME, { ImGui::GetContentRegionAvail ( ).x / 1.0f,0 } ) ) {
								injection->inject ( curSub );

							}
							ENDIF
							IF ( !apiConnector->isConnected ) {
								ImGui::PopItemFlag ( );
								ImGui::PopStyleVar ( );
							}
							ENDIF

						}
						ENDIF
					}
					ImGui::EndChild ( );
				}
				ImGui::EndChild ( );
			}
				ENDIF


			ImGui::SetCursorPosY ( ImGui::GetWindowHeight ( ) - ImGui::GetTextLineHeight ( ) - 15.f );
			ImGui::Separator ( );
			ImGui::Text ( apiConnector->isConnected ? S_MENU_CONNECTED_NAME : S_MENU_ERROR_CONNECTED_NAME );
			IF ( !apiConnector->message.empty ( ) ) {
				ImGui::SameLine ( );
				ImGui::Text ( S_MENU_ERROR_NAME, apiConnector->message.c_str ( ) );
			}
			ENDIF
			ImGui::End ( );
		}
		ENDIF

		// Rendering
		ImGui::EndFrame ( );
		g_pd3dDevice->SetRenderState ( D3DRS_ZENABLE, FALSE );
		g_pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE, FALSE );
		g_pd3dDevice->SetRenderState ( D3DRS_SCISSORTESTENABLE, FALSE );
		D3DCOLOR clear_col_dx = D3DCOLOR_RGBA ( (int) (clearColor.x * clearColor.w * 255.0f), (int) (clearColor.y * clearColor.w * 255.0f), (int) (clearColor.z * clearColor.w * 255.0f), (int) (clearColor.w * 255.0f) );
		g_pd3dDevice->Clear ( 0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0 );
		IF ( g_pd3dDevice->BeginScene ( ) >= 0 ) {
			ImGui::Render ( );
			ImGui_ImplDX9_RenderDrawData ( ImGui::GetDrawData ( ) );
			g_pd3dDevice->EndScene ( );
		}
		ENDIF
		HRESULT result = g_pd3dDevice->Present ( nullptr, nullptr, nullptr, nullptr );

		// Handle loss of D3D9 device
		IF ( result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel ( ) == D3DERR_DEVICENOTRESET )
			ResetDevice ( );
		END
	
	ENDWHILE
	RETURN(true)
	OBF_END
}
