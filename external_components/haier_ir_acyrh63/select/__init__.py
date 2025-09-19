import esphome.codegen as cg
from esphome.components import select
import esphome.config_validation as cv

from ..climate import CONF_HAIER_ID, HaierIrAcYrh63Climate, haier_ir_acyrh63_ns

FreshSelect = haier_ir_acyrh63_ns.class_("FreshSelect", select.Select)
SterilizeSelect = haier_ir_acyrh63_ns.class_("SterilizeSelect", select.Select)
VrticalSelect = haier_ir_acyrh63_ns.class_("VrticalSelect", select.Select)
HorizontalSelect = haier_ir_acyrh63_ns.class_("HorizontalSelect", select.Select)

CONF_FRESH = "fresh"
CONF_STERILIZE = "sterilize"
CONF_VERTICAL = "vertical"
CONF_HORIZONTAL= "horizontal"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_HAIER_ID): cv.use_id(HaierIrAcYrh63Climate),
        cv.Optional(CONF_FRESH): select.select_schema(
            FreshSelect,
            icon="mdi:air-filter",
        ),
        cv.Optional(CONF_STERILIZE): select.select_schema(
            SterilizeSelect,
            icon="mdi:weather-windy",
        ),
        cv.Optional(CONF_VERTICAL): select.select_schema(
            FreshSelect,
            icon="mdi:arrow-up-down",
        ),
        cv.Optional(CONF_HORIZONTAL): select.select_schema(
            SterilizeSelect,
            icon="mdi:arrow-left-right",
        ),
    }
)

async def to_code(config):
    parent = await cg.get_variable(config[CONF_HAIER_ID])

    if fresh_config := config.get(CONF_FRESH):
        s = await select.new_select(
            fresh_config,
            options=["Off", "Low", "High"],
        )
        await cg.register_parented(s, parent)
        cg.add(parent.set_fresh_select_(s))

    if sterilize_config := config.get(CONF_STERILIZE):
        s = await select.new_select(
            sterilize_config,
            options=["Off", "Turbo", "Quiet"],
        )
        await cg.register_parented(s, parent)
        cg.add(parent.set_sterilize_select_(s))

    if vertical_config := config.get(CONF_VERTICAL):
        s = await select.new_select(
            vertical_config,
            options=["Highest", "High", "Middle", "Low", "Auto"],
        )
        await cg.register_parented(s, parent)
        cg.add(parent.set_vertical_select_(s))

    if horizontal_config := config.get(CONF_HORIZONTAL):
        s = await select.new_select(
            horizontal_config,
            options=["Middle", "LeftRight", "LeftRightMax", "LeftMax", "Left", "Right", "RightMax", "Auto"],
        )
        await cg.register_parented(s, parent)
        cg.add(parent.set_horizontal_select_(s))
