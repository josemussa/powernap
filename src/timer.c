#include "pebble.h"
#include "timer.h"



static Window *window_timer;
static TextLayer *text_layer;
void timer_callback(void *data);
static int init_counter = 0;
static int nap_counter = 0;
static int nap_time;

static int last_x = 0;
static int last_y = 0;
static int last_z = 0;

static bool nap_counter_ready = false;
static bool init_counter_ready = true;

static BitmapLayer *image_layer;

static GBitmap *ready;
static GBitmap *calm;
static GBitmap *napping;
static GBitmap *wakeup;

void handle_tick(struct tm *tick_time, TimeUnits units_changed);

static void generate_image(Window *window, int image){

	if(ready != NULL){
		gbitmap_destroy(ready);
	}

	if(calm != NULL){
		gbitmap_destroy(calm);
	}

	if(napping != NULL){
		gbitmap_destroy(napping);
	}

	if(wakeup != NULL){
		gbitmap_destroy(wakeup);
	}

	if(image_layer != NULL){
		bitmap_layer_destroy(image_layer);
	}

	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_frame(window_layer);
	image_layer = bitmap_layer_create(bounds);

	switch(image){
		case 0:
			ready = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_1);
			bitmap_layer_set_bitmap(image_layer, ready);
			break;
		case 1:
			calm = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_2);
			bitmap_layer_set_bitmap(image_layer, calm);
			break;
		case 2:
			napping = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_3);
			bitmap_layer_set_bitmap(image_layer, napping);
			break;
		case 3:
			wakeup = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_4);
			bitmap_layer_set_bitmap(image_layer, wakeup);
			break;
	}

	
    bitmap_layer_set_alignment(image_layer, GAlignCenter);
    layer_add_child(window_layer, bitmap_layer_get_layer(image_layer));
}

static void reset_init_counter(){
	  init_counter = 0;
	  generate_image(window_timer, 1);
}

static void handle_accel(AccelData *data, uint32_t num_samples) {
	if(nap_counter_ready == false){

	  for(int i = 0; i < 10; i++){
	    int current_x = data[i].x;
	    int result_x = abs(last_x) - abs(current_x);
	    result_x = abs(result_x);
	    if(result_x > 700){
	    	reset_init_counter();
	    }
	    last_x = data[i].x;

	    int current_y = data[i].y;
	    int result_y = abs(last_y) - abs(current_y);
	    result_y = abs(result_y);
	    if(result_y > 700){
	    	reset_init_counter();
	    }
	    last_y = data[i].y;

	    int current_z = data[i].z;
	    int result_z = abs(last_z) - abs(current_z);
	    result_z = abs(result_z);
	    if(result_z > 700){
	    	reset_init_counter();
	    }
	    last_z = data[i].z;
	  }
	}
}

void handle_tick(struct tm *tick_time, TimeUnits units_changed)
{
	switch(nap_time_selection){
		case 0:
			nap_time = 900;
			break;
		case 1:
			nap_time = 1800;
			break;
		case 2:
			nap_time = 3600;
			break;
		case 3:
			nap_time = 5400;
			break;
	}
	

	if(init_counter_ready == true){
    	init_counter += 1;
    	generate_image(window_timer, 0);
    }
    
    
    if(init_counter > 100){

    	nap_counter_ready = true;
    	init_counter_ready = false;

    }

    if(nap_counter_ready == true){
    	nap_counter += 1;
    	generate_image(window_timer, 2);
    	if(nap_counter > nap_time){
    		vibes_long_pulse();
    		generate_image(window_timer, 3);
    	}
    }

}

static void window_load(Window *window) {
	tick_timer_service_subscribe(SECOND_UNIT, handle_tick);
}

static void window_unload(Window *window) {

	if(ready){
		gbitmap_destroy(ready);
	}

	if(calm){
		gbitmap_destroy(calm);
	}

	if(napping){
		gbitmap_destroy(napping);
	}

	if(wakeup){
		gbitmap_destroy(wakeup);
	}

	if(image_layer){
		bitmap_layer_destroy(image_layer);
	}
	
	accel_data_service_unsubscribe();
	text_layer_destroy(text_layer);
	window_destroy(window_timer);
	tick_timer_service_unsubscribe();
    init_counter = 0;
    nap_counter = 0;
	last_x = 0;
	last_y = 0;
	last_z = 0;
	nap_counter_ready = false;
	init_counter_ready = true;
}

void timer_deinit(void) {

}

void timer_init(void) {
	window_timer = window_create();
	window_set_background_color(window_timer, GColorBlack);
	text_layer = text_layer_create(GRect(0, 0, 144, 154));
	nap_counter_ready = false;
	init_counter_ready = true;
  // Setup the window handlers
  window_set_window_handlers(window_timer, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(window_timer, true);
  accel_data_service_subscribe(20, handle_accel);
  accel_service_set_sampling_rate(ACCEL_SAMPLING_10HZ);
}