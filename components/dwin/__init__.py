import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components import uart, output
from esphome.const import CONF_ID

MULTI_CONF = True
DEPENDENCIES = ['uart']
AUTO_LOAD = ['output']


dwin_ns = cg.esphome_ns.namespace('dwin')

DWIN = dwin_ns.class_('Dwin', cg.Component, uart.UARTDevice)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(DWIN),
}).extend(uart.UART_DEVICE_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
  

