#include "seesaw_neopixel.h"
#include "esphome/core/log.h"

namespace esphome {
namespace seesaw {
namespace neopixel {

static const char *const TAG = "seesaw.neopixel";

void SeesawNeopixelLight::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Seesaw NeoPixel...");

  // Allocate buffer (3 bytes per pixel for RGB)
  size_t buffer_size = this->num_leds_ * 3;
  this->buffer_ = std::make_unique<uint8_t[]>(buffer_size);
  memset(this->buffer_.get(), 0, buffer_size);

  // Allocate effect data
  this->effect_data_ = new uint8_t[this->num_leds_];
  memset(this->effect_data_, 0, this->num_leds_);

  // Set NeoPixel pin
  this->parent_->write_register(SEESAW_NEOPIXEL, SEESAW_NEOPIXEL_PIN, &this->pin_, 1);

  // Set speed to 800KHz
  uint8_t speed = 1;
  this->parent_->write_register(SEESAW_NEOPIXEL, SEESAW_NEOPIXEL_SPEED, &speed, 1);

  // Set buffer length
  uint8_t len_data[2] = {
      uint8_t(buffer_size >> 8),
      uint8_t(buffer_size & 0xFF),
  };
  this->parent_->write_register(SEESAW_NEOPIXEL, SEESAW_NEOPIXEL_BUF_LENGTH, len_data, 2);

  // Clear all pixels
  this->clear();
  this->show();
}

void SeesawNeopixelLight::dump_config() {
  ESP_LOGCONFIG(TAG, "Seesaw NeoPixel:");
  ESP_LOGCONFIG(TAG, "  Pin: %d", this->pin_);
  ESP_LOGCONFIG(TAG, "  Num LEDs: %d", this->num_leds_);
  ESP_LOGCONFIG(TAG, "  Color Order: %d", this->color_order_);
}

light::LightTraits SeesawNeopixelLight::get_traits() {
  auto traits = light::LightTraits();
  traits.set_supported_color_modes({light::ColorMode::RGB});
  return traits;
}

void SeesawNeopixelLight::write_state(light::LightState *state) {
  this->send_buffer_();
  this->parent_->write_register(SEESAW_NEOPIXEL, SEESAW_NEOPIXEL_SHOW);
  this->mark_shown_();
}

void SeesawNeopixelLight::clear_effect_data() {
  for (int i = 0; i < this->size(); i++) {
    this->effect_data_[i] = 0;
  }
}

light::ESPColorView SeesawNeopixelLight::get_view_internal(int32_t index) const {
  size_t offset = index * 3;
  uint8_t *base = this->buffer_.get() + offset;

  // Map based on color order
  uint8_t *r, *g, *b;
  switch (this->color_order_) {
    case COLOR_ORDER_RGB:
      r = base + 0; g = base + 1; b = base + 2;
      break;
    case COLOR_ORDER_RBG:
      r = base + 0; g = base + 2; b = base + 1;
      break;
    case COLOR_ORDER_GRB:
      r = base + 1; g = base + 0; b = base + 2;
      break;
    case COLOR_ORDER_GBR:
      r = base + 2; g = base + 0; b = base + 1;
      break;
    case COLOR_ORDER_BRG:
      r = base + 1; g = base + 2; b = base + 0;
      break;
    case COLOR_ORDER_BGR:
      r = base + 2; g = base + 1; b = base + 0;
      break;
    default:
      r = base + 1; g = base + 0; b = base + 2;  // GRB default
      break;
  }

  return light::ESPColorView(r, g, b, nullptr, this->effect_data_ + index, &this->correction_);
}

void SeesawNeopixelLight::set_pixel(uint16_t index, uint8_t r, uint8_t g, uint8_t b) {
  if (index >= this->num_leds_) {
    return;
  }
  auto view = this->get_view_internal(index);
  view.set_red(r);
  view.set_green(g);
  view.set_blue(b);
}

void SeesawNeopixelLight::show() {
  this->send_buffer_();
  this->parent_->write_register(SEESAW_NEOPIXEL, SEESAW_NEOPIXEL_SHOW);
}

void SeesawNeopixelLight::clear() {
  size_t buffer_size = this->num_leds_ * 3;
  memset(this->buffer_.get(), 0, buffer_size);
}

void SeesawNeopixelLight::send_buffer_() {
  // Send buffer in chunks (max ~30 bytes per I2C transaction for safety)
  const size_t chunk_size = 24;  // 8 pixels per chunk
  size_t buffer_size = this->num_leds_ * 3;

  for (size_t offset = 0; offset < buffer_size; offset += chunk_size) {
    size_t remaining = buffer_size - offset;
    size_t to_send = std::min(remaining, chunk_size);

    // Data format: [offset_hi, offset_lo, pixel_data...]
    uint8_t data[2 + chunk_size];
    data[0] = offset >> 8;
    data[1] = offset & 0xFF;
    memcpy(data + 2, this->buffer_.get() + offset, to_send);

    this->parent_->write_register(SEESAW_NEOPIXEL, SEESAW_NEOPIXEL_BUF, data, 2 + to_send);
  }
}

}  // namespace neopixel
}  // namespace seesaw
}  // namespace esphome
