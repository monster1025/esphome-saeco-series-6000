DEPENDENCIES = ["uart", "sensor"]

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart, sensor
from esphome.const import CONF_ID, CONF_DEBUG

CONF_STATE_SENSOR = "state_sensor"
CONF_B0_STATUS_SENSOR = "b0_status_sensor"
CONF_B5_STATUS_SENSOR = "b5_status_sensor"
CONF_BA_STATUS_SENSOR = "ba_status_sensor"
CONF_S90_STATUS_SENSOR = "s90_status_sensor"
CONF_S91_STATUS_SENSOR = "s91_status_sensor"
CONF_S93_STATUS_SENSOR = "s93_status_sensor"
CONF_WATER_SENSOR = "water_sensor"
CONF_COFFEE_GROUNDS_CONTAINER_SENSOR = "coffee_grounds_container_sensor"
CONF_PALLET_SENSOR = "pallet_sensor"
CONF_ERROR_CODE_SENSOR = "error_code_sensor"
CONF_GRAIN_TRAY_SENSOR = "grain_tray_sensor"
CONF_STATUS_TEXT_SENSOR = "status_text_sensor"

saeco_series_6000_ns = cg.esphome_ns.namespace('saeco_series_6000')
SaecoSeries6000 = saeco_series_6000_ns.class_('SaecoSeries6000', cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(SaecoSeries6000),
    cv.Required('uart_display'): cv.use_id(uart.UARTComponent),
    cv.Required('uart_mainboard'): cv.use_id(uart.UARTComponent),
    cv.Optional(CONF_DEBUG, default=False): cv.boolean,
    cv.Optional(CONF_STATE_SENSOR): sensor.sensor_schema(),
}).extend(cv.COMPONENT_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    
    uart_display = yield cg.get_variable(config['uart_display'])
    cg.add(var.set_uart_display(uart_display))
    
    uart_mainboard = yield cg.get_variable(config['uart_mainboard'])
    cg.add(var.set_uart_mainboard(uart_mainboard))

    if config[CONF_DEBUG]:
        cg.add(var.set_debug(True))

    if CONF_STATE_SENSOR in config:
        sens = yield sensor.new_sensor(config[CONF_STATE_SENSOR])
        cg.add(var.set_status_sensor(sens)) 