import esphome.codegen as cg
from esphome import automation
from esphome.components import climate_ir

AUTO_LOAD = ["climate_ir"]

haier_ir_acyrh63_ns = cg.esphome_ns.namespace("haier_ir_acyrh63")
HaierIrAcYrh63Climate = haier_ir_acyrh63_ns.class_("HaierIrAcYrh63", climate_ir.ClimateIR)
TurnOnAction = haier_ir_acyrh63_ns.class_("TurnOnAction", automation.Action)

CONF_HAIER_ID = "haier_acyrh63_id"

CONFIG_SCHEMA = climate_ir.climate_ir_with_receiver_schema(HaierIrAcYrh63Climate)

async def to_code(config):
    await climate_ir.new_climate_ir(config)
