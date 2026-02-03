#pragma once

#include "esphome/core/component.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "../seesaw.h"

namespace esphome {
namespace seesaw {

class SeesawBinarySensor : public binary_sensor::BinarySensor, public PollingComponent {
 public:
  void setup() override;
  void update() override;
  void dump_config() override;

  void set_parent(SeesawComponent *parent) { this->parent_ = parent; }
  void set_pin(uint8_t pin) { this->pin_ = pin; }
  void set_pullup(bool pullup) { this->pullup_ = pullup; }

 protected:
  SeesawComponent *parent_;
  uint8_t pin_;
  bool pullup_{true};
};

}  // namespace seesaw
}  // namespace esphome
