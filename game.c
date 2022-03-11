#define is_down(b) input->buttons[b].is_down
#define pressed(b) (input->buttons[b].is_down && input->buttons[b].changed)
#define released(b)  (!input->buttons[b].is_down && input->buttons[b].changed)

float player_pos_x = 0.f;
float player_pos_y = 0.f;

static void
simulate_game(struct Input* input, float dt) {
	clear_screen(0xffffff);
	float speed = 5.f; //units per second

	// speed = units/seconds
	// dt = seconds/frame
	//speed * dt = u/s * s/f -> u/f -> makes the movement relative

	if (pressed(BUTTON_UP)) {player_pos_y += speed;}
	if (pressed(BUTTON_DOWN)) {player_pos_y -= speed;}
	if (pressed(BUTTON_RIGHT)) {player_pos_x += speed;}
	if (pressed(BUTTON_LEFT)) {player_pos_x -= speed;}

	draw_rect(player_pos_x, player_pos_y, 5, 5, 0x0000ff);

}
