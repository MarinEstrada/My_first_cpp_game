// we have to create entry point for communication with windows
// we will be making platform dependent & platform independent code (this portion == platform dependent
// use wnmain msdn

//note.. for some reason this project is being built with uniqocde instead of ANSI 
#include "utils.cpp"
#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
//added to implement font
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

static bool running = true;

struct Render_State {
	int height, width;
	void* memory;
	BITMAPINFO bitmap_info;
};

static Render_State render_state;
#include "platform_common.cpp"
#include "renderer.cpp" // including renderer file after render_state so that renderer file can use it, this is bc unity build
#include "game.cpp"

// macros bassically just replace text
//macro: part of a switch case
#define process_button(b, vk)\
case vk: {\
input.buttons[b].has_changed = is_down != input.buttons[b].is_down; \
input.buttons[b].is_down = is_down; \
break;\
}

//WNDPROC Wndproc;

//LRESULT Wndproc(HWND unnamedParam1, UINT unnamedParam2, WPARAM unnamedParam3, LPARAM unnamedParam4);

LRESULT CALLBACK window_callback  (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	//return DefWindowProc(hwnd, uMsg, wParam, lParam);

	LRESULT result = 0;

	switch (uMsg) {
		case WM_CLOSE:
		case WM_DESTROY: {
			running = false;
		} break;

		case WM_SIZE: {
			RECT rect;
			GetClientRect(hwnd, &rect);
			render_state.width = rect.right - rect.left;
			render_state.height = rect.bottom - rect.top;

			//Buffer will be width * hight, enough to hold the color for each pixel!
			// we will use 32 bits, & RGB format (1 byte per colour, & 1 byte having wasted space... thats okay... 4*8 bits = 32 bits

			int buffer_size = render_state.width * render_state.height * sizeof(uint32);

			// to reserve space on heap we usually use malloc. But here we are going to use more window specific func since we on OS integration layer
			// buffer memory == void ptr

			// NOTE: if ptr already has valid memory, free that memory before allocating new memory
			if (render_state.memory) VirtualFree(render_state.memory, 0, MEM_RELEASE);
			// allocating memory
			render_state.memory = VirtualAlloc(0, buffer_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

			render_state.bitmap_info.bmiHeader.biSize = sizeof(render_state.bitmap_info.bmiHeader);
			render_state.bitmap_info.bmiHeader.biWidth = render_state.width;
			render_state.bitmap_info.bmiHeader.biHeight = render_state.height;
			render_state.bitmap_info.bmiHeader.biPlanes = 1; // why? mystery :eyes:
			render_state.bitmap_info.bmiHeader.biBitCount = 32;
			render_state.bitmap_info.bmiHeader.biCompression = BI_RGB;

		} break;

		default: {
			result = DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}

	return result;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nShowCmd) {
//int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR lpCmdLine, _In_ int nShowCmd) {
	// we want to draw a window, 3 things needed
	//1) Create a window class
	WNDCLASS window_class = {}; // starts out empty
	window_class.style = CS_HREDRAW | CS_VREDRAW; //ensure window is redrawsn both veritically & horizontally whenever needed
	window_class.lpszClassName = L"Game Window Class"; // is visiable to the normal user
	window_class.lpfnWndProc = window_callback; //call back function is how windows passes messages back to us (wether its active, inactive, etc)
	//2) register the class
	RegisterClass(&window_class);
	//3) create the actual instance
	HWND window = CreateWindow(window_class.lpszClassName, L"My First Game!", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720, 0, 0, hInstance, 0);

	HDC hdc = GetDC(window); // this is where we get the device context (ref windows use to draw to window)
	//load_font("../arial.ttf", 24.f);
	load_font("../PixelifySans-Regular.ttf", 24.f);

	//input struct in platform_common file
	Input input = {};

	// how much time elapsed in one frame? Assume for first frame 60fps o 0.016666 s/f
	float delta_time = 0.016666f;
	LARGE_INTEGER frame_begin_time;
	QueryPerformanceCounter(&frame_begin_time); //function that accepts a pointer, get time at start of frame

	float performance_frequency;
	{
		LARGE_INTEGER perf;
		QueryPerformanceFrequency(&perf); //returns how many cycles computer runs in one second
		performance_frequency = (float)perf.QuadPart;
	}

	while (running) {
		// 3 parts to this
		// Input
		MSG message;

		// want to change status of input has changed to false because we only want input to last for a single frame
		// (this allows to hold the button down for continous movement)
		for (int i = 0; i < BUTTON_COUNT; i++) {

		}

		while (PeekMessage(&message, window, 0, 0, PM_REMOVE)) {

			switch (message.message) {
				case WM_KEYUP:
				case WM_KEYDOWN: {
					uint32 vk_code = (uint32)message.wParam;
					bool is_down = ((message.lParam & (1 << 31)) == 0);

					// for button want to check 2 things: is button up or down? Has button changed state?
					switch (vk_code) {
						process_button(BUTTON_UP, VK_UP);
						process_button(BUTTON_DOWN, VK_DOWN);
						process_button(BUTTON_RIGHT, VK_RIGHT);
						process_button(BUTTON_LEFT, VK_LEFT);
						process_button(BUTTON_SHIFT, VK_SHIFT);
						process_button(BUTTON_CTRL, VK_CONTROL);
						process_button(BUTTON_W, 'W');
						process_button(BUTTON_S, 'S');

						default: 
							break;
					}

					break;
				}

				default: {
					TranslateMessage(&message);
					DispatchMessage(&message);
				}

			}

		}

		// Simulate (eg color the pixels)
		// called in diff file -> do unity build
		simulate_game(&input, delta_time);

		// Render	-> once memory has been allocated for window (in window_callback) need to use it, use device context (hdc var)
		// if rendering with allocated mem without change, will be black screen! Because each pixel of allocated memory is 0x0000
		StretchDIBits(hdc, 0, 0, render_state.width, render_state.height, 0, 0, render_state.width, render_state.height, render_state.memory, &render_state.bitmap_info, DIB_RGB_COLORS, SRCCOPY);

		LARGE_INTEGER frame_end_time;
		QueryPerformanceCounter(&frame_end_time); //get time at end of frame
		delta_time = (float)(frame_end_time.QuadPart - frame_begin_time.QuadPart) / performance_frequency; // calc new delta time, which is seconds / frame
		frame_begin_time = frame_end_time; //update begin_time for next frame calc
	}

	return 0;

}