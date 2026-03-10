#define is_down(b) input->buttons[b].is_down
#define pressed(b) (input->buttons[b].is_down && input->buttons[b].has_changed)
#define released(b) (!input->buttons[b].is_down && input->buttons[b].has_changed)

//float player_pos = 0.f;
float x_player_pos = 0.f;
float y_player_pos = 0.f;
float movement_speed = 0.1f; //currently this number is units per second... need to make it units per frame

static void simulate_game(Input* input) {
		//render_background();
		clear_screen(0x00009f);
		if (pressed(BUTTON_UP)) x_player_pos += movement_speed;
		if (pressed(BUTTON_DOWN)) x_player_pos -= movement_speed;
		if (pressed(BUTTON_RIGHT)) y_player_pos += movement_speed;
		if (pressed(BUTTON_LEFT)) y_player_pos -= movement_speed;
		draw_rect(y_player_pos, x_player_pos, 1, 1, 0xff5522);
		//if (input->buttons[BUTTON_RIGHT].is_down)
		//	draw_rect(50, 20, 25, 3, 0x0088fe);
		//if (input->buttons[BUTTON_LEFT].is_down)
		//	draw_rect(-45, -15, 7, 20, 0x335544);
 }