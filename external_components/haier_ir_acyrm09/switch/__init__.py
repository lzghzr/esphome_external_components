import esphome.codegen as cg
from esphome.components import switch
import esphome.config_validation as cv

from ..climate import CONF_HAIER_ID, HaierIrAcYrm09Climate, haier_ir_acyrm09_ns

HealthSwitch = haier_ir_acyrm09_ns.class_("HealthSwitch", switch.Switch)
HeatingSwitch = haier_ir_acyrm09_ns.class_("HeatingSwitch", switch.Switch)

CONF_HEALTH = "health"
CONF_HEATING = "heating"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_HAIER_ID): cv.use_id(HaierIrAcYrm09Climate),
        cv.Optional(CONF_HEALTH): switch.switch_schema(
            HealthSwitch,
            icon="mdi:leaf",
            default_restore_mode="DISABLED",
        ),
        cv.Optional(CONF_HEATING): switch.switch_schema(
            HeatingSwitch,
            icon="mdi:heating-coil",
            default_restore_mode="DISABLED",
        ),
    }
)

async def to_code(config):
    parent = await cg.get_variable(config[CONF_HAIER_ID])

    for switch_type in [CONF_HEALTH, CONF_HEATING]:
        if conf := config.get(switch_type):
            sw_var = await switch.new_switch(conf)
            await cg.register_parented(sw_var, parent)
            cg.add(getattr(parent, f"set_{switch_type}_switch_")(sw_var))
