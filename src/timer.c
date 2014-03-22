#include "pebble.h"
#include "timer.h"



static Window *window_timer;
static TextLayer *text_layer;
static void timer_callback(void *data);
static int init_counter = 0;
static int nap_counter = 0;

int nap_time_selection;
static int nap_time;

static int last_x = 0;
static int last_y = 0;
static int last_z = 0;

static bool nap_counter_ready = false;
static bool init_counter_ready = true;

void handle_tick(struct tm *tick_time, TimeUnits units_changed);

static void reset_init_counter(){
	  init_counter = 0;
}

static void handle_accel(AccelData *data, uint32_t num_samples) {
	if(nap_counter_ready == false){

	  for(int i = 0; i < 10; i++){
	    int current_x = data[i].x;
	    int result_x = abs(last_x) - abs(current_x);
	    result_x = abs(result_x);
	    //APP_LOG(APP_LOG_LEVEL_DEBUG, "result X: %d", result_x);
	    if(result_x > 700){
	    	//APP_LOG(APP_LOG_LEVEL_DEBUG, "miauuuuuuuuuuuuu");
	    	//vibes_double_pulse();
	    	reset_init_counter();
	    }
	    last_x = data[i].x;

	    int current_y = data[i].y;
	    int result_y = abs(last_y) - abs(current_y);
	    result_y = abs(result_y);
	    //APP_LOG(APP_LOG_LEVEL_DEBUG, "result Y: %d", result_y);
	    if(result_y > 700){
	    	//APP_LOG(APP_LOG_LEVEL_DEBUG, "miauuuuuuuuuuuuu");
	    	//vibes_double_pulse();
	    	reset_init_counter();
	    }
	    last_y = data[i].y;

	    int current_z = data[i].z;
	    int result_z = abs(last_z) - abs(current_z);
	    result_z = abs(result_z);
	    //APP_LOG(APP_LOG_LEVEL_DEBUG, "result Z: %d", result_z);
	    if(result_z > 700){
	    	//APP_LOG(APP_LOG_LEVEL_DEBUG, "miauuuuuuuuuuuuu");
	    	//vibes_double_pulse();
	    	reset_init_counter();
	    }
	    last_z = data[i].z;
	  }
	}
}

void writeTo(char *str){
	// App Logging!
	APP_LOG(APP_LOG_LEVEL_DEBUG, str);
}

void handle_tick(struct tm *tick_time, TimeUnits units_changed)
{
	switch(nap_time_selection){
		case 0:
			//nap_time = 900;
			nap_time = 5;
			break;
		case 1:
			//nap_time = 1800;
			nap_time = 10;
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
    	APP_LOG(APP_LOG_LEVEL_DEBUG, "cuenta: %d", init_counter);
    }
    
    
    if(init_counter > 10){
    	nap_counter_ready = true;
    	init_counter_ready = false;
    }

    if(nap_counter_ready == true){
    	nap_counter += 1;
    	APP_LOG(APP_LOG_LEVEL_DEBUG, "nap: %d", nap_counter);
    	if(nap_counter > nap_time){
    		vibes_long_pulse();
    	}
    }

}

static void window_load(Window *window) {
	tick_timer_service_subscribe(SECOND_UNIT, handle_tick);
}

static void window_unload(Window *window) {
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
	text_layer = text_layer_create(GRect(0, 0, 144, 154));
 	
	writeTo("hello");
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