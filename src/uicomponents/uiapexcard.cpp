#include "uiapexcard.hpp"
#include <src/extra/widgets/chart/lv_chart.h>
#include <src/misc/lv_area.h>
#include <src/misc/lv_color.h>

// FIXME: we do a whole lot of json parsing in this file, that we should be doing somewhere else.

UIApexCard::UIApexCard(HABackend &_backend, const std::string _panel, int _index, lv_obj_t* _parent) :
  UIEntity(nullptr, _parent)
{

  // auto state = entity->getJsonState();
  // std::cerr << "INITIAL STATE FOR " << _entity->name << ":" << state.dump(2) << std::endl;

  // if (!state.contains("attributes")) {
  //   throw std::runtime_error("RGBLight can't operate with a state that has no attributes");
  // }

  // auto attributes = state["attributes"];
  // if (!attributes.contains("supported_color_modes") || !attributes["supported_color_modes"].is_array()) {
  //   throw std::runtime_error("RGBLight really needs to know the supported_color_modes as an array");
  // }
  // std::vector<std::string> supported_color_modes = attributes["supported_color_modes"].get<std::vector<string>>();

  // // FIXME: we get all the supported color modes here, but we don't support all of them. Missing is RGBW, RGBWW and WHITE
  // for (auto& mode : supported_color_modes) {
  //   std::cerr << "    supported color mode:" << mode << std::endl;
  //   std::transform(mode.begin(), mode.end(), mode.begin(), [](unsigned char _c) { return std::tolower(_c); }); // this needed?
  //   if (mode == "unknown") {
  //     throw std::runtime_error("support color mode is 'unknown', we really can't deal with that.");
  //   }
  //   if (mode == "brightness") {
  //     showBrightness = true;
  //   }
  //   if (mode == "color_temp") { // FIXME: HA docs say "and it's color temperature is present in the state".
  //     showBrightness = true;
  //     showColorTemp = true;
  //   }
  //   if (mode == "hs") {
  //     showColorWheel = true;
  //     showBrightness = true;
  //   }
  //   if (mode == "rgbw") {
  //     showColorWheel = true;
  //     showBrightness = true;
  //   }
  //   if (mode == "rgb" || mode == "rgbww") {
  //     showColorWheel = true;
  //     showBrightness = true;
  //     showColorTemp = true;
  //   }
  //   if (mode == "white") {
  //     showWhite = true;
  //   }
  // }

  // // We generate a UI based on 'supported_color_modes'. color_mode then tells us which mode to use. Color_mode should be in uiupdate()
  flowpanel = lv_obj_create(_parent);
  lv_obj_set_width(flowpanel, uiEntityWidth*3);
  lv_obj_set_height(flowpanel, 450);
  // lv_obj_set_style_pad_all(flowpanel, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
  // lv_obj_set_align(flowpanel, LV_ALIGN_CENTER);
  // lv_obj_set_flex_flow(flowpanel, LV_FLEX_FLOW_COLUMN);
  // lv_obj_set_flex_align(flowpanel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);

  // lv_coord_t widthheight = uiEntityWidth - (lv_coord_t)50;

  chart = lv_chart_create(flowpanel);
  lv_obj_set_size(chart, uiEntityWidth*3 - 100, 350);
  lv_obj_center(chart);
  lv_chart_set_type(chart, LV_CHART_TYPE_BAR);
  lv_chart_set_point_count(chart, 24); // hours
  lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 200, 300); // FIXME we're going to fake milli-EUR here instead of euros
  lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_X, 24, 5, 24, 1, true, 40); // FIXME document what these args mean because the 8.3 lvgl docs aren't helping me
  lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_Y, 10, 5, 6, 2, true, 50);

  ser1 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_BLUE), LV_CHART_AXIS_PRIMARY_Y);
  lv_coord_t* ser1_array = lv_chart_get_y_array(chart, ser1);

  auto data = _backend.getEntityByName("sensor.current_electricity_price_all_in");

  std::cerr<<data->toString()<<std::endl;

  for(int i=0; i<24; i++) {
    ser1_array[i] = data->getJsonState()["attributes"]["prices"][i]["price"].get<float>()*1000;
  }

  // lv_obj_t* label = createLabel(flowpanel, entity->name);
  // lv_obj_set_width(label, LV_PCT(100));
  // lv_obj_set_align(label, LV_ALIGN_CENTER);
  // lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);

  // brightnessLabel = createLabel(flowpanel, "");
  // lv_obj_set_width(brightnessLabel, LV_PCT(100));
  // lv_obj_set_align(brightnessLabel, LV_ALIGN_CENTER);
  // lv_obj_set_style_text_align(brightnessLabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);

  // tilecontainer = lv_tileview_create(flowpanel);
  // lv_obj_add_event_cb(tilecontainer, UIApexCard::changeTileCB, LV_EVENT_VALUE_CHANGED, reinterpret_cast<void*>(this));
  // lv_obj_set_height(tilecontainer, uiEntityWidth);

  // lv_coord_t sliderheight = widthheight - (lv_coord_t)25;

  // if (showBrightness) {
  //   lv_obj_t* brightness_tile = lv_tileview_add_tile(tilecontainer, 0, 0, LV_DIR_HOR);
  //   brightnessSlider = lv_slider_create(brightness_tile);
  //   lv_obj_set_height(brightnessSlider, sliderheight);
  //   lv_obj_set_width(brightnessSlider, 50);
  //   lv_obj_set_align(brightnessSlider, LV_ALIGN_CENTER);
  //   lv_slider_set_range(brightnessSlider, 0, 255);
  //   lv_obj_add_event_cb(brightnessSlider, UIApexCard::slideBrightnessCB, LV_EVENT_VALUE_CHANGED, reinterpret_cast<void*>(&entity));
  // }

  // if (showColorWheel) {
  //   lv_obj_t* cw_tile = lv_tileview_add_tile(tilecontainer, 1, 0, LV_DIR_HOR);
  //   cw = lv_colorwheel_create(cw_tile, true);
  //   lv_obj_set_size(cw, widthheight, widthheight);
  //   lv_obj_set_align(cw, LV_ALIGN_CENTER);
  //   lv_colorwheel_set_mode_fixed(cw, false);
  //   lv_obj_add_event_cb(cw, UIApexCard::changeColorWheelCB, LV_EVENT_VALUE_CHANGED, reinterpret_cast<void*>(this));
  //   lv_obj_set_style_arc_width(cw, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
  //   lv_obj_set_style_border_color(cw, lv_color_hex(0xFFFFFF), LV_PART_KNOB | LV_STATE_DEFAULT);
  //   lv_obj_set_style_opa(cw, 255, LV_PART_KNOB | LV_STATE_DEFAULT);
  //   lv_obj_set_style_border_opa(cw, 255, LV_PART_KNOB | LV_STATE_DEFAULT);
  //   lv_obj_set_style_border_width(cw, 3, LV_PART_KNOB | LV_STATE_DEFAULT);
  //   lv_obj_set_style_pad_all(cw, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
  // }

  // if (showColorTemp) {

  //   // TODO: RGBW and RGBWW have a few sliders underneath the color wheels
  //   // In white mode, the indicator on the color wheel should be removed. Not sure if we can do that.

  //   lv_obj_t* colortemp_tile = lv_tileview_add_tile(tilecontainer, 2, 0, LV_DIR_HOR);
  //   colortempSlider = lv_slider_create(colortemp_tile);
  //   lv_obj_set_height(colortempSlider, sliderheight);
  //   lv_obj_set_width(colortempSlider, 50);
  //   lv_obj_set_align(colortempSlider, LV_ALIGN_CENTER);
  //   int min_color_temp = getIntAttribute("min_color_temp_kelvin", 2000);
  //   int max_color_temp = getIntAttribute("max_color_temp_kelvin", 6500);
  //   // FIXME: for whatever reason we can't get this to work the opposite way around, which is what HA does
  //   lv_slider_set_range(colortempSlider, min_color_temp, max_color_temp);
  //   lv_obj_add_event_cb(colortempSlider, UIApexCard::slideColorTempCB, LV_EVENT_VALUE_CHANGED, reinterpret_cast<void*>(&entity));
  // }

  // btns = lv_obj_create(flowpanel);
  // lv_obj_remove_style_all(btns);
  // lv_obj_set_width(btns, uiEntityWidth - 15);
  // lv_obj_set_height(btns, 50);
  // lv_obj_set_style_pad_all(btns, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  // lv_obj_set_align(btns, LV_ALIGN_CENTER);
  // lv_obj_set_flex_flow(btns, LV_FLEX_FLOW_ROW);
  // lv_obj_set_flex_align(btns, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  // lv_obj_clear_flag(btns, LV_OBJ_FLAG_SCROLLABLE);

  // btnOnOff = createImageButton(LV_SYMBOL_POWER, UIApexCard::btnOnOffCB, LV_EVENT_VALUE_CHANGED, true);

  // if (showBrightness) {
  //   btnBrightness = createImageButton(&G_BRIGHTNESS24, UIApexCard::btnBrightnessCB, LV_EVENT_CLICKED);
  //   lv_obj_add_state(btnBrightness, LV_STATE_CHECKED);
  // }

  // if (showColorWheel) {
  //   btnColorWheel = createImageButton(&G_COLORWHEEL24, UIApexCard::btnColorWheelCB, LV_EVENT_CLICKED);
  // }
  // if (showColorTemp) {
  //   btnColorTemp = createImageButton(&G_COLORTEMP24, UIApexCard::btnColorTempCB, LV_EVENT_CLICKED);
  // }

  uiupdate();
}

void UIApexCard::uiupdate()
{
  lv_chart_refresh(chart);
  // auto state = entity->getJsonState();
  // string colormode = getColorMode();
  // // std::cerr << "COLOR MODE: " << colormode << std::endl;
  // std::cerr << "UPDATED STATE FOR " << entity->name << ":" << state.dump(2) << std::endl;

  // {
  //   std::unique_lock<std::mutex> lvlock(g_lvgl_updatelock);
  //   if (state["state"] == "on") { // FIXME: We should get rid of parsing JSON here
  //     lv_obj_add_state(btnOnOff, LV_STATE_CHECKED);
  //     int brightness = state["attributes"]["brightness"].get<int>(); // brightness is NULL If the thing is off
  //     lv_slider_set_value(brightnessSlider, brightness, LV_ANIM_OFF);
  //     int brightness_percent = static_cast<int>(brightness / 2.55);
  //     std::string str_brightness_percent = std::to_string(brightness_percent);
  //     lv_label_set_text(brightnessLabel, (str_brightness_percent + "%").c_str());
  //     if (colormode == "hs") {

  //       std::vector<int> vec_hs = state["attributes"]["hs_color"];

  //       lv_color_hsv_t hsv_val;
  //       hsv_val.h = vec_hs.at(0);
  //       hsv_val.s = vec_hs.at(1);
  //       hsv_val.v = brightness_percent;
  //       lv_colorwheel_set_hsv(cw, hsv_val);
  //     }
  //     else if (colormode == "color_temp") {
  //       int colortemp = state["attributes"]["color_temp_kelvin"].get<int>();
  //       std::cerr << "colormode == color temp and colortemp itself is: " << colortemp << std::endl;
  //       //   lv_slider_set_value(colortempSlider, colortemp, LV_ANIM_OFF);
  //     }
  //     else if (colormode == "brightness") {
  //       std::cerr << "BRIGHTNESS" << std::endl;
  //     }
  //     else if (colormode == "white") {
  //       std::cerr << "WHITE" << std::endl;
  //     }
  //     else {
  //       std::cerr << " NO COLOR MODE " << colormode << " SO USING RGB" << std::endl;
  //       std::vector<int> vec_rgb = state["attributes"]["rgb_color"];

  //       int rgb[3];
  //       short cnt = 0;
  //       for (auto col : vec_rgb) {
  //         rgb[cnt] = col;
  //         cnt++;
  //       }
  //       lv_colorwheel_set_rgb(cw, lv_color_make(rgb[0], rgb[1], rgb[2]));
  //     }
  //   }
  //   else {
  //     lv_obj_clear_state(btnOnOff, LV_STATE_CHECKED);
  //     lv_slider_set_value(brightnessSlider, 0, LV_ANIM_OFF);
  //     lv_label_set_text(brightnessLabel, "Off");
  //   }
  // }
}

