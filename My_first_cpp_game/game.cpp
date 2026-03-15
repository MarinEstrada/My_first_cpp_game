#define is_down(b) input->buttons[b].is_down
#define pressed(b) (input->buttons[b].is_down && input->buttons[b].has_changed)
#define released(b) (!input->buttons[b].is_down && input->buttons[b].has_changed)

//float player_pos = 0.f;
float x_player_pos = 0.f;
float y_player_pos = 0.f;
float paddle1_pos = 0.f;
float paddle2_pos = 0.f;

// change will be mainatined accross frames, v = derivative of position
float paddle1_speed, paddle2_speed;
//float movement_speed = 50.f; // units per second... need to make it units per frame (u/f = (u/s)*(s/f))

//static void get_paddle_pos(Input* input, float delta_time, float* paddle_pos,float* paddle_speed) {
//		//is added to velocity the longer it is held, is derivative of velocit == derivative of deriviative of position
//		float movement_accel = 0.f;
//		if (is_down(BUTTON_UP)) movement_accel += 2000;
//		if (is_down(BUTTON_DOWN)) movement_accel -= 2000;
//
//		// some natural friction
//		movement_accel -= *paddle_speed * 10.f;
//
//		//use equation of movement to find new position using derivative of position & derivative of derivative of position
//		*paddle_pos = *paddle_pos + *paddle_speed * delta_time + movement_accel * delta_time * delta_time * .5f;
//		*paddle_speed = *paddle_speed + movement_accel * delta_time;
//
//}

static void simulate_game(Input* input, float delta_time) {
		//render_background();
		clear_screen(0x00009f);
		draw_rect(0, 0, 85, 45, 0xffaa33); //Arena

		//// if shift button, increase speed, if ctrl, decrease speed.
		//if (is_down(BUTTON_SHIFT)) movement_speed *= 1.5f;
		//if (is_down(BUTTON_CTRL)) movement_speed *= 0.5f;

		// -----
		// GET BUTTON INPUTS
		//is added to velocity the longer it is held, is derivative of velocit == derivative of deriviative of position
		float paddle1_accel = 0.f;
		if (is_down(BUTTON_W)) paddle1_accel += 2000;
		if (is_down(BUTTON_S)) paddle1_accel -= 2000;

		float paddle2_accel = 0.f;
		if (is_down(BUTTON_UP)) paddle2_accel += 2000;
		if (is_down(BUTTON_DOWN)) paddle2_accel -= 2000;
		// -----
		// GET POSITION 

		// some natural friction to paddles
		paddle1_accel -= paddle1_speed * 10.f;
		paddle2_accel -= paddle2_speed * 10.f;

		//use equation of movement to find new position using derivative of position & derivative of derivative of position
		paddle1_pos = paddle1_pos + paddle1_speed * delta_time + paddle1_accel * delta_time * delta_time * .5f;
		paddle1_speed = paddle1_speed + paddle1_accel * delta_time;

		paddle2_pos = paddle2_pos + paddle2_speed * delta_time + paddle2_accel * delta_time * delta_time * .5f;
		paddle2_speed = paddle2_speed + paddle2_accel * delta_time;
		// -----


		draw_rect(y_player_pos, x_player_pos, 1, 1, 0xa000a0); // player
		draw_rect(-80, paddle1_pos, 2.5, 12, 0xff0000); // paddle 1
		draw_rect(80, paddle2_pos, 2.5, 12, 0x0000ff); // paddle 2

 }