import esphome.codegen as cg
from esphome.components import button
import esphome.config_validation as cv

from ..climate import CONF_HAIER_ID, HaierIrAcYrh63Climate, haier_ir_acyrh63_ns

LightButton = haier_ir_acyrh63_ns.class_("LightButton", button.Button)

CONF_LIGHT = "light"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_HAIER_ID): cv.use_id(HaierIrAcYrh63Climate),
        cv.Optional(CONF_LIGHT): button.button_schema(
            LightButton,
            icon="mdi:lightbulb-on",
        ),
    }
)

async def to_code(config):
    parent = await cg.get_variable(config[CONF_HAIER_ID])

    if light_conf := config.get(CONF_LIGHT):
        b = await button.new_button(light_conf)
        await cg.register_parented(b, parent)
        cg.add(parent.set_light_button_(b))
