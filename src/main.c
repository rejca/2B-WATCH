#include <pebble.h>

#define KEY_FONT 0
#define KEY_MARGIN 1
#define KEY_BACKGRND 2
#define KEY_BLUETOOTH 3
#define KEY_BLUETOOTH_GRID 4
#define KEY_BATTERY 5  
#define KEY_CMINUTE 6
#define KEY_CHOUR 7
#define KEY_PMINUTE 8
#define KEY_GRID 9
  
uint8_t byte_array[10];  //variable to watch storage
int DATA_KEY = 1;  
  
static Window *s_main_window;

enum {
  FONT_LIGHT = 0,
  FONT_NORMAL = 1
};

enum {
  MARGIN_MIN = 0,
  MARGIN_LOW = 1,
  MARGIN_MIDDLE = 2,
  MARGIN_HIGH = 3,
  MARGIN_MAX = 4
};

//background
enum {
  BACKGRND_WHITE = 0,
  BACKGRND_BLACK = 1
};

//vibration bluetooth
enum {
  BLUETOOTH_OFF = 0,
  BLUETOOTH_ON = 1
};

//show bluetooth
enum {
  BLUETOOTH_GRID_OFF = 0,
  BLUETOOTH_GRID_ON = 1
};


//show battery
enum {
  BATTERY_OFF = 0,
  BATTERY_ON = 1
};

//color minute
enum {
  CMINUTE_BLACK = 0,
  CMINUTE_GRAY = 1
};

//color hour
enum {
  CHOUR_BLACK = 0,
  CHOUR_GRAY = 1
};

//place minutes
enum {
  PMINUTE_DOWN = 0,
  PMINUTE_RIGHT = 1,
  PMINUTE_DIAGONAL = 2,
  PMINUTE_RANDOM = 3,
  PMINUTE_CYCLE = 4
};

//grid show
enum {
  GRID_OFF = 0,
  GRID_SHORT = 1,
  GRID_LONG = 2,
  GRID_FULL = 3
};

static int font;
static int margin;
static int backgrnd;
static int bluetooth;
static int bluetooth_grid;
static int battery;
static int cminute;
static int chour;
static int pminute;
static int grid;

static BitmapLayer *s_background_layer_1;
static BitmapLayer *s_background_layer_2;
static BitmapLayer *s_background_layer_3;
static BitmapLayer *s_background_layer_4;

static GBitmap *s_FONT_B;//numbers
static GBitmap *s_FONT_G;
static GBitmap *s_B[10]; 
static GBitmap *s_G[10]; 

static GBitmap *s_BATTERY; //battery, bluetooth
static GBitmap *s_BAT[11]; 

static BitmapLayer *s_background_layer_vertical;
static BitmapLayer *s_background_layer_horizontal;
static BitmapLayer *s_background_layer_battery;
static BitmapLayer *s_background_layer_bluetooth;

static GBitmap *s_vertical;
static GBitmap *s_horizontal;

int s_bluetooth = 2;  //global bluetooth, 2 - always change state (nevertime)
int s_cycle = 0;
int s_random = 0;

int x_diff = 0;  //margin - maybe enum?
int y_diff = 0;

static void set_number(BitmapLayer *s_background_layer, int id, int black) {      
  if (black == 1) {
    bitmap_layer_set_bitmap(s_background_layer, s_B[id]);
  } else {
    bitmap_layer_set_bitmap(s_background_layer, s_G[id]);
  }
}

char* subString (const char* input, int offset, int len, char* dest)
{ 
  int input_len = strlen (input);
  if (offset + len > input_len)
  {
     return NULL;
  }
  strncpy (dest, input + offset, len);
  
  dest[len] = '\0';
  return dest;
}

int integer (char id[1]) {
  int c = 0;
  if (strcmp(id, "0") == 0) c = 0;
  if (strcmp(id, "1") == 0) c = 1;
  if (strcmp(id, "2") == 0) c = 2;
  if (strcmp(id, "3") == 0) c = 3;
  if (strcmp(id, "4") == 0) c = 4;
  if (strcmp(id, "5") == 0) c = 5;
  if (strcmp(id, "6") == 0) c = 6;
  if (strcmp(id, "7") == 0) c = 7;
  if (strcmp(id, "8") == 0) c = 8;
  if (strcmp(id, "9") == 0) c = 9;
  return c;
}

static void update_time() {

  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);    
  
  // Create a long-lived buffer
  static char buffer_time[] = "0000";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    //Use 2h hour format
    strftime(buffer_time, sizeof("0000"), "%H%M", tick_time);
  } else {
    //Use 12 hour format
    strftime(buffer_time, sizeof("0000"), "%I%M", tick_time);
  }
  
  char cislice[1]; //only one number
  int m1; int m2; int h1; int h2;
  
  subString(buffer_time, 0, 1, cislice); 
  h1 = integer(cislice);    
  subString(buffer_time, 1, 1, cislice); 
  h2 = integer(cislice);      
  subString(buffer_time, 2, 1, cislice);
  m1 = integer(cislice);     
  subString(buffer_time, 3, 1, cislice);
  m2 = integer(cislice);      
    
  int minute_type;
  
  if (pminute == PMINUTE_RANDOM) {
    minute_type = s_random;
  } else if (pminute == PMINUTE_CYCLE) {
    minute_type = s_cycle;    
  } else {
    minute_type = pminute;
  }
  
  int hour_black;  
  int minute_black;
  hour_black = (chour == CHOUR_BLACK) ? 1 : 0;
  minute_black = (cminute == CHOUR_BLACK) ? 1 : 0;  
  
  switch(minute_type) {    
    case PMINUTE_DOWN:                                  
      set_number(s_background_layer_1, h1, hour_black);        
      set_number(s_background_layer_2, h2, hour_black);        
      set_number(s_background_layer_3, m1, minute_black);        
      set_number(s_background_layer_4, m2, minute_black);                
      break;
    
    case PMINUTE_RIGHT:                                  
      set_number(s_background_layer_1, h1, hour_black);        
      set_number(s_background_layer_3, h2, hour_black);        
      set_number(s_background_layer_2, m1, minute_black);        
      set_number(s_background_layer_4, m2, minute_black);                
    break;
    
    case PMINUTE_DIAGONAL:                                        
      set_number(s_background_layer_1, h1, hour_black);        
      set_number(s_background_layer_4, h2, hour_black);        
      set_number(s_background_layer_3, m1, minute_black);        
      set_number(s_background_layer_2, m2, minute_black);                
    break;    
  }        
          
  if (bluetooth == BLUETOOTH_ON) { //
    static int s_bluetooth_buffer;
    if (bluetooth_connection_service_peek()) {
      s_bluetooth_buffer = 1;
    } else {
      s_bluetooth_buffer = 0;
    }    
    if (s_bluetooth != s_bluetooth_buffer) {             
      vibes_short_pulse();
      s_bluetooth=s_bluetooth_buffer;
    }  
  }
  
  if (bluetooth_grid == BLUETOOTH_GRID_ON) {    
    if (bluetooth_connection_service_peek()) {
      bitmap_layer_set_bitmap(s_background_layer_bluetooth, s_BAT[10]);
    } else {
      bitmap_layer_set_bitmap(s_background_layer_bluetooth, s_BAT[0]);
    }
  }
  
  if (battery == BATTERY_ON) {    
    int ibattery;            
    BatteryChargeState charge_state = battery_state_service_peek();
    ibattery = charge_state.charge_percent / 10;    
    bitmap_layer_set_bitmap(s_background_layer_battery, s_BAT[ibattery]);    
  }  
      
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Heap Available: %d", heap_bytes_free()); 
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Heap Usage: %d", heap_bytes_used());   
}

static void main_window_load(Window *window) {
  
  //nahodny laout minut
  srand(time(NULL));
  s_random = rand() % 3;    
  
  if (backgrnd == BACKGRND_WHITE) {      
    s_BATTERY = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_B);
  } else {
    s_BATTERY = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_W);
  }
  
  int i;
  
  for(i=0;i<=10;i++){  
    s_BAT[i] = gbitmap_create_as_sub_bitmap(s_BATTERY, GRect((10-i)*5, 0, ((10-i)+1)*5, 168));
  }
  
  if (font == FONT_NORMAL) {
    if (backgrnd == BACKGRND_WHITE) {      
      s_FONT_B = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_FONT_B);
      s_FONT_G = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_FONT_G);
    } else {
      s_FONT_B = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_FONT_W);
      s_FONT_G = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_FONT_R);
    }
  }
  
  if (font == FONT_LIGHT) {
    if (backgrnd == BACKGRND_WHITE) {      
      s_FONT_B = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_FONT_B_LIGHT);
      s_FONT_G = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_FONT_G_LIGHT);
    } else {
      s_FONT_B = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_FONT_W_LIGHT);
      s_FONT_G = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_FONT_R_LIGHT);
    }
  }  
  
  for(i=0;i<=9;i++){    
    s_B[i] =  gbitmap_create_as_sub_bitmap(s_FONT_B, GRect(i*42, 0, (i+1)*42, 66));
    s_G[i] =  gbitmap_create_as_sub_bitmap(s_FONT_G, GRect(i*42, 0, (i+1)*42, 66));    
  }      
  
  switch(margin) {
    case MARGIN_MIN:
      x_diff=3; y_diff=3;
      break;
    case MARGIN_LOW:
      x_diff=6; y_diff=6;
      break;
    case MARGIN_MIDDLE:
      x_diff=9; y_diff=9;
      break;
    case MARGIN_HIGH:
      x_diff=12; y_diff=9;
      break;    
    case MARGIN_MAX:
      x_diff=15; y_diff=9;    
      break;    
  }
  
  s_background_layer_1 = bitmap_layer_create(GRect(30-x_diff, 18-y_diff, 42, 66));  //30 18  (default +-3)  144x168  //15,87
  s_background_layer_2 = bitmap_layer_create(GRect(72+x_diff, 18-y_diff, 42, 66));  //72 18  
  s_background_layer_3 = bitmap_layer_create(GRect(30-x_diff, 84+y_diff, 42, 66));  //30 84  
  s_background_layer_4 = bitmap_layer_create(GRect(72+x_diff, 84+y_diff, 42, 66));  //72 84
    
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer_1));  
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer_2));  
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer_3));  
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer_4));     
    
  if (bluetooth_grid == BLUETOOTH_GRID_ON) {
    if (backgrnd == BACKGRND_WHITE) {  //black smaller - big contrast
      s_background_layer_bluetooth = bitmap_layer_create(GRect(141, 0, 3, 168));  //72 0 1 168
    } else {
      s_background_layer_bluetooth = bitmap_layer_create(GRect(143, 0, 1, 168));  //72 0 1 168
    }
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer_bluetooth));  
  }
  
  if  (battery == BATTERY_ON)  {
    if (backgrnd == BACKGRND_WHITE) {
      s_background_layer_battery = bitmap_layer_create(GRect(0, 0, 3, 168));  //144x168
    } else {
      s_background_layer_battery = bitmap_layer_create(GRect(0, 0, 1, 168));  //144x168
    }
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer_battery));      
  }  
  
  int xa_grid; int xd_grid;
  int ya_grid; int yd_grid;
  
  xa_grid = 0; xd_grid = 168;
  ya_grid = 0; yd_grid = 144;    
  
  if (grid != GRID_OFF) {
    s_vertical = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_VERTICAL); 
    s_horizontal = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HORIZONTAL);   
  
    if (grid == GRID_SHORT) {
      xa_grid = 18-(1*y_diff); xd_grid = 132+(2*y_diff);
      ya_grid = 30-(1*x_diff); yd_grid = 84+(2*x_diff);
    } else if (grid == GRID_LONG) {
      xa_grid = 18-(2*y_diff); xd_grid = 132+(4*y_diff);
      ya_grid = 30-(2*x_diff); yd_grid = 84+(4*x_diff);    
    } else if (grid == GRID_FULL) {
      //default after inicialization
    }
    
    s_background_layer_vertical = bitmap_layer_create(GRect(72, xa_grid, 1, xd_grid));  //72 0 1 168
    s_background_layer_horizontal = bitmap_layer_create(GRect(ya_grid, 84, yd_grid, 1));  //0 84 144 1
    
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer_vertical));  
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer_horizontal));                     
    
    bitmap_layer_set_bitmap(s_background_layer_vertical, s_vertical);
    bitmap_layer_set_bitmap(s_background_layer_horizontal, s_horizontal);    
  }

  update_time();
}

static void main_window_unload(Window *window) {  
  
  //Destroy GBitmap  
  int i;  
  for(i=0;i<=10;i++){
    if (i<=9) {
      if (s_B[i]) gbitmap_destroy(s_B[i]);
      if (s_G[i]) gbitmap_destroy(s_G[i]);
    }
    if (s_BAT[i]) gbitmap_destroy(s_BAT[i]);    
  }
    
  gbitmap_destroy(s_FONT_B);
  gbitmap_destroy(s_FONT_G);  
  gbitmap_destroy(s_BATTERY);  
  
  gbitmap_destroy(s_horizontal);
  gbitmap_destroy(s_vertical);    
  
  //Destroy BitmapLayer
  bitmap_layer_destroy(s_background_layer_1);
  bitmap_layer_destroy(s_background_layer_2);
  bitmap_layer_destroy(s_background_layer_3);
  bitmap_layer_destroy(s_background_layer_4);
  
  bitmap_layer_destroy(s_background_layer_vertical);
  bitmap_layer_destroy(s_background_layer_horizontal);  
  bitmap_layer_destroy(s_background_layer_battery);  
  bitmap_layer_destroy(s_background_layer_bluetooth);  
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {      
  //every 15s (only cycle)
  if (pminute == PMINUTE_CYCLE) {
    if(tick_time->tm_sec % 15 == 0) {
      s_cycle++;
      if (s_cycle > 2) {s_cycle = 0;}
      update_time();
    }
  } else {
    update_time();
  }  
}

static void inbox_received_handler(DictionaryIterator *iterator, void *context) {  
  
  Tuple *t = dict_read_first(iterator);

  // For all items
  while(t != NULL) {
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "Key %d has value!", (int)t->key);        
    if (t->key == KEY_FONT) { byte_array[KEY_FONT]=(int)t->value->int32; }
    if (t->key == KEY_MARGIN) { byte_array[KEY_MARGIN]=(int)t->value->int32; }
    if (t->key == KEY_BACKGRND) { byte_array[KEY_BACKGRND]=(int)t->value->int32; }
    if (t->key == KEY_BLUETOOTH) { byte_array[KEY_BLUETOOTH]=(int)t->value->int32; }
    if (t->key == KEY_BLUETOOTH_GRID) { byte_array[KEY_BLUETOOTH_GRID]=(int)t->value->int32; }
    if (t->key == KEY_BATTERY) { byte_array[KEY_BATTERY]=(int)t->value->int32; }
    if (t->key == KEY_CMINUTE) { byte_array[KEY_CMINUTE]=(int)t->value->int32; }
    if (t->key == KEY_CHOUR) { byte_array[KEY_CHOUR]=(int)t->value->int32; }
    if (t->key == KEY_PMINUTE) { byte_array[KEY_PMINUTE]=(int)t->value->int32; }
    if (t->key == KEY_GRID) { byte_array[KEY_GRID]=(int)t->value->int32; }
    
    // Look for next item
    t = dict_read_next(iterator);
  }  
}

static void init() {      
  
  if (persist_exists(DATA_KEY)) {    
    persist_read_data(DATA_KEY, byte_array, sizeof(byte_array));
    font = byte_array[KEY_FONT];  
    margin = byte_array[KEY_MARGIN];
    backgrnd = byte_array[KEY_BACKGRND];
    bluetooth = byte_array[KEY_BLUETOOTH];
    bluetooth_grid = byte_array[KEY_BLUETOOTH_GRID];
    battery = byte_array[KEY_BATTERY];
    cminute = byte_array[KEY_CMINUTE];
    chour = byte_array[KEY_CHOUR];
    pminute = byte_array[KEY_PMINUTE];
    grid = byte_array[KEY_GRID];
    
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "konfigurace nactena");
    
  } else {
    font = FONT_NORMAL;
    margin = MARGIN_LOW;
    backgrnd = BACKGRND_WHITE;
    bluetooth = BLUETOOTH_ON;
    bluetooth_grid = BLUETOOTH_GRID_ON;
    battery = BATTERY_ON;
    cminute = CMINUTE_BLACK;
    chour = CHOUR_GRAY;
    pminute = PMINUTE_DOWN;
    grid = GRID_OFF;
    
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "konfigurace preddefinovana");
  }    
  
  /*for(int i = 0; i <= 10; i++) {
    // Store loop value in array
    APP_LOG(APP_LOG_LEVEL_DEBUG, "nacteno %d", byte_array[i]);
  } */  
  
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  //Black theme
  if (backgrnd == BACKGRND_BLACK) {
    window_set_background_color(s_main_window, GColorBlack);
  }
  
  // Register with TickTimerService  - cyklicky vterinove, jinak minutove
  if (pminute == PMINUTE_CYCLE) {
    tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  } else {
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  }
  
  // Register callbacks

  app_message_register_inbox_received(inbox_received_handler);
  //const uint32_t inbound_size = 200; const uint32_t outbound_size = 20;  
  //int chyba;  
  //chyba = app_message_open(inbound_size, outbound_size);
  //if(chyba != APP_MSG_OK) APP_LOG(APP_LOG_LEVEL_DEBUG, "app_message_open chyba %d", (int)chyba);
  
  int chyba;
  chyba = app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());   
  if (chyba != APP_MSG_OK) APP_LOG(APP_LOG_LEVEL_DEBUG, "app_message_open chyba %d", (int)chyba);
  
  
}

static void deinit() {
  // Destroy Window
  persist_write_data(DATA_KEY, byte_array, sizeof(byte_array));
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "konfigurace ulozena");
  
  /*for(int i = 0; i <= 10; i++) {
    // Store loop value in array
    APP_LOG(APP_LOG_LEVEL_DEBUG, "ulozeno %d", byte_array[i]);
  } */ 
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
