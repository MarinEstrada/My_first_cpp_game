// Emscripten platform layer — replaces win32_platform.cpp
// Compile with:
//   emcc My_first_cpp_game/emscripten_platform.cpp -s USE_SDL=2 -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -O2 -o docs/index.html

#include "utils.cpp"
#include <SDL2/SDL.h>
#include <emscripten.h>
#include <cstdlib>

// ---- Render state (mirrors win32_platform.cpp, minus BITMAPINFO) ----
struct Render_State {
    int height, width;
    void* memory;
};

static Render_State render_state;

#include "platform_common.cpp"
#include "renderer.cpp"
#include "game.cpp"

// ---- Globals ----
static SDL_Window*   window       = nullptr;
static SDL_Renderer* sdl_renderer = nullptr;
static SDL_Texture*  texture      = nullptr;
static Input         input        = {};
static float         delta_time   = 0.016666f;
static Uint64        frame_begin_time;
static Uint64        performance_frequency;

// ---- Input mapping (mirrors the Win32 process_button macro) ----
#define process_button(b, sdl_key)                                        \
    case sdl_key: {                                                        \
        bool changed = is_down != input.buttons[b].is_down;               \
        input.buttons[b].has_changed = changed;                           \
        input.buttons[b].is_down     = is_down;                           \
        break;                                                             \
    }

// ---- Main loop (called by Emscripten each frame) ----
static void main_loop() {
    // Reset has_changed before processing new events this frame
    for (int i = 0; i < BUTTON_COUNT; i++) {
        input.buttons[i].has_changed = false;
    }

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                emscripten_cancel_main_loop();
                return;

            case SDL_KEYDOWN:
            case SDL_KEYUP: {
                bool is_down = (event.type == SDL_KEYDOWN);
                switch (event.key.keysym.sym) {
                    process_button(BUTTON_UP,    SDLK_UP);
                    process_button(BUTTON_DOWN,  SDLK_DOWN);
                    process_button(BUTTON_LEFT,  SDLK_LEFT);
                    process_button(BUTTON_RIGHT, SDLK_RIGHT);
                    process_button(BUTTON_SHIFT, SDLK_LSHIFT);
                    process_button(BUTTON_CTRL,  SDLK_LCTRL);
                    process_button(BUTTON_W,     SDLK_w);
                    process_button(BUTTON_S,     SDLK_s);
                    default: break;
                }
                break;
            }
        }
    }

    simulate_game(&input, delta_time);

    // Blit pixel buffer to SDL texture then to screen
    SDL_UpdateTexture(texture, nullptr, render_state.memory, render_state.width * sizeof(uint32));
    SDL_RenderClear(sdl_renderer);
    SDL_RenderCopy(sdl_renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(sdl_renderer);

    // Update delta time
    Uint64 frame_end_time = SDL_GetPerformanceCounter();
    delta_time = (float)(frame_end_time - frame_begin_time) / (float)performance_frequency;
    if (delta_time > 0.05f) delta_time = 0.05f; // cap at 20fps minimum to avoid tunneling
    frame_begin_time = frame_end_time;
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);

    const int width  = 1280;
    const int height = 720;

    window = SDL_CreateWindow(
        "My First Game!",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_SHOWN
    );
    sdl_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    texture      = SDL_CreateTexture(
        sdl_renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        width, height
    );

    render_state.width  = width;
    render_state.height = height;
    render_state.memory = malloc(width * height * sizeof(uint32));

    performance_frequency = SDL_GetPerformanceFrequency();
    frame_begin_time      = SDL_GetPerformanceCounter();

    // Use 0 fps = browser's requestAnimationFrame; 1 = simulate_infinite_loop
    emscripten_set_main_loop(main_loop, 0, 1);

    free(render_state.memory);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(sdl_renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
