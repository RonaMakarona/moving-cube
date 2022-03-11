

static void render_background() {

	unsigned int* pixel = (unsigned int*)render_state.memory;
	for (int y = 0; y < render_state.height; y++) {
		for (int x = 0; x < render_state.width; x++) {
			*pixel++ = 0xff0000;
		}
	}
}

static void clear_screen(unsigned int color) {
	unsigned int* pixel = (unsigned int*)render_state.memory;
	for (int y = 0; y < render_state.height; y++) {
		for (int x = 0; x < render_state.width; x++) {
			*pixel++ = color;
		}
	}
}


static void draw_rect_in_pixels(int x0, int y0, int x1, int y1, unsigned int color) {

	x0 = clamp(0, x0, render_state.width);
	x1 = clamp(0, x1, render_state.width);
	y0 = clamp(0, y0, render_state.height);
	y1 = clamp(0, y1, render_state.height);

	for (int y = y0; y < y1; y++) {
		unsigned int* pixel = (unsigned int*)render_state.memory + x0 +y*render_state.width;
		for (int x = x0; x < x1; x++) {
			*pixel++ = color;
		}
	}
}

static float render_scale = 0.01f;

static void draw_rect(float x, float y, float half_size_x, float half_size_y, unsigned int color) {

	x *= render_state.height * render_scale; //x in scale of the whole height
	y *= render_state.height * render_scale; //y in scale of the whole height
	half_size_x *= render_state.height * render_scale; // same ^
	half_size_y *= render_state.height * render_scale; // same ^

	x += render_state.width / 2.f;
	y += render_state.height / 2.f;

	//Change to pixels
	int x0 = x - half_size_x; 
	int x1 = x + half_size_x;
	int y0 = y - half_size_y;
	int y1 = y + half_size_y;

	//Points are calculated by their x and y, where the x and y are the middle coords
	//Using that logic, we need x0 and y0 to be x-(half of x) and y-(half of y)
	//That way, x and y will resemble the middle coords.

	draw_rect_in_pixels(x0, y0, x1, y1, color);
}
