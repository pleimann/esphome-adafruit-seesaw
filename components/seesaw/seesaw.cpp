#include "seesaw.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace seesaw {

static const char *const TAG = "seesaw";

void SeesawComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Seesaw...");

  // Software reset - device needs time to boot
  this->software_reset();
  delay(500);  // Seesaw needs 500ms after reset

  // Read hardware ID with retry
  uint8_t hw_id = 0;
  bool success = false;
  for (int retry = 0; retry < 5; retry++) {
    if (this->read_register(SEESAW_STATUS, SEESAW_STATUS_HW_ID, &hw_id, 1)) {
      success = true;
      break;
    }
    ESP_LOGW(TAG, "Retry %d reading hardware ID...", retry + 1);
    delay(100);
  }
  if (!success) {
    ESP_LOGE(TAG, "Failed to read hardware ID");
    this->mark_failed();
    return;
  }
  this->hardware_id_ = hw_id;

  // Validate hardware ID
  if (hw_id != SEESAW_HW_ID_ATTINY816 && hw_id != SEESAW_HW_ID_ATTINY817 &&
      hw_id != SEESAW_HW_ID_ATTINY1617 && hw_id != SEESAW_HW_ID_ATTINY1616) {
    ESP_LOGE(TAG, "Unknown hardware ID: 0x%02X", hw_id);
    this->mark_failed();
    return;
  }

  // Read version
  uint8_t version_buf[4];
  if (!this->read_register(SEESAW_STATUS, SEESAW_STATUS_VERSION, version_buf, 4, 1000)) {
    ESP_LOGE(TAG, "Failed to read version");
    this->mark_failed();
    return;
  }
  this->version_ = (uint32_t(version_buf[0]) << 24) | (uint32_t(version_buf[1]) << 16) |
                   (uint32_t(version_buf[2]) << 8) | uint32_t(version_buf[3]);

  ESP_LOGCONFIG(TAG, "Seesaw initialized successfully");
}

void SeesawComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "Seesaw:");
  LOG_I2C_DEVICE(this);

  const char *hw_name;
  switch (this->hardware_id_) {
    case SEESAW_HW_ID_ATTINY816:
      hw_name = "ATtiny816";
      break;
    case SEESAW_HW_ID_ATTINY817:
      hw_name = "ATtiny817";
      break;
    case SEESAW_HW_ID_ATTINY1617:
      hw_name = "ATtiny1617";
      break;
    case SEESAW_HW_ID_ATTINY1616:
      hw_name = "ATtiny1616";
      break;
    default:
      hw_name = "Unknown";
      break;
  }
  ESP_LOGCONFIG(TAG, "  Hardware: %s (0x%02X)", hw_name, this->hardware_id_);
  ESP_LOGCONFIG(TAG, "  Version: 0x%08X", this->version_);
}

void SeesawComponent::software_reset() {
  const uint8_t data[] = {0xFF};
  this->write_register(SEESAW_STATUS, SEESAW_STATUS_SWRST, data, 1);
}

bool SeesawComponent::write_register(uint8_t module, uint8_t reg, const uint8_t *data, size_t len) {
  std::vector<uint8_t> buf;
  buf.reserve(2 + len);
  buf.push_back(module);
  buf.push_back(reg);
  if (data != nullptr && len > 0) {
    buf.insert(buf.end(), data, data + len);
  }
  return this->write(buf.data(), buf.size()) == i2c::ERROR_OK;
}

bool SeesawComponent::read_register(uint8_t module, uint8_t reg, uint8_t *data, size_t len, uint16_t delay_us) {
  uint8_t cmd[2] = {module, reg};
  if (this->write(cmd, 2) != i2c::ERROR_OK) {
    return false;
  }
  delayMicroseconds(delay_us);
  return this->read(data, len) == i2c::ERROR_OK;
}

bool SeesawComponent::gpio_set_pin_mode(uint8_t pin, bool output) {
  uint32_t pin_mask = 1UL << pin;
  uint8_t data[4] = {
      uint8_t(pin_mask >> 24),
      uint8_t(pin_mask >> 16),
      uint8_t(pin_mask >> 8),
      uint8_t(pin_mask),
  };
  uint8_t reg = output ? SEESAW_GPIO_DIRSET : SEESAW_GPIO_DIRCLR;
  return this->write_register(SEESAW_GPIO, reg, data, 4);
}

bool SeesawComponent::gpio_set_pullup(uint8_t pin, bool enable) {
  uint32_t pin_mask = 1UL << pin;
  uint8_t data[4] = {
      uint8_t(pin_mask >> 24),
      uint8_t(pin_mask >> 16),
      uint8_t(pin_mask >> 8),
      uint8_t(pin_mask),
  };
  uint8_t reg = enable ? SEESAW_GPIO_PULLENSET : SEESAW_GPIO_PULLENCLR;
  return this->write_register(SEESAW_GPIO, reg, data, 4);
}

bool SeesawComponent::gpio_digital_write(uint8_t pin, bool value) {
  uint32_t pin_mask = 1UL << pin;
  uint8_t data[4] = {
      uint8_t(pin_mask >> 24),
      uint8_t(pin_mask >> 16),
      uint8_t(pin_mask >> 8),
      uint8_t(pin_mask),
  };
  uint8_t reg = value ? SEESAW_GPIO_BULK_SET : SEESAW_GPIO_BULK_CLR;
  return this->write_register(SEESAW_GPIO, reg, data, 4);
}

bool SeesawComponent::gpio_digital_read(uint8_t pin, bool &value) {
  uint32_t values;
  if (!this->gpio_read_bulk(values)) {
    return false;
  }
  value = (values >> pin) & 1;
  return true;
}

bool SeesawComponent::gpio_read_bulk(uint32_t &values) {
  uint8_t data[4];
  if (!this->read_register(SEESAW_GPIO, SEESAW_GPIO_BULK, data, 4)) {
    return false;
  }
  values = (uint32_t(data[0]) << 24) | (uint32_t(data[1]) << 16) | (uint32_t(data[2]) << 8) | uint32_t(data[3]);
  return true;
}

}  // namespace seesaw
}  // namespace esphome
