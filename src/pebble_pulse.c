#include <pebble.h>

//static int inc = 0;
static Window *window;
static TextLayer *text_layer;

// Key values for AppMessage Dictionary
enum {
    STATUS_KEY = 0, 
    MESSAGE_KEY = 1
};

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Message Sent!");
  send_message(BUTTON_ID_SELECT);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Up");
  send_message(BUTTON_ID_UP);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Down");
  send_message(BUTTON_ID_DOWN);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

// Write message to buffer & send
void send_message(ButtonId button_pressed){
    DictionaryIterator *iter;
    
    app_message_outbox_begin(&iter);
    dict_write_uint8(iter, STATUS_KEY, 0x1);
    dict_write_cstring(iter, MESSAGE_KEY, "Hi Phone, I'm a Pebble!");
    switch(button_pressed){
        case BUTTON_ID_UP: dict_write_cstring(iter, MESSAGE_KEY, "."); break;
        case BUTTON_ID_DOWN: dict_write_cstring(iter, MESSAGE_KEY, "-"); break;
        case BUTTON_ID_SELECT: return;
        default: dict_write_cstring(iter, MESSAGE_KEY, "\nInvalid Button\n"); break;

    dict_write_end(iter);
    app_message_outbox_send();
}

// Called when a message is received from PebbleKitJS
static void in_received_handler(DictionaryIterator *received, void *context) {
    Tuple *tuple;
    
    tuple = dict_find(received, STATUS_KEY);
    if(tuple) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Received Status: %d", (int)tuple->value->uint32); 
    }
    
    tuple = dict_find(received, MESSAGE_KEY);
    if(tuple) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Received Message: %s", tuple->value->cstring);
    }}

// Called when an incoming message from PebbleKitJS is dropped
static void in_dropped_handler(AppMessageResult reason, void *context) {    
}

// Called when PebbleKitJS does not acknowledge receipt of a message
static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(text_layer, "Press a button");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

void init(void) {
    window = window_create();
    window_set_click_config_provider(window, click_config_provider);
    window_set_window_handlers(window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
    // const bool animated = true;
    window_stack_push(window, true);
    
    // Register AppMessage handlers
    app_message_register_inbox_received(in_received_handler); 
    app_message_register_inbox_dropped(in_dropped_handler); 
    app_message_register_outbox_failed(out_failed_handler);
        
    app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
    
    send_message();
}

void deinit(void) {
    app_message_deregister_callbacks();
    window_destroy(window);
}

int main( void ) {
    init();
    app_event_loop();
    deinit();
}