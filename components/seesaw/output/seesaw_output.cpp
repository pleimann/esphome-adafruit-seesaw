#include "seesaw_output.h"
#include "esphome/core/log.h"

namespace esphome {
namespace seesaw {

static const char *const TAG = "seesaw.output";

void SeesawOutput::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Seesaw Output pin %d...", this->pin_);
  this->parent_->gpio_set_pin_mode(this->pin_, true);
}

void SeesawOutput::dump_config() {
  ESP_LOGCONFIG(TAG, "Seesaw Output:");
  ESP_LOGCONFIG(TAG, "  Pin: %d", this->pin_);
}

void SeesawOutput::write_state(bool state) {
  this->parent_->gpio_digital_write(this->pin_, state);
}

}  // namespace seesaw
}  // namespace esphome
