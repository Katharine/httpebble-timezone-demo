#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#include "http.h"
#include "itoa.h"

PBL_APP_INFO(HTTP_UUID,
             "Timezone Demo", "Your Company",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_STANDARD_APP);

Window window;
TextLayer nameLayer;
TextLayer offsetLayer;
TextLayer dstLayer;
TextLayer unixLayer;


void have_time(int32_t dst_offset, bool is_dst, uint32_t unixtime, const char* tz_name, void* context) {
	static char stz_name[50];
	static char dst[] = "DST: yes";
	static char dsto[] = "UTC offset: xxxxxxxxxxx";
	static char unix[12];
	text_layer_set_text(&unixLayer, "loaded");
	if(!is_dst) memcpy(dst, "DST: no", 8);
	text_layer_set_text(&dstLayer, dst);

	memcpy(&dsto[12], itoa(dst_offset / 3600), 11);
	text_layer_set_text(&offsetLayer, dsto);
	
	memcpy(&unix, itoa(unixtime), 11);
	text_layer_set_text(&unixLayer, unix);
	
	memcpy(stz_name, tz_name, strlen(tz_name) + 1);
	text_layer_set_text(&nameLayer, stz_name);
}

void handle_init(AppContextRef ctx) {
	window_init(&window, "Window Name");
	window_stack_push(&window, true /* Animated */);
	
	text_layer_init(&nameLayer, GRect(0, 0, 144, 30));
	text_layer_init(&offsetLayer, GRect(0, 30, 144, 30));
	text_layer_init(&dstLayer, GRect(0, 60, 144, 30));
	text_layer_init(&unixLayer, GRect(0, 90, 144, 30));
	layer_add_child(&window.layer, &nameLayer.layer);
	layer_add_child(&window.layer, &offsetLayer.layer);
	layer_add_child(&window.layer, &dstLayer.layer);
	layer_add_child(&window.layer, &unixLayer.layer);

	http_set_app_id(34525634);
	http_register_callbacks((HTTPCallbacks){
		.time=have_time,
	}, NULL);

	http_time_request();
}

void pbl_main(void *params) {
	PebbleAppHandlers handlers = {
		.init_handler = &handle_init,
		.messaging_info = {
			.buffer_sizes = {
				.inbound = 256,
				.outbound = 256,
			}
		},
	};
	app_event_loop(params, &handlers);
}
