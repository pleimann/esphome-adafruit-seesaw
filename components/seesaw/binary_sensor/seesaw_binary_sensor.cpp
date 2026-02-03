#include "seesaw_binary_sensor.h"
#include "esphome/core/log.h"

namespace esphome {
namespace seesaw {

static const char *const TAG = "seesaw.binary_sensor";

void SeesawBinarySensor::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Seesaw Binary Sensor pin %d...", this->pin_);
  this->parent_->gpio_set_pin_mode(this->pin_, false);  // Input
  if (this->pullup_) {
    this->parent_->gpio_set_pullup(this->pin_, true);
  }
}

void SeesawBinarySensor::update() {
  bool value;
  if (this->parent_->gpio_digital_read(this->pin_, value)) {
    this->publish_state(value);
  }
}

void SeesawBinarySensor::dump_config() {
  LOG_BINARY_SENSOR("", "Seesaw Binary Sensor", this);
  ESP_LOGCONFIG(TAG, "  Pin: %d", this->pin_);
  ESP_LOGCONFIG(TAG, "  Pullup: %s", YESNO(this->pullup_));
}

}  // namespace seesaw
}  // namespace esphome
