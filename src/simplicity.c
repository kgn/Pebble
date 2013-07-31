#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define SCREEN_HEIGHT 168

#define PADDING 8
#define BOTTOM_OFFSET 68
#define DATE_BOTTOM_OFFSET BOTTOM_OFFSET
#define LINE_BOTTOM_OFFSET BOTTOM_OFFSET-2
#define TIME_BOTTOM_OFFSET BOTTOM_OFFSET+24

#define MY_UUID { 0xEA, 0x7D, 0x3A, 0x11, 0x90, 0xF7, 0x42, 0xD5, 0xBE, 0x6C, 0xF8, 0xA5, 0x51, 0x7C, 0x26, 0x55 }
PBL_APP_INFO(MY_UUID, "KGN Simple", "@iamkgn", 1, 0 /* App version */, RESOURCE_ID_IMAGE_MENU_ICON, APP_INFO_WATCH_FACE);

Window window;

TextLayer text_date_layer;
TextLayer text_time_layer;

Layer line_layer;

void line_layer_update_callback(Layer *layer, GContext *ctx){
    graphics_context_set_stroke_color(ctx, GColorWhite);

    int16_t bottomOffset = window.layer.bounds.size.h-LINE_BOTTOM_OFFSET;
    graphics_draw_line(ctx, GPoint(PADDING, bottomOffset), GPoint(layer->bounds.size.w-PADDING, bottomOffset));
    graphics_draw_line(ctx, GPoint(PADDING, bottomOffset+1), GPoint(layer->bounds.size.w-PADDING, bottomOffset+1));

}


void handle_init(AppContextRef ctx){
    window_init(&window, "KGN Simple");
    window_stack_push(&window, true /* Animated */);
    window_set_background_color(&window, GColorBlack);

    resource_init_current_app(&APP_RESOURCES);

    // Date
    text_layer_init(&text_date_layer, window.layer.bounds);
    text_layer_set_text_color(&text_date_layer, GColorWhite);
    text_layer_set_background_color(&text_date_layer, GColorClear);
    layer_set_frame(&text_date_layer.layer, GRect(PADDING, DATE_BOTTOM_OFFSET, window.layer.bounds.size.w-PADDING, window.layer.bounds.size.h-DATE_BOTTOM_OFFSET));
    text_layer_set_font(&text_date_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_21)));
    layer_add_child(&window.layer, &text_date_layer.layer);

    // Time
    text_layer_init(&text_time_layer, window.layer.bounds);
    text_layer_set_text_color(&text_time_layer, GColorWhite);
    text_layer_set_background_color(&text_time_layer, GColorClear);
    layer_set_frame(&text_time_layer.layer, GRect(PADDING, TIME_BOTTOM_OFFSET, window.layer.bounds.size.w-PADDING, window.layer.bounds.size.h-TIME_BOTTOM_OFFSET));
    text_layer_set_font(&text_time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_BOLD_SUBSET_49)));
    layer_add_child(&window.layer, &text_time_layer.layer);

    // Line
    layer_init(&line_layer, window.layer.bounds);
    line_layer.update_proc = &line_layer_update_callback;
    layer_add_child(&window.layer, &line_layer);


    // TODO: Update display here to avoid blank display on launch?
}


void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t){
    // Need to be static because they're used by the system later.
    static char time_text[] = "00:00";
    static char date_text[] = "Day, Mon 00";

    char *time_format;

    // TODO: Only update the date when it's changed.
    string_format_time(date_text, sizeof(date_text), "%a, %b %e", t->tick_time);
    text_layer_set_text(&text_date_layer, date_text);


    if(clock_is_24h_style()){
        time_format = "%R";
    }else{
        time_format = "%I:%M";
    }

    string_format_time(time_text, sizeof(time_text), time_format, t->tick_time);

    // Kludge to handle lack of non-padded hour format string
    // for twelve hour clock.
    if (!clock_is_24h_style() && (time_text[0] == '0')) {
        memmove(time_text, &time_text[1], sizeof(time_text) - 1);
    }

    text_layer_set_text(&text_time_layer, time_text);
}


void pbl_main(void *params) {
    PebbleAppHandlers handlers = {
        .init_handler = &handle_init,

        .tick_info = {
            .tick_handler = &handle_minute_tick,
            .tick_units = MINUTE_UNIT
        }

    };
    app_event_loop(params, &handlers);
}
