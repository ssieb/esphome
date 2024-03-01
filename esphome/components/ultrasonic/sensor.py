import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import sensor
from esphome.const import (
    CONF_ECHO_PIN,
    CONF_TRIGGER_PIN,
    CONF_TIMEOUT,
    STATE_CLASS_MEASUREMENT,
    UNIT_METER,
    ICON_ARROW_EXPAND_VERTICAL,
)

CONF_PULSE_TIME = "pulse_time"
CONF_SIGNAL_PIN = "signal_pin"

ultrasonic_ns = cg.esphome_ns.namespace("ultrasonic")
UltrasonicSensorComponent = ultrasonic_ns.class_(
    "UltrasonicSensorComponent", sensor.Sensor, cg.PollingComponent
)

CONFIG_SCHEMA = (
    sensor.sensor_schema(
        UltrasonicSensorComponent,
        unit_of_measurement=UNIT_METER,
        icon=ICON_ARROW_EXPAND_VERTICAL,
        accuracy_decimals=2,
        state_class=STATE_CLASS_MEASUREMENT,
    )
    .extend(
        {
            cv.Optional(CONF_TRIGGER_PIN): pins.gpio_output_pin_schema,
            cv.Optional(CONF_ECHO_PIN): pins.internal_gpio_input_pin_schema,
            cv.Optional(CONF_SIGNAL_PIN): pins.internal_gpio_output_pin_schema,
            cv.Optional(CONF_TIMEOUT, default="2m"): cv.distance,
            cv.Optional(
                CONF_PULSE_TIME, default="10us"
            ): cv.positive_time_period_microseconds,
        }
    )
    .extend(cv.polling_component_schema("60s"))
)


async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)

    if CONF_SIGNAL_PIN in config:
        signal = await cg.gpio_pin_expression(config[CONF_SIGNAL_PIN])
        cg.add(var.set_signal_pin(signal))
    else:
        trigger = await cg.gpio_pin_expression(config[CONF_TRIGGER_PIN])
        cg.add(var.set_trigger_pin(trigger))
        echo = await cg.gpio_pin_expression(config[CONF_ECHO_PIN])
        cg.add(var.set_echo_pin(echo))

    cg.add(var.set_timeout_us(config[CONF_TIMEOUT] / (0.000343 / 2)))
    cg.add(var.set_pulse_time_us(config[CONF_PULSE_TIME]))
