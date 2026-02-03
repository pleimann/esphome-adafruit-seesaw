#pragma once

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "../seesaw.h"

namespace esphome {
namespace seesaw {
namespace keypad {

// Keypad module registers
enum SeesawKeypadReg : uint8_t {
  SEESAW_KEYPAD_STATUS = 0x00,
  SEESAW_KEYPAD_EVENT = 0x01,
  SEESAW_KEYPAD_INTENSET = 0x02,
  SEESAW_KEYPAD_INTENCLR = 0x03,
  SEESAW_KEYPAD_COUNT = 0x04,
  SEESAW_KEYPAD_FIFO = 0x10,
};

enum KeyEdge : uint8_t {
  KEY_EDGE_HIGH = 0,
  KEY_EDGE_LOW = 1,
  KEY_EDGE_FALLING = 2,
  KEY_EDGE_RISING = 3,
};

struct KeyEvent {
  uint8_t key;
  KeyEdge edge;
};

class SeesawKeypad;

class KeyEventTrigger : public Trigger<KeyEvent> {
 public:
  explicit KeyEventTrigger(SeesawKeypad *parent);
};

class SeesawKeypad : public PollingComponent {
 public:
  void setup() override;
  void update() override;
  void dump_config() override;

  void set_parent(SeesawComponent *parent) { this->parent_ = parent; }
  void add_on_key_event_callback(std::function<void(KeyEvent)> &&callback) {
    this->key_event_callback_.add(std::move(callback));
  }

 protected:
  void read_fifo_();

  SeesawComponent *parent_;
  CallbackManager<void(KeyEvent)> key_event_callback_;
};

}  // namespace keypad
}  // namespace seesaw
}  // namespace esphome
