import esphome.codegen as cg
from esphome.components import output
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_PIN

from .. import CONF_SEESAW_ID, SeesawComponent, seesaw_ns

DEPENDENCIES = ["seesaw"]

SeesawOutput = seesaw_ns.class_("SeesawOutput", output.BinaryOutput, cg.Component)

CONFIG_SCHEMA = output.BINARY_OUTPUT_SCHEMA.extend(
    {
        cv.Required(CONF_ID): cv.declare_id(SeesawOutput),
        cv.GenerateID(CONF_SEESAW_ID): cv.use_id(SeesawComponent),
        cv.Required(CONF_PIN): cv.int_range(min=0, max=20),
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await output.register_output(var, config)

    parent = await cg.get_variable(config[CONF_SEESAW_ID])
    cg.add(var.set_parent(parent))
    cg.add(var.set_pin(config[CONF_PIN]))
