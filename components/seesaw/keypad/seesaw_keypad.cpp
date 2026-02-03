#include "seesaw_keypad.h"
#include "esphome/core/log.h"

namespace esphome {
namespace seesaw {
namespace keypad {

static const char *const TAG = "seesaw.keypad";

KeyEventTrigger::KeyEventTrigger(SeesawKeypad *parent) {
  parent->add_on_key_event_callback([this](KeyEvent event) { this->trigger(event); });
}

void SeesawKeypad::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Seesaw Keypad...");
  // Enable all key events by writing to event register
  // Each bit enables events for that key
  // For now, enable all keys (0xFF for 8 keys)
  uint8_t enable_data = 0xFF;
  this->parent_->write_register(SEESAW_KEYPAD, SEESAW_KEYPAD_EVENT, &enable_data, 1);
}

void SeesawKeypad::update() {
  this->read_fifo_();
}

void SeesawKeypad::dump_config() {
  ESP_LOGCONFIG(TAG, "Seesaw Keypad:");
  ESP_LOGCONFIG(TAG, "  Update interval: %ums", this->get_update_interval());
}

void SeesawKeypad::read_fifo_() {
  // Read number of events in FIFO
  uint8_t count;
  if (!this->parent_->read_register(SEESAW_KEYPAD, SEESAW_KEYPAD_COUNT, &count, 1)) {
    return;
  }

  if (count == 0) {
    return;
  }

  // Read events from FIFO
  // Each event is 1 byte: [key_number:6][edge:2]
  uint8_t events[count];
  if (!this->parent_->read_register(SEESAW_KEYPAD, SEESAW_KEYPAD_FIFO, events, count)) {
    return;
  }

  for (uint8_t i = 0; i < count; i++) {
    KeyEvent event;
    event.key = events[i] & 0x3F;  // Lower 6 bits
    event.edge = static_cast<KeyEdge>((events[i] >> 6) & 0x03);  // Upper 2 bits

    ESP_LOGD(TAG, "Key event: key=%d edge=%d", event.key, event.edge);
    this->key_event_callback_.call(event);
  }
}

}  // namespace keypad
}  // namespace seesaw
}  // namespace esphome
