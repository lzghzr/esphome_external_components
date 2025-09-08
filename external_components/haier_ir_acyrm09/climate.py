import esphome.codegen as cg
from esphome.components import climate_ir

AUTO_LOAD = ["climate_ir"]

haier_ir_acyrm09_ns = cg.esphome_ns.namespace("haier_ir_acyrm09")
HaierIrAcYrm09Climate = haier_ir_acyrm09_ns.class_("HaierIrAcYrm09", climate_ir.ClimateIR)

CONF_HAIER_ID = "haier_acyrm09_id"

CONFIG_SCHEMA = climate_ir.climate_ir_with_receiver_schema(HaierIrAcYrm09Climate)

async def to_code(config):
    await climate_ir.new_climate_ir(config)
