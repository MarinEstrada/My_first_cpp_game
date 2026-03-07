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


void clear_screen(unsigned int color) {
	unsigned int* pixel = (unsigned int*)render_state.memory;
	for (int y = 0; y < render_state.height; y++) {
		for (int x = 0; x < render_state.width; x++) {
			*pixel++ = color;
		}
	}
}

void draw_rect(int x0, int y0, int x1, int y1, unsigned int color) {
	for (int y = y0; y < y1; y++) {
		// do y*width bc we want to up by the size of a row, remember mem basciallly an array
		unsigned int* pixel = (unsigned int*)render_state.memory + x0 + y*render_state.width;
		for (int x = x0; x < x1; x++) {
			*pixel++ = color;
		}
	}
}