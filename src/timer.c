#include "pebble.h"
#include "timer.h"

static Window *window_timer;
static TextLayer *text_layer;
void writeTo(char *str);

static void handle_accel(AccelData *data, uint32_t num_samples) {
  char x[15];
  char y[15];
  char z[15];
  char str[51];
  for(int i = 0; i < 10; i++){
    snprintf(x, sizeof(x), "%d", data[i].x);
    snprintf(y, sizeof(y), "%d", data[i].y);
    snprintf(z, sizeof(z), "%d", data[i].z);
    snprintf(str, sizeof(str), "%s, %s, %s", x, y, z);
    //APP_LOG(APP_LOG_LEVEL_DEBUG, str);
    writeTo(str);
  }
}

void writeTo(char *str){

	text_layer_set_text(text_layer, str);

	layer_add_child(window_get_root_layer(window_timer), text_layer_get_layer(text_layer));
	

	// App Logging!
	APP_LOG(APP_LOG_LEVEL_DEBUG, str);

}

static void window_load(Window *window) {
	vibes_double_pulse();
}

static void window_unload(Window *window) {
	accel_data_service_unsubscribe();
	text_layer_destroy(text_layer);
}

void timer_deinit(void) {
	if(window_timer){
		window_destroy(window_timer);
	}

}

void timer_init(void) {
	window_timer = window_create();
	text_layer = text_layer_create(GRect(0, 0, 144, 154));
 	
	writeTo("hello");
  // Setup the window handlers
  window_set_window_handlers(window_timer, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(window_timer, true);
  accel_data_service_subscribe(10, handle_accel);
  accel_service_set_sampling_rate(ACCEL_SAMPLING_10HZ);
}