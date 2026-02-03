import esphome.codegen as cg
from esphome.components import binary_sensor
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_PIN

from .. import CONF_SEESAW_ID, SeesawComponent, seesaw_ns

DEPENDENCIES = ["seesaw"]

CONF_PULLUP = "pullup"

SeesawBinarySensor = seesaw_ns.class_(
    "SeesawBinarySensor", binary_sensor.BinarySensor, cg.PollingComponent
)

CONFIG_SCHEMA = (
    binary_sensor.binary_sensor_schema(SeesawBinarySensor)
    .extend(
        {
            cv.GenerateID(CONF_SEESAW_ID): cv.use_id(SeesawComponent),
            cv.Required(CONF_PIN): cv.int_range(min=0, max=20),
            cv.Optional(CONF_PULLUP, default=True): cv.boolean,
        }
    )
    .extend(cv.polling_component_schema("60ms"))
)


async def to_code(config):
    var = await binary_sensor.new_binary_sensor(config)
    await cg.register_component(var, config)

    parent = await cg.get_variable(config[CONF_SEESAW_ID])
    cg.add(var.set_parent(parent))
    cg.add(var.set_pin(config[CONF_PIN]))
    cg.add(var.set_pullup(config[CONF_PULLUP]))
