import esphome.codegen as cg
from esphome.components import light
import esphome.config_validation as cv
from esphome.const import CONF_NUM_LEDS, CONF_OUTPUT_ID, CONF_PIN

from esphome.components.seesaw import CONF_SEESAW_ID, SeesawComponent, seesaw_ns

DEPENDENCIES = ["seesaw"]

CONF_COLOR_ORDER = "color_order"

seesaw_neopixel_ns = seesaw_ns.namespace("neopixel")
SeesawNeopixelLight = seesaw_neopixel_ns.class_("SeesawNeopixelLight", light.AddressableLight)

ColorOrder = seesaw_neopixel_ns.enum("ColorOrder")
COLOR_ORDERS = {
    "RGB": ColorOrder.COLOR_ORDER_RGB,
    "RBG": ColorOrder.COLOR_ORDER_RBG,
    "GRB": ColorOrder.COLOR_ORDER_GRB,
    "GBR": ColorOrder.COLOR_ORDER_GBR,
    "BRG": ColorOrder.COLOR_ORDER_BRG,
    "BGR": ColorOrder.COLOR_ORDER_BGR,
}

CONFIG_SCHEMA = light.ADDRESSABLE_LIGHT_SCHEMA.extend(
    {
        cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(SeesawNeopixelLight),
        cv.GenerateID(CONF_SEESAW_ID): cv.use_id(SeesawComponent),
        cv.Required(CONF_PIN): cv.int_range(min=0, max=20),
        cv.Required(CONF_NUM_LEDS): cv.positive_not_null_int,
        cv.Optional(CONF_COLOR_ORDER, default="GRB"): cv.enum(COLOR_ORDERS, upper=True),
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_OUTPUT_ID])
    await light.register_light(var, config)
    await cg.register_component(var, config)

    parent = await cg.get_variable(config[CONF_SEESAW_ID])
    cg.add(var.set_parent(parent))
    cg.add(var.set_pin(config[CONF_PIN]))
    cg.add(var.set_num_leds(config[CONF_NUM_LEDS]))
    cg.add(var.set_color_order(config[CONF_COLOR_ORDER]))
