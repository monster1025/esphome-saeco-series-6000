#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace saeco_series_6000 {

// Buffer sizes for packet sniffing
constexpr size_t DISPLAY_BUFFER_SIZE = 128;
constexpr size_t MAINBOARD_BUFFER_SIZE = 128;

class SaecoSeries6000 : public Component {
 public:
  void set_uart_display(uart::UARTComponent *uart_display) { uart_display_ = uart_display; }
  void set_uart_mainboard(uart::UARTComponent *uart_mainboard) { uart_mainboard_ = uart_mainboard; }
  void set_debug(bool debug) { debug_ = debug; }
  void set_status_sensor(sensor::Sensor *sensor) { status_sensor_ = sensor; }
  void loop() override;
  void dump_config() override;

 protected:
  uart::UARTComponent *uart_display_;
  uart::UARTComponent *uart_mainboard_;
  bool debug_{false};
  sensor::Sensor *status_sensor_{nullptr};

 private:
  uint8_t display_buffer_[DISPLAY_BUFFER_SIZE];
  uint16_t display_buffer_pos_{0};

  uint8_t mainboard_buffer_[MAINBOARD_BUFFER_SIZE];
  uint16_t mainboard_buffer_pos_{0};

  uint8_t display_last_bytes_[3] = {0, 0, 0};
  uint8_t mainboard_last_bytes_[3] = {0, 0, 0};
  
  uint8_t display_sync_count_{0};
  uint8_t mainboard_sync_count_{0};
  
  void process_display_byte(uint8_t byte);
  void process_mainboard_byte(uint8_t byte);

  void parse_display_packet(const uint8_t* buffer, uint16_t size);
  void parse_mainboard_packet(const uint8_t* buffer, uint16_t size);
  void pub_status(uint8_t status);
};

}  // namespace saeco_series_6000
}  // namespace esphome 