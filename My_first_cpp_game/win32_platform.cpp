// we have to create entry point for communication with windows
// we will be making platform dependent & platform independent code (this portion == platform dependent
// use wnmain msdn

//note.. for some reason this project is being built with uniqocde instead of ANSI 

#include <windows.h>
#include <iostream>

bool running = true;

void* buffer_memory;
int buffer_width;
int buffer_height;
BITMAPINFO buffer_bitmap_info;


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
			buffer_width = rect.right - rect.left;
			buffer_height = rect.bottom - rect.top;

			//Buffer will be width * hight, enough to hold the color for each pixel!
			// we will use 32 bits, & RGB format (1 byte per colour, & 1 byte having wasted space... thats okay... 4*8 bits = 32 bits

			int buffer_size = buffer_width * buffer_height * sizeof(unsigned int);

			// to reserve space on heap we usually use malloc. But here we are going to use more window specific func since we on OS integration layer
			// buffer memory == void ptr

			// NOTE: if ptr already has valid memory, free that memory before allocating new memory
			if (buffer_memory) VirtualFree(buffer_memory, 0, MEM_RELEASE);
			// allocating memory
			buffer_memory = VirtualAlloc(0, buffer_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

			buffer_bitmap_info.bmiHeader.biSize = sizeof(buffer_bitmap_info.bmiHeader);
			buffer_bitmap_info.bmiHeader.biWidth = buffer_width;
			buffer_bitmap_info.bmiHeader.biHeight = buffer_height;
			buffer_bitmap_info.bmiHeader.biPlanes = 1; // why? mystery :eyes:
			buffer_bitmap_info.bmiHeader.biBitCount = 32;
			buffer_bitmap_info.bmiHeader.biCompression = BI_RGB;

		} break;

		default: {
			result = DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}

	return result;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nShowCmd) {
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

	while (running) {
		// 3 parts to this
		// Input
		MSG message;
		while (PeekMessage(&message, window, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		// Simulate

		// Render	-> once memory has been allocated for window (in window_callback) need to use it, use device context (hdc var)
		// if rendering with allocated mem without change, will be black screen! Because each pixel of allocated memory is x0000
		StretchDIBits(hdc, 0, 0, buffer_width, buffer_height, 0, 0, buffer_width, buffer_height, buffer_memory, &buffer_bitmap_info, DIB_RGB_COLORS, SRCCOPY);


	}

	return 0;

}