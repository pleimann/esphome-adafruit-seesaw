import esphome.codegen as cg
from esphome import automation
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_TRIGGER_ID

from .. import CONF_SEESAW_ID, SeesawComponent, seesaw_ns

DEPENDENCIES = ["seesaw"]

CONF_ON_KEY_EVENT = "on_key_event"

seesaw_keypad_ns = seesaw_ns.namespace("keypad")
SeesawKeypad = seesaw_keypad_ns.class_("SeesawKeypad", cg.PollingComponent)
KeyEvent = seesaw_keypad_ns.struct("KeyEvent")
KeyEventTrigger = seesaw_keypad_ns.class_(
    "KeyEventTrigger", automation.Trigger.template(KeyEvent)
)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(SeesawKeypad),
            cv.GenerateID(CONF_SEESAW_ID): cv.use_id(SeesawComponent),
            cv.Optional(CONF_ON_KEY_EVENT): automation.validate_automation(
                {
                    cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(KeyEventTrigger),
                }
            ),
        }
    )
    .extend(cv.polling_component_schema("10ms"))
    .extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    parent = await cg.get_variable(config[CONF_SEESAW_ID])
    cg.add(var.set_parent(parent))

    for conf in config.get(CONF_ON_KEY_EVENT, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [(KeyEvent, "x")], conf)
