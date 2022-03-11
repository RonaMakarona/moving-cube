#include "utilities.c"
#include <Windows.h>
#include <stdbool.h>
#include <stdint.h>

static bool running = true;

struct Render_State {
	int height;
	int width;
	void* memory;

	BITMAPINFO bitmapinfo;
};

static struct Render_State render_state;

#include "platform_common.c"
#include "renderer.c"
#include "game.c"

HBITMAP hLogoImage;

void load_image() {
	hLogoImage = (HBITMAP)LoadImageW(NULL, L"Kolobok1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
}

LRESULT CALLBACK window_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	LRESULT result = 0;
	switch (uMsg)
	{
		case WM_CLOSE:
		case WM_DESTROY: {
			running = false;
		} break;
	
		case WM_SIZE: {
			RECT rect;
			GetClientRect(hwnd, &rect);
			render_state.width = rect.right - rect.left;
			render_state.height = rect.bottom - rect.top;

			int buffer_size = render_state.width * render_state.height * sizeof(unsigned int);

			if (render_state.memory) { 
				VirtualFree(render_state.memory, 0, MEM_RELEASE); 
			}

			render_state.memory = VirtualAlloc(0, buffer_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

			render_state.bitmapinfo.bmiHeader.biSize = sizeof(render_state.bitmapinfo.bmiHeader);
			render_state.bitmapinfo.bmiHeader.biWidth = render_state.width;
			render_state.bitmapinfo.bmiHeader.biHeight = render_state.height;
			render_state.bitmapinfo.bmiHeader.biPlanes = 1;
			render_state.bitmapinfo.bmiHeader.biBitCount = 32;
			render_state.bitmapinfo.bmiHeader.biCompression = BI_RGB;
		}

		default: {
			result = DefWindowProc(hwnd, uMsg, wParam, lParam);
		}

	}

	return result;
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

	//Create Window Class
	WNDCLASS window_class = {""};
	window_class.style = CS_HREDRAW | CS_VREDRAW;
	window_class.lpszClassName = "Game Window Class";
	window_class.lpfnWndProc = window_callback;

	//Register Class
	RegisterClass(&window_class);

	//Create Window
	HWND window = CreateWindowA(window_class.lpszClassName, "My First Game!", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720, 0, 0, hInstance, 0);
	HDC hdc = GetDC(window);


	struct Input input;

	float delta_time = 0.16666f; //Time elapsed in one frame, assuming its 60fps for the first frame
	LARGE_INTEGER frame_begin_time;
	QueryPerformanceCounter(&frame_begin_time); //Time in the beggining of the frame

	float performance_frequency; { //cycles per sec
		LARGE_INTEGER perf;
		QueryPerformanceFrequency(&perf);
		performance_frequency = (float)perf.QuadPart;
	}

	for (int i = 0; i < BUTTON_COUNT; i++) {
		input.buttons[i].is_down = false;
	}

	while (running) {
		//Input
		load_image;
		MSG message;
		for (int i = 0; i < BUTTON_COUNT; i++) {
			input.buttons[i].changed = false;
		}

		uint32_t vk_code;
		while (PeekMessage(&message, window, 0, 0, PM_REMOVE)) {
			switch (message.message)
			{
			case WM_KEYUP:
			case WM_KEYDOWN:
				vk_code = (unsigned int)message.wParam;
				bool is_down = ((message.lParam & (1 << 31)) == 0); // lParam bits table (https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-keyup)
																	// meaning, only the first bit should be on (look at table)

//macro
#define process_button(b, vk)\
case vk: {\
input.buttons[b].is_down = is_down;\
input.buttons[b].changed = true;\
}break;

				switch (vk_code) //stands for virtual key
				{
					process_button(BUTTON_UP, VK_UP);
					process_button(BUTTON_DOWN, VK_DOWN);
					process_button(BUTTON_LEFT, VK_LEFT);
					process_button(BUTTON_RIGHT, VK_RIGHT);

				} break;

			default: {
				TranslateMessage(&message);
				DispatchMessage(&message);
			}
			}
		}
		//Simulate
		simulate_game(&input, delta_time);

		//Render
		StretchDIBits(hdc, 0, 0, render_state.width, render_state.height, 0, 0, render_state.width, render_state.height, render_state.memory, &render_state.bitmapinfo, DIB_RGB_COLORS, SRCCOPY);

		LARGE_INTEGER frame_end_time;
		QueryPerformanceCounter(&frame_end_time); //Time in the end of the frame


		delta_time = (float)(frame_end_time.QuadPart - frame_begin_time.QuadPart) / performance_frequency; //time in second
		frame_begin_time = frame_end_time;
	}
}
