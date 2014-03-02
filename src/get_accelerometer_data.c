#include <pebble.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
  
#define ACCEL_STEP_MS 1000
  
Window *window;
TextLayer *text_layer;
void writeTo(char *str);
static AppTimer *timer;

//This function should write and also log the acceleration on the x axis every ACCEL_STEP_MS.
//It is called by init.
//It logs the datum, but refuses to write to the pebble screen.  we don't know why.
/*
static void timer_callback(void *data) {
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
*/

//This function should _log_ a string representation of the acceleration data in 3-space
//It should run every second, and collect data at a rate of 10 samples/second due to accel_data_service_subscribe()
//It worked at one point, it doesn't now.  we don't know why.
void accel_data_handler(AccelData *data, uint32_t num_samples) {
  char x[15];
  char y[15];
  char z[15];
  char str[51];
  for(int i = 0; i < 10; i++){
    snprintf(x, sizeof(x), "%d", data[i].x);
    snprintf(y, sizeof(y), "%d", data[i].y);
    snprintf(z, sizeof(z), "%d", data[i].z);
    snprintf(str, sizeof(str), "%s, %s, %s", x, y, z);
    APP_LOG(APP_LOG_LEVEL_DEBUG, str);
  }
}

//This function should write the argument str to the screen.
//It worked for some strings, but not the ones we needed.  we don't know why.
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

void handle_init(void) {
	// Create a window and text layer
	window = window_create();
	text_layer = text_layer_create(GRect(0, 0, 144, 154));
	
	writeTo("hello");
  
//  call timer_callback()
//  accel_data_service_subscribe(0, NULL);
//  timer = app_timer_register(ACCEL_STEP_MS, timer_callback, NULL);
  
//  call accel_data_handler()  
  accel_data_service_subscribe(10, &accel_data_handler);
  accel_service_set_sampling_rate(ACCEL_SAMPLING_10HZ);
}

void handle_deinit(void) {
	// Destroy the text layer
	text_layer_destroy(text_layer);
	
	// Destroy the window
	window_destroy(window);
  accel_data_service_unsubscribe();
}

int main(void) {
	handle_init();
	app_event_loop();
	handle_deinit();
}