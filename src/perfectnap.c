#include "pebble.h"
#include "timer.h"

#define NUM_MENU_SECTIONS 1
#define NUM_FIRST_MENU_ITEMS 4

static Window *window;

// This is a menu layer
// You have more control than with a simple menu layer
static MenuLayer *menu_layer;


// A callback is used to specify the amount of sections of menu items
// With this, you can dynamically add and remove sections
static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return NUM_MENU_SECTIONS;
}

// Each section has a number of items;  we use a callback to specify this
// You can also dynamically add and remove items using this
static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  switch (section_index) {
    case 0:
      return NUM_FIRST_MENU_ITEMS;
    default:
      return 0;
  }
}

// A callback is used to specify the height of the section header
static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  // This is a define provided in pebble.h that you may use for the default height
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

// Here we draw what each header is
static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
    menu_cell_basic_header_draw(ctx, cell_layer, "Long of your nap");
}

// This is the menu item draw callback where you specify what each item should look like
static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
      // Use the row to specify which item we'll draw
      switch (cell_index->row) {
        case 0:
          // This is a basic menu item with a title and subtitle
          menu_cell_basic_draw(ctx, cell_layer, "15 minutes", "Boost your alertness", NULL);
          break;

        case 1:
          // This is a basic menu item with a title and subtitle
          menu_cell_basic_draw(ctx, cell_layer, "30 minutes", "Restore wakefulness", NULL);
          break;

        case 2:
          // This is a basic menu item with a title and subtitle
          menu_cell_basic_draw(ctx, cell_layer, "60 minutes", "Improve your memory", NULL);
          break;
        case 3:
          // This is a basic menu item with a title and subtitle
          menu_cell_basic_draw(ctx, cell_layer, "90 minutes", "Refresh your creativity", NULL);
          break;
      }
}

// Here we capture when a user selects a menu item
void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  // Use the row to specify which item will receive the select action
  switch (cell_index->row) {
    // This is the menu item with the cycling icon
    case 0:
      nap_time_selection = 0;
      timer_init();
      
      break;
    case 1:
      nap_time_selection = 1;
      timer_init();
      
      break;
    case 2:
      nap_time_selection = 2;
      timer_init();
      
      break;
    case 3:
      nap_time_selection = 3;
      timer_init();
      
      break;
  }

}

// This initializes the menu upon window load
void window_load(Window *window) {

  // We prepare to initialize the menu layer
  // We need the bounds to specify the menu layer's viewport size
  // In this case, it'll be the same as the window's

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  // Create the menu layer
  menu_layer = menu_layer_create(bounds);

  // Set all the callbacks for the menu layer
  menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks){
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_callback,
  });

  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(menu_layer, window);

  // Add it to the window for display
  layer_add_child(window_layer, menu_layer_get_layer(menu_layer));
}

void window_unload(Window *window) {
  // Destroy the menu layer

  menu_layer_destroy(menu_layer);
  
}

static void init(void) {

}

static void deinit(void) {
  timer_deinit();
}

int main(void) {
  window = window_create();

  // Setup the window handlers
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  window_stack_push(window, true /* Animated */);
  init();
  app_event_loop();
  deinit();
  window_destroy(window);
}
