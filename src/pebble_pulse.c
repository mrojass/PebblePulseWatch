#include <pebble.h>

static int inc = 0;
static Window *window;
static TextLayer *text_layer;

 void out_sent_handler(DictionaryIterator *sent, void *context) {
   // outgoing message was delivered
   //text_layer_set_text(text_layer, "Sent message was received by the phone!");
 }


 void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
   // outgoing message failed
 }


 void in_received_handler(DictionaryIterator *received, void *context) {
   // incoming message received
 }


 void in_dropped_handler(AppMessageResult reason, void *context) {
   // incoming message dropped
 }

// Read message with button input and send to phone
/*static void send_morse_code()
{
    //app_message_out_send();
}*/

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  //send_morse_code();
  text_layer_set_text(text_layer, "Message Sent!");
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Up");
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Down");
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
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
static void get_message()
{
  DictionaryIterator *iter;
  if (app_message_outbox_begin(&iter) != APP_MSG_OK) {
    return;
  }
  //Tuplet value = TupletInteger(0, 1);
  dict_write_cstring(iter, 0, "hello"); 
  // app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  /*while(click_recognizer_get_button_id(select_click_handler) != BUTTON_ID_SELECT)
  {
    if(click_recognizer_get_button_id(select_click_handler) == BUTTON_ID_UP) {
      Tuplet value = TupletInteger(inc, 0);
      dict_write_tuplet(iter, &value); 
      inc++;
    } else if (click_recognizer_get_button_id(select_click_handler) == BUTTON_ID_DOWN) {
      Tuplet value = TupletInteger(inc, 1);
      dict_write_tuplet(iter, &value);
      inc++; 
    } else {
      text_layer_set_text(text_layer, "Invalid Button!");
    }
  }*/
  app_message_outbox_send();
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  app_message_register_outbox_sent(out_sent_handler);
  app_message_register_outbox_failed(out_failed_handler);


  const uint32_t inbound_size = 64;
  const uint32_t outbound_size = 64;
  app_message_open(inbound_size, outbound_size);
  get_message();
  
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();

  deinit();
}
