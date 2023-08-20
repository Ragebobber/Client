#pragma once

#define GUI_WINDOW_WIDTH  600
#define GUI_WINDOW_HEIGHT 400
#define CLASS_NAME "Loader"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler ( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

// Data
static LPDIRECT3D9 g_pD3D = nullptr;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};
static LPDIRECT3DDEVICE9        g_pd3dDevice = nullptr;

static POINTS position = {};

// Forward declarations of helper functions
bool CreateDeviceD3D ( HWND hWnd );
void CleanupDeviceD3D ( );
void ResetDevice ( );
LRESULT CALLBACK WndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
void ResetDevice ( );
bool Render ( );

namespace ImGui
{
	inline void TextCentered ( std::string text ) {
		float win_width = ImGui::GetWindowSize ( ).x;
		float text_width = ImGui::CalcTextSize ( text.c_str ( ) ).x;

		// calculate the indentation that centers the text on one line, relative
		// to window left, regardless of the `ImGuiStyleVar_WindowPadding` value
		float text_indentation = (win_width - text_width) * 0.5f;

		// if text is too long to be drawn on one line, `text_indentation` can
		// become too small or even negative, so we check a minimum indentation
		float min_indentation = 20.0f;
		if ( text_indentation <= min_indentation ) {
			text_indentation = min_indentation;
		}

		ImGui::SameLine ( text_indentation );
		ImGui::PushTextWrapPos ( win_width - text_indentation );
		ImGui::TextWrapped ( text.c_str ( ) );
		ImGui::PopTextWrapPos ( );

	}

}