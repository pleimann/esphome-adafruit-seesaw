#pragma once

#include "esphome/core/component.h"
#include "esphome/components/light/addressable_light.h"
#include "../seesaw.h"

namespace esphome {
namespace seesaw {
namespace neopixel {

// NeoPixel module registers
enum SeesawNeopixelReg : uint8_t {
  SEESAW_NEOPIXEL_STATUS = 0x00,
  SEESAW_NEOPIXEL_PIN = 0x01,
  SEESAW_NEOPIXEL_SPEED = 0x02,
  SEESAW_NEOPIXEL_BUF_LENGTH = 0x03,
  SEESAW_NEOPIXEL_BUF = 0x04,
  SEESAW_NEOPIXEL_SHOW = 0x05,
};

enum ColorOrder : uint8_t {
  COLOR_ORDER_RGB = 0,
  COLOR_ORDER_RBG = 1,
  COLOR_ORDER_GRB = 2,
  COLOR_ORDER_GBR = 3,
  COLOR_ORDER_BRG = 4,
  COLOR_ORDER_BGR = 5,
};

class SeesawNeopixelLight : public light::AddressableLight {
 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::HARDWARE; }

  // AddressableLight interface
  int32_t size() const override { return this->num_leds_; }
  light::LightTraits get_traits() override;
  void write_state(light::LightState *state) override;
  void clear_effect_data() override;

  // Configuration setters
  void set_parent(SeesawComponent *parent) { this->parent_ = parent; }
  void set_pin(uint8_t pin) { this->pin_ = pin; }
  void set_num_leds(uint16_t num) { this->num_leds_ = num; }
  void set_color_order(ColorOrder order) { this->color_order_ = order; }

  // Raw API for direct pixel control
  void set_pixel(uint16_t index, uint8_t r, uint8_t g, uint8_t b);
  void show();
  void clear();

 protected:
  light::ESPColorView get_view_internal(int32_t index) const override;
  void send_buffer_();

  SeesawComponent *parent_;
  uint8_t pin_;
  uint16_t num_leds_;
  ColorOrder color_order_{COLOR_ORDER_GRB};
  std::unique_ptr<uint8_t[]> buffer_;
  uint8_t *effect_data_{nullptr};
};

}  // namespace neopixel
}  // namespace seesaw
}  // namespace esphome
