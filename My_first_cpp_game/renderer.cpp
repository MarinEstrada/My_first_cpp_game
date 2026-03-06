void render_background() {
	unsigned int* pixel = (unsigned int*)render_state.memory; //ptr to current pixel
	for (int y = 0; y < render_state.height; y++) {
		for (int x = 0; x < render_state.width; x++) {
			//*pixel++ = 0xff5500; //this line assigned *pixel to 0xff5500, then afterwards it increments the ptr pixel by one
			//*pixel++ = 0xff5500 * x + 0xff5500 * y;
			//*pixel++ = (x + y) * 2;
			*pixel++ = 0xff00ff * x + 0x00ff00 * y;
		}
	}
}

void clear_screen(unsigned int color) {
	unsigned int* pixel = (unsigned int*)render_state.memory;
	for (int y = 0; y < render_state.height; y++) {
		for (int x = 0; x < render_state.width; x++) {
			*pixel++ = color;
		}
	}
}

void draw_rect(int x0, int y0, int x1, int y1, unsigned int color) {

}