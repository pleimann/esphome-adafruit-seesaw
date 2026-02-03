import esphome.codegen as cg
from esphome.components import i2c
import esphome.config_validation as cv
from esphome.const import CONF_ID

CODEOWNERS = ["@pleimann"]
DEPENDENCIES = ["i2c"]
MULTI_CONF = True

CONF_SEESAW_ID = "seesaw_id"

seesaw_ns = cg.esphome_ns.namespace("seesaw")
SeesawComponent = seesaw_ns.class_("SeesawComponent", cg.Component, i2c.I2CDevice)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(SeesawComponent),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(i2c.i2c_device_schema(0x49))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)
