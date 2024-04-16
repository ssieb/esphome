from esphome import automation
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import CONF_ID, CONF_ADDRESS, CONF_TRIGGER_ID

CODEOWNERS = ["@s1lvi0"]
DEPENDENCIES = ["uart"]

CONF_BMS_DALY_ID = "bms_daly_id"
CONF_ON_FAILURE_STATUS = "on_failure_status"

daly_bms = cg.esphome_ns.namespace("daly_bms")
DalyBmsComponent = daly_bms.class_(
    "DalyBmsComponent", cg.PollingComponent, uart.UARTDevice
)

DalyOnFailureStatus = daly_bms.class_(
    "DalyOnFailureStatus",
    automation.Trigger.template(
        cg.std_vector.template(cg.uint8).operator("ref").operator("const")
    ),
)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(DalyBmsComponent),
            cv.Optional(CONF_ADDRESS, default=0x80): cv.positive_int,
            cv.Optional(CONF_ON_FAILURE_STATUS): automation.validate_automation(
                {
                    cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(DalyOnFailureStatus),
                }
            ),
        }
    )
    .extend(uart.UART_DEVICE_SCHEMA)
    .extend(cv.polling_component_schema("30s"))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
    cg.add(var.set_address(config[CONF_ADDRESS]))
    for conf in config.get(CONF_ON_FAILURE_STATUS, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(
            trigger,
            [(cg.std_vector.template(cg.uint8).operator("ref").operator("const"), "x")],
            conf,
        )
