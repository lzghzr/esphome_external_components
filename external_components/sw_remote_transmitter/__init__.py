from esphome import automation, pins
import esphome.codegen as cg
from esphome.components import esp32, remote_base
from esphome.config_helpers import filter_source_files_from_platform
import esphome.config_validation as cv
from esphome.const import (
    CONF_CARRIER_DUTY_PERCENT,
    CONF_ID,
    CONF_PIN,
    PlatformFramework,
)

AUTO_LOAD = ["remote_base"]

CONF_ON_TRANSMIT = "on_transmit"
CONF_ON_COMPLETE = "on_complete"
CONF_TRANSMITTER_ID = remote_base.CONF_TRANSMITTER_ID

sw_remote_transmitter_ns = cg.esphome_ns.namespace("sw_remote_transmitter")
RemoteTransmitterComponent = sw_remote_transmitter_ns.class_(
    "RemoteTransmitterComponent", remote_base.RemoteTransmitterBase, cg.Component
)

MULTI_CONF = True
CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(RemoteTransmitterComponent),
        cv.Required(CONF_PIN): pins.gpio_output_pin_schema,
        cv.Required(CONF_CARRIER_DUTY_PERCENT): cv.All(
            cv.percentage_int, cv.Range(min=1, max=100)
        ),
        cv.Optional(CONF_ON_TRANSMIT): automation.validate_automation(single=True),
        cv.Optional(CONF_ON_COMPLETE): automation.validate_automation(single=True),
    }
).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    pin = await cg.gpio_pin_expression(config[CONF_PIN])
    var = cg.new_Pvariable(config[CONF_ID], pin)
    await cg.register_component(var, config)

    cg.add(var.set_carrier_duty_percent(config[CONF_CARRIER_DUTY_PERCENT]))

    if on_transmit_config := config.get(CONF_ON_TRANSMIT):
        await automation.build_automation(
            var.get_transmit_trigger(), [], on_transmit_config
        )

    if on_complete_config := config.get(CONF_ON_COMPLETE):
        await automation.build_automation(
            var.get_complete_trigger(), [], on_complete_config
        )

FILTER_SOURCE_FILES = filter_source_files_from_platform(
    {
        "sw_remote_transmitter.cpp": {
            PlatformFramework.ESP32_ARDUINO,
            PlatformFramework.ESP32_IDF,
            PlatformFramework.ESP8266_ARDUINO,
            PlatformFramework.BK72XX_ARDUINO,
            PlatformFramework.RTL87XX_ARDUINO,
            PlatformFramework.LN882X_ARDUINO,
        },
    }
)
