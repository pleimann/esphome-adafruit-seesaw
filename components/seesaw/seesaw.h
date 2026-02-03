#pragma once

#include "esphome/core/component.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace seesaw {

// Seesaw module base addresses
enum SeesawModule : uint8_t {
  SEESAW_STATUS = 0x00,
  SEESAW_GPIO = 0x01,
  SEESAW_NEOPIXEL = 0x0E,
  SEESAW_KEYPAD = 0x10,
};

// Status module registers
enum SeesawStatusReg : uint8_t {
  SEESAW_STATUS_HW_ID = 0x01,
  SEESAW_STATUS_VERSION = 0x02,
  SEESAW_STATUS_OPTIONS = 0x03,
  SEESAW_STATUS_SWRST = 0x7F,
};

// GPIO module registers
enum SeesawGPIOReg : uint8_t {
  SEESAW_GPIO_DIRSET = 0x02,
  SEESAW_GPIO_DIRCLR = 0x03,
  SEESAW_GPIO_BULK = 0x04,
  SEESAW_GPIO_BULK_SET = 0x05,
  SEESAW_GPIO_BULK_CLR = 0x06,
  SEESAW_GPIO_PULLENSET = 0x0B,
  SEESAW_GPIO_PULLENCLR = 0x0C,
};

// Hardware IDs for ATtiny 1-series
enum SeesawHardwareID : uint8_t {
  SEESAW_HW_ID_ATTINY816 = 0x84,
  SEESAW_HW_ID_ATTINY817 = 0x85,
  SEESAW_HW_ID_ATTINY1616 = 0x88,
};

class SeesawComponent : public Component, public i2c::I2CDevice {
 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::IO; }

  /// Write data to a seesaw register
  bool write_register(uint8_t module, uint8_t reg, const uint8_t *data, size_t len);
  bool write_register(uint8_t module, uint8_t reg) { return this->write_register(module, reg, nullptr, 0); }

  /// Read data from a seesaw register
  bool read_register(uint8_t module, uint8_t reg, uint8_t *data, size_t len, uint16_t delay_us = 250);

  /// Get hardware ID
  uint8_t get_hardware_id() const { return this->hardware_id_; }

  /// Get firmware version
  uint32_t get_version() const { return this->version_; }

  /// Software reset the seesaw
  void software_reset();

  /// GPIO operations
  bool gpio_set_pin_mode(uint8_t pin, bool output);
  bool gpio_set_pullup(uint8_t pin, bool enable);
  bool gpio_digital_write(uint8_t pin, bool value);
  bool gpio_digital_read(uint8_t pin, bool &value);
  bool gpio_read_bulk(uint32_t &values);

 protected:
  uint8_t hardware_id_{0};
  uint32_t version_{0};
};

}  // namespace seesaw
}  // namespace esphome
