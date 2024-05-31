#include "dwin.h"
#include "esphome/core/log.h"

namespace esphome {
namespace dwin {

static const char *TAG = "dwin";

void DWIN::setup() {
}

void DWIN::loop() {
  while (this->available()) {
    uint8_t c;
    this->read_byte(&c);
    this->handle_char_(c);
  }
}

void DWIN::handle_char_(uint8_t c) {
  if (c == '\r')
    return;
  if (c == '\n') {
    std::string s(this->rx_message_.begin(), this->rx_message_.end());
    if (this->the_text_ != nullptr)
      this->the_text_->publish_state(s);
    if (this->the_sensor_ != nullptr)
      this->the_sensor_->publish_state(parse_number<float>(s).value_or(0));
    if (this->the_binsensor_ != nullptr)
      this->the_binsensor_->publish_state(s == "ON");
    this->rx_message_.clear();
    return;
  }
  this->rx_message_.push_back(c);
}

void DWIN::dump_config() {
  LOG_TEXT_SENSOR("", "The Text Sensor", this->the_text_);
  LOG_SENSOR("", "The Sensor", this->the_sensor_);
}

void DWIN::write_binary(bool state) {
  this->write_str(ONOFF(state));
}

void DWIN::ping() {
  this->write_str("PING");
}

void DWIN::write_float(float state) {
  this->write_str(to_string(state).c_str());
}

void DWINBOutput::dump_config() {
  LOG_BINARY_OUTPUT(this);
}

void DWINBOutput::write_state(bool state) {
  this->parent_->write_binary(state);
}

void DWINFOutput::dump_config() {
  LOG_FLOAT_OUTPUT(this);
}

void DWINFOutput::write_state(float state) {
  this->parent_->write_float(state);
}

void DWINSwitch::dump_config() {
  LOG_SWITCH("", "UART Demo Switch", this);
}

void DWINSwitch::write_state(bool state) {
  this->parent_->write_binary(state);
  this->publish_state(state);
}

void DWINButton::dump_config() {
  LOG_BUTTON("", "UART Demo Button", this);
}

void DWINButton::press_action() {
  this->parent_->ping();
}

}  // namespace dwin
}  // namespace esphome
