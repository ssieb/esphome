import esphome.codegen as cg
from esphome.components import i2c, output
import esphome.config_validation as cv
from esphome.const import CONF_CHANNEL, CONF_ID

from .. import CONF_LP5562_ID, LP5562, lp5562_ns

DEPENDENCIES = ["lp5562"]

LP5562Output = lp5562_ns.class_(
    "LP5562Output", cg.Component, i2c.I2CDevice, output.FloatOutput
)

CONFIG_SCHEMA = output.FLOAT_OUTPUT_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(LP5562Output),
        cv.GenerateID(CONF_LP5562_ID): cv.use_id(LP5562),
        cv.Required(CONF_CHANNEL): cv.int_range(min=0, max=3),
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await output.register_output(var, config)

    await cg.register_parented(var, config[CONF_LP5562_ID])

    cg.add(var.set_channel(config[CONF_CHANNEL]))
