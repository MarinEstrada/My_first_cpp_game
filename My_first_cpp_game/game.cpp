#define is_down(b) input->buttons[b].is_down
#define pressed(b) (input->buttons[b].is_down && input->buttons[b].has_changed)
#define released(b) (!input->buttons[b].is_down && input->buttons[b].has_changed)

//float player_pos = 0.f;
float ball_x_pos = 0.f;
float ball_y_pos = 0.f;
float paddle1_pos = 0.f;
float paddle2_pos = 0.f;

float paddles_x_pos = 80.f;

// change will be mainatined accross frames, v = derivative of position
float initial_speed = 50.0f;
float paddle1_speed, paddle2_speed, ball_x_speed = initial_speed, ball_y_speed;
//float movement_speed = 50.f; // units per second... need to make it units per frame (u/f = (u/s)*(s/f))

float ball_half_size = 1.0f;

float arena_half_size_x = 85;
float arena_half_size_y = 45;

float paddle_half_size_x = 2.5;
float paddle_half_size_y = 12;

float wall_collision_speed_modifier = 0.30f;
float paddle_contact_speed_modifier = 0.75f;
float speed_increase_modifier = 1.1;

//bool hit_roof_floor(int multiplier_val = 1) {
//	
//	if (ball_y_pos < arena_half_size_y ) return false;
//	if (ball_y_pos > -arena_half_size_y ) return false;
//
//	return true;
//}

static void initialize_ball(float x_speed_modifer = 1, float y_speed_modifer = 0) {
	ball_x_pos = 0; 
	ball_y_pos = 0;
	ball_x_speed = initial_speed * x_speed_modifer;
	ball_y_speed = initial_speed * y_speed_modifer;
}

static void initialize_game(float x_speed_modifer = 1, float y_speed_modifer = 0) {
	paddle1_pos = 0;
	paddle2_pos = 0;
	initialize_ball(x_speed_modifer, y_speed_modifer);
}

static bool in_paddle_range(int paddle_num) {

	//float *paddle_pos;
	float paddle_pos, x_pos;
	switch (paddle_num) {
		case 1:
			//paddle_pos = &paddle1_pos;
			paddle_pos = paddle1_pos;
			x_pos = -ball_x_pos;
			break;

		default:
			//paddle_pos = &paddle2_pos;
			paddle_pos = paddle2_pos;
			x_pos = ball_x_pos;
	}

	//first check if in correct horizontal position (x)
	if (x_pos + ball_half_size < paddles_x_pos - paddle_half_size_x) return false;

	if (x_pos - ball_half_size > paddles_x_pos + paddle_half_size_x) return false;

	//check if ball below paddle
	if (ball_y_pos + ball_half_size < paddle_pos - paddle_half_size_y) return false;

	//check if ball above paddle
	if (ball_y_pos - ball_half_size > paddle_pos + paddle_half_size_y) return false;

	//if previous checks return false, ball must be making contact with paddle
	return true;
}

//get paddle position
static void simulate_paddle(float *position, float* speed, float accel, float delta_time) {

	// some natural friction to paddle
	accel -= *speed * 10.f;

	//use equation of movement to find new position using derivative of position & derivative of derivative of position
	*position = *position + *speed * delta_time + accel * delta_time * delta_time * .5f;
	*speed = *speed + accel * delta_time;

	// ADD COLLISION TO PADDLES AND CEILING/FLOOR
	// if paddle hit areana boundary, stop paddle
	if (*position + paddle_half_size_y > arena_half_size_y) {
		*position = arena_half_size_y - paddle_half_size_y;
		*speed *= - wall_collision_speed_modifier;// if collision happens, invert speed for bounce and slow down
	}
	else if (*position - paddle_half_size_y < -arena_half_size_y) { // do colliison for going down as well
		*position = -arena_half_size_y + paddle_half_size_y;
		*speed *= -wall_collision_speed_modifier;// if collision happens, invert speed for bounce and slow down
	}

}

static void simulate_game(Input* input, float delta_time) {
	//render_background();
	clear_screen(0x00009f);
	draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0xffaa33); //Arena

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
	// GET POSITION OF PADDLES
	simulate_paddle(&paddle1_pos, &paddle1_speed, paddle1_accel, delta_time);
	simulate_paddle(&paddle2_pos, &paddle2_speed, paddle2_accel, delta_time);

	//----
	//SIMULATE BALL POSITION
	{
		ball_x_pos += ball_x_speed * delta_time;
		ball_y_pos += ball_y_speed * delta_time;

		//BALL COLLISION
		//check paddle 2 (right side)
		if (in_paddle_range(2)) {
			ball_x_pos = paddles_x_pos - paddle_half_size_x - ball_half_size;
			ball_x_speed *= -1 * speed_increase_modifier;
			ball_y_speed = (ball_y_pos - paddle2_pos) * 3 + (paddle2_speed * paddle_contact_speed_modifier);
		}
		//check paddle 1 (left side)
		if (in_paddle_range(1)) {
			ball_x_pos = -paddles_x_pos + paddle_half_size_x + ball_half_size;
			ball_x_speed *= -1 * speed_increase_modifier;
			ball_y_speed = (ball_y_pos - paddle1_pos) * 3 + (paddle1_speed * paddle_contact_speed_modifier);

		}

		////if (hit_roof_floor())
		if (ball_y_pos + ball_half_size > arena_half_size_y) {//check if hit roof
			ball_y_pos = arena_half_size_y - ball_half_size;
			ball_y_speed *= -1;
		}
		if (ball_y_pos - ball_half_size < -arena_half_size_y) {//check if hit floor
			ball_y_pos = -arena_half_size_y + ball_half_size;
			ball_y_speed *= -1;
		}

		//if ball passes paddles reset game 
		if (ball_x_pos + ball_half_size > arena_half_size_x) {//check if passed 2nd paddle
			initialize_game(-1);
		}
		if (ball_x_pos - ball_half_size < -arena_half_size_x) {//check if passed 1st paddle
			initialize_game();
		}
	}

	//---
	// RENDER OBJECTS ON SCREEN

	draw_rect(ball_x_pos, ball_y_pos, ball_half_size, ball_half_size, 0xa000a0); // player
	//draw_rect(-80, paddle1_pos, paddle_half_size_x, paddle_half_size_y, 0xff0000); // paddle 1
	//draw_rect(80, paddle2_pos, paddle_half_size_x, paddle_half_size_y, 0x0000ff); // paddle 2
	draw_rect(-paddles_x_pos, paddle1_pos, paddle_half_size_x, paddle_half_size_y, 0xff0000); // paddle 1
	draw_rect(paddles_x_pos, paddle2_pos, paddle_half_size_x, paddle_half_size_y, 0x0000ff); // paddle 2

 }