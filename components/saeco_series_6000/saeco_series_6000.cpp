#include "saeco_series_6000.h"
#include "esphome/core/log.h"
#include <vector>

namespace esphome {
namespace saeco_series_6000 {

static const char *TAG = "saeco_series_6000";
static uint32_t last_activity_time = 0;

void SaecoSeries6000::loop() {
  uint8_t byte;
  
  // Bridge from uart_display (display) to uart_mainboard (mainboard)
  while (uart_display_->available()) {
    uart_display_->read_byte(&byte);
    uart_mainboard_->write_byte(byte);  // Mirror byte immediately
    process_display_byte(byte); // Process the "sniffed" byte
  }
  
  // Bridge from uart_mainboard (mainboard) to uart_display (display)
  while (uart_mainboard_->available()) {
    uart_mainboard_->read_byte(&byte);
    uart_display_->write_byte(byte);  // Mirror byte immediately
    process_mainboard_byte(byte); // Process the "sniffed" byte
  }
  // Контроль активности UART (выключение через 15 сек)
  static uint32_t last_check = 0;
  uint32_t now = millis();
  if (now - last_check > 1000) { // проверяем каждую секунду
    last_check = now;
    if (now - last_activity_time > 15000) {
      pub_status(2); // публикуем статус "выключено"
      last_activity_time = now; // чтобы не публиковать повторно
    }
  }
}

void SaecoSeries6000::process_display_byte(uint8_t byte) {
    // Если получили первый 0xAA — начинаем новый пакет
    if (byte == 0xAA && display_buffer_pos_ == 0) {
        display_buffer_[display_buffer_pos_++] = byte;
        return;
    }
    // Если уже начали собирать пакет
    if (display_buffer_pos_ > 0 && display_buffer_pos_ < DISPLAY_BUFFER_SIZE) {
        display_buffer_[display_buffer_pos_++] = byte;
        if (byte == 0x55) {
            parse_display_packet(display_buffer_, display_buffer_pos_);
            display_buffer_pos_ = 0;
        }
    }
}

void SaecoSeries6000::process_mainboard_byte(uint8_t byte) {
    // Если получили первый 0xAA — начинаем новый пакет
    if (byte == 0xAA && mainboard_buffer_pos_ == 0) {
        mainboard_buffer_[mainboard_buffer_pos_++] = byte;
        return;
    }
    // Если уже начали собирать пакет
    if (mainboard_buffer_pos_ > 0 && mainboard_buffer_pos_ < MAINBOARD_BUFFER_SIZE) {
        mainboard_buffer_[mainboard_buffer_pos_++] = byte;
        if (byte == 0x55) {
            parse_mainboard_packet(mainboard_buffer_, mainboard_buffer_pos_);
            mainboard_buffer_pos_ = 0;
        }
    }
}

void SaecoSeries6000::parse_display_packet(const uint8_t* buffer, uint16_t size) {
    if (debug_) {
        ESP_LOGD(TAG, "parse_display_packet: size=%u", size);
    }
    // Здесь больше не парсим B0-пакеты, только debug
}

void SaecoSeries6000::parse_mainboard_packet(const uint8_t* buffer, uint16_t size) {
    last_activity_time = millis(); // сбрасываем таймер активности при любом пакете
    if (debug_) {
        std::vector<char> hex_buffer;
        hex_buffer.resize(size * 3 + 1);
        for (uint16_t i = 0; i < size; i++) {
            sprintf(hex_buffer.data() + i * 3, "%02X ", buffer[i]);
        }
        ESP_LOGD(TAG, "Packet from Mainboard: %s", hex_buffer.data());
        ESP_LOGD(TAG, "parse_mainboard_packet: size=%u", size);
    }
    // Парсим B0
    if (size > 4 && buffer[3] == 0xB0) {
        // Логика из philips_series_5400.cpp
        if (size < 12) return;
        if (buffer[6] == 0x0E) {
            if (buffer[9] == 0x00) {
                pub_status(20); // "Опорож. контейнер для коф. гущи"
            } else {
                if ((buffer[9] & 0x40) != 0) {
                    pub_status(22); // "Воды нет"
                } else {
                    pub_status(17); // "Вода есть"
                }
                if ((buffer[9] & 0x80) != 0) {
                    pub_status(23); // "Извлечен"
                } else {
                    pub_status(18); // "Вставлен"
                }
            }
        } else if (buffer[6] == 0x06) {
            pub_status(21); // "Вода есть","Вставлен","Зерна есть","Выберите напиток","Пустой"
        } else if (buffer[6] == 0x0C) {
            if (buffer[7] == 0x01) {
                pub_status(8); // "Наслаждайтесь"
            } else if (buffer[7] == 0x02) {
                pub_status(15); // "Что-то (07 0C 02)"
            }
        } else if (buffer[6] == 0x07) {
            if (buffer[7] == 0x0E) {
                pub_status(9); // "Нагрев воды"
            } else if (buffer[7] == 0x0D) {
                pub_status(10); // "Перемалываем зерна"
            } else if (buffer[7] == 0x10) {
                pub_status(3); // "Наливаем молоко"
            } else if (buffer[7] == 0x11) {
                pub_status(4); // "Наливаем кофе"
            } else if (buffer[7] == 0x12) {
                pub_status(5); // "Предварительное дозирование"
            } else if (buffer[7] == 0x13) {
                pub_status(6); // "Создание пара для молока"
            } else if (buffer[7] == 0x14) {
                pub_status(7); // "Заварочный узел в положение заваривания"
            } else if (buffer[7] == 0x15) {
                pub_status(15); // "Наслаждайтесь"
            }
        } else if (buffer[6] == 0x08) {
            if (buffer[7] == 0x0E) {
                pub_status(14); // "Нагревание"
            } else if (buffer[7] == 0x02) {
                pub_status(11); // "Промывка"
            } else if (buffer[7] == 0x14) {
                pub_status(13); // "Что-то (07 08 14)"
            } else if (buffer[7] == 0x05) {
                pub_status(12); // "Зерна закончились"
            } else if (buffer[7] == 0x16) {
                pub_status(24); // "Удаление накипи стадия 1"
            } else if (buffer[7] == 0x18) {
                pub_status(25); // "Удаление накипи стадия 2"
            }
        } else if (buffer[7] == 0x00) {
            if (buffer[6] == 0x01) {
                pub_status(1); // "Что-то (07 01 00)"
            } else if (buffer[6] == 0x05) {
                pub_status(2); // "Выключено"
            }
        }
    } else if (size > 12 && buffer[3] == 0xB5) {
        // Парсим B5 (Error Code)
        if (buffer[10] == 0x00) {
            if (buffer[11] == 0x00) pub_status(30);
            else if (buffer[11] == 0x0B) pub_status(31);
            else if (buffer[11] == 0xE6) pub_status(32);
            else if (buffer[11] == 0x80) pub_status(33);
            else if (buffer[11] == 0xCB) pub_status(34);
            else if (buffer[11] == 0xFF) pub_status(35);
            else if (buffer[11] == 0xA0) pub_status(36);
        } else if (buffer[10] == 0x01) {
            pub_status(37); // "Статус2 0x01"
        }
    }
}

void SaecoSeries6000::pub_status(uint8_t status) {
    if (status_sensor_ != nullptr) {
        status_sensor_->publish_state(status);
    }
}

void SaecoSeries6000::dump_config() {
    ESP_LOGCONFIG(TAG, "Saeco Series 6000 UART Bridge");
    ESP_LOGCONFIG(TAG, "  UART Display (uart_display): %p", uart_display_);
    ESP_LOGCONFIG(TAG, "  UART Mainboard (uart_mainboard): %p", uart_mainboard_);
    ESP_LOGCONFIG(TAG, "  Debug mode: %s", ONOFF(this->debug_));
}

}  // namespace saeco_series_6000
}  // namespace esphome 