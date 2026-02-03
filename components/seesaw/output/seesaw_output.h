#pragma once

#include "esphome/core/component.h"
#include "esphome/components/output/binary_output.h"
#include "../seesaw.h"

namespace esphome {
namespace seesaw {

class SeesawOutput : public output::BinaryOutput, public Component {
 public:
  void setup() override;
  void dump_config() override;

  void set_parent(SeesawComponent *parent) { this->parent_ = parent; }
  void set_pin(uint8_t pin) { this->pin_ = pin; }

 protected:
  void write_state(bool state) override;

  SeesawComponent *parent_;
  uint8_t pin_;
};

}  // namespace seesaw
}  // namespace esphome
