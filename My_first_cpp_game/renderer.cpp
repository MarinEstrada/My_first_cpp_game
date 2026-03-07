//void render_background() {
//	uint32* pixel = (uint32*)render_state.memory; //ptr to current pixel
//	for (int y = 0; y < render_state.height; y++) {
//		for (int x = 0; x < render_state.width; x++) {
//			//*pixel++ = 0xff5500; //this line assigned *pixel to 0xff5500, then afterwards it increments the ptr pixel by one
//			//*pixel++ = 0xff5500 * x + 0xff5500 * y;
//			//*pixel++ = (x + y) * 2;
//			*pixel++ = 0xff00ff * x + 0x00ff00 * y;
//		}
//	}
//}

static float render_scale = 0.01f;

static void clear_screen(uint32 color) {
	uint32* pixel = (uint32*)render_state.memory;
	for (int y = 0; y < render_state.height; y++) {
		for (int x = 0; x < render_state.width; x++) {
			*pixel++ = color;
		}
	}
}

static void draw_rect_in_pixels(int x0, int y0, int x1, int y1, uint32 color) {

	x0 = clamp(0, x0, render_state.width);
	x1 = clamp(0, x1, render_state.width);
	y0 = clamp(0, y0, render_state.height);
	y1 = clamp(0, y1, render_state.height);

	for (int y = y0; y < y1; y++) {
		// do y*width bc we want to up by the size of a row, remember mem basciallly an array
		uint32* pixel = (uint32*)render_state.memory + x0 + y*render_state.width;
		for (int x = x0; x < x1; x++) {
			*pixel++ = color;
		}
	}
}

//want to make render pixel independent
static void draw_rect(float x, float y, float half_size_x, float half_size_y, uint32 color) {

	// multiply the 4 var by EITHER screen width or height to make box relative to the height or width of the screen! (with 1 = 1 width or height)
	// But... we can also use aspect ratio width:height! check if taller or wider than 16:9 (standard widescreen)
	// if taller, use width to make items size relative to width
	// if wider, use height to make items size relative to height

	float standard_widescreen = 16.0 / 9.0;
	float current_ratio = (float)render_state.width / (float)render_state.height;

	int relative_to;

	if (current_ratio > standard_widescreen) relative_to = render_state.height;
	else relative_to = render_state.width;


	x *= relative_to * render_scale;
	y *= relative_to * render_scale;
	half_size_x *= relative_to * render_scale;
	half_size_y *= relative_to * render_scale;

	x += render_state.width / 2.f; // center x using width
	y += render_state.height / 2.f; // center y using height

	int x0 = x - half_size_x;
	int x1 = x + half_size_x;
	int y0 = y - half_size_y;
	int y1 = y + half_size_y;

	draw_rect_in_pixels(x0, y0, x1, y1, color);
}
