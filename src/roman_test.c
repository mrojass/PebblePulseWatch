#include <pebble.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
  
#define ACCEL_STEP_MS 1000
  
Window *window;
TextLayer *text_layer;
void writeTo(char *str);
static AppTimer *timer;

int countx = 0;
int county = 0;
int countz = 0;

static void timer_callback(void *data) {

  /*
  AccelData accel = (AccelData) { .x = 0, .y = 0, .z = 0 };

  accel_service_peek(&accel);
  if(accel.x > 250){
    writeTo("X last movement");
  }else{
    if(accel.y > 250){
      writeTo("Y last movement");
    }else
      if(accel.z > 75){
        writeTo("Z last movement");   
      }
    
  }
  */
  
  
  AccelData dats;
  accel_service_peek(&dats);
  
  char str[15];
  snprintf(str, sizeof(str), "%d", dats.x);
  //char strn[16];
  //snprintf(strn, sizeof(strn), "%s\0", str);
  //text_layer_set_text(text_layer, str);
  writeTo(str);
  
  
  
  
  timer = app_timer_register(ACCEL_STEP_MS, timer_callback, NULL);
}

void handle_init(void) {
	// Create a window and text layer
	window = window_create();
	text_layer = text_layer_create(GRect(0, 0, 144, 154));
	
	writeTo("hello");
  accel_data_service_subscribe(0, NULL);

  timer = app_timer_register(ACCEL_STEP_MS, timer_callback, NULL);
}

void handle_deinit(void) {
	// Destroy the text layer
	text_layer_destroy(text_layer);
	
	// Destroy the window
	window_destroy(window);
  accel_data_service_unsubscribe();
}

void writeTo(char *str){
  
  
  // Set the text, font, and text alignment
	text_layer_set_text(text_layer, str);
	text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
	
	// Add the text layer to the window
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_layer));

	// Push the window
	window_stack_push(window, true);
	
	// App Logging!
	APP_LOG(APP_LOG_LEVEL_DEBUG, str);
}


int main(void) {
	handle_init();
	app_event_loop();
	handle_deinit();
}