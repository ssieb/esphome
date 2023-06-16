import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components import i2c, sensor
from esphome.const import (
    CONF_ID,
    # CONF_CO2,
    CONF_CONDUCTIVITY,
    CONF_EC,
    CONF_HUMIDITY,
    CONF_PH,
    CONF_SENSORS,
    CONF_TEMPERATURE,
    CONF_TRIGGER_ID,
    CONF_TYPE,
    DEVICE_CLASS_EMPTY,
    DEVICE_CLASS_HUMIDITY,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_VOLTAGE,
    ICON_EMPTY,
    ICON_FLASH,
    ICON_PERCENT,
    ICON_THERMOMETER,
    STATE_CLASS_MEASUREMENT,
    UNIT_CELSIUS,
    UNIT_EMPTY,
    UNIT_MICROSIEMENS_PER_CENTIMETER,
    UNIT_MILLIGRAMS_PER_CUBIC_METER,
    UNIT_PARTS_PER_MILLION,
    UNIT_PERCENT,
    UNIT_PH,
    UNIT_VOLT,
)

CODEOWNERS = ["@ssieb"]

DEPENDENCIES = ["i2c"]

CONF_SINGLE = "single"
CONF_MULTI = "multi"
CONF_DO = "do"
CONF_FLO = "flo"
CONF_HUM = "hum"
CONF_MG = "mg"
CONF_O2 = "o2"
CONF_ORP = "orp"
CONF_PRS = "prs"
CONF_RGB = "rgb"
CONF_RTD = "rtd"

CONF_DEWPOINT = "dewpoint"
CONF_PERCENT = "percent"
CONF_SALINITY = "salinity"
CONF_SPECIFIC_GRAVITY = "specific_gravity"
CONF_TOTAL_DISSOLVED_SOLIDS = "total_dissolved_solids"

ICON_SIGMA = "mdi:sigma-lower"

CONF_ON_LED = "on_led"
CONF_ON_DEVICE_INFORMATION = "on_device_information"
CONF_ON_SLOPE = "on_slope"
CONF_ON_CALIBRATION = "on_calibration"
CONF_ON_T = "on_t"
CONF_ON_CUSTOM = "on_custom"

ezo_ns = cg.esphome_ns.namespace("ezo")

EZOSensor = ezo_ns.class_("EZOSensor", cg.PollingComponent, i2c.I2CDevice)
EZOSensorSingle = ezo_ns.class_("EZOSensorSingle", EZOSensor, sensor.Sensor)
EZOSensorMulti = ezo_ns.class_("EZOSensorMulti", EZOSensor)
EZOSensorDO = ezo_ns.class_("EZOSensorDO", EZOSensor)
EZOSensorEC = ezo_ns.class_("EZOSensorEC", EZOSensor)
EZOSensorFLO = ezo_ns.class_("EZOSensorFLO", EZOSensor)
EZOSensorRGB = ezo_ns.class_("EZOSensorRGB", EZOSensor)

CustomTrigger = ezo_ns.class_(
    "CustomTrigger", automation.Trigger.template(cg.std_string)
)


TTrigger = ezo_ns.class_("TTrigger", automation.Trigger.template(cg.std_string))

SlopeTrigger = ezo_ns.class_("SlopeTrigger", automation.Trigger.template(cg.std_string))

CalibrationTrigger = ezo_ns.class_(
    "CalibrationTrigger", automation.Trigger.template(cg.std_string)
)

DeviceInformationTrigger = ezo_ns.class_(
    "DeviceInformationTrigger", automation.Trigger.template(cg.std_string)
)

LedTrigger = ezo_ns.class_("LedTrigger", automation.Trigger.template(cg.bool_))

BASE_SCHEMA = cv.Schema(
    {
        cv.Optional(CONF_ON_CUSTOM): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(CustomTrigger),
            }
        ),
        cv.Optional(CONF_ON_CALIBRATION): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(CalibrationTrigger),
            }
        ),
        cv.Optional(CONF_ON_SLOPE): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(SlopeTrigger),
            }
        ),
        cv.Optional(CONF_ON_T): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(TTrigger),
            }
        ),
        cv.Optional(CONF_ON_DEVICE_INFORMATION): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(DeviceInformationTrigger),
            }
        ),
        cv.Optional(CONF_ON_LED): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(LedTrigger),
            }
        ),
    }
).extend(cv.polling_component_schema("60s"))

CONFIG_SCHEMA = cv.typed_schema(
    {
        CONF_SINGLE: (
            sensor.sensor_schema(EZOSensorSingle)
            .extend(BASE_SCHEMA)
            .extend(i2c.i2c_device_schema(None))
        ),
        CONF_MULTI: BASE_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(EZOSensorMulti),
                cv.Required(CONF_SENSORS): cv.ensure_list(sensor.sensor_schema()),
            }
        ).extend(i2c.i2c_device_schema(None)),
        CONF_DO: BASE_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(EZOSensorDO),
                cv.Optional(CONF_MG): sensor.sensor_schema(
                    accuracy_decimals=2,
                    device_class=DEVICE_CLASS_EMPTY,
                    icon=ICON_EMPTY,
                    state_class=STATE_CLASS_MEASUREMENT,
                    unit_of_measurement=UNIT_MILLIGRAMS_PER_CUBIC_METER,
                ),
                cv.Optional(CONF_PERCENT): sensor.sensor_schema(
                    accuracy_decimals=2,
                    device_class=DEVICE_CLASS_EMPTY,
                    icon=ICON_PERCENT,
                    state_class=STATE_CLASS_MEASUREMENT,
                    unit_of_measurement=UNIT_PERCENT,
                ),
            }
        ).extend(i2c.i2c_device_schema(97)),
        CONF_FLO: BASE_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(EZOSensorFLO),
                cv.Optional(CONF_MG): sensor.sensor_schema(
                    accuracy_decimals=2,
                    device_class=DEVICE_CLASS_EMPTY,
                    icon=ICON_EMPTY,
                    state_class=STATE_CLASS_MEASUREMENT,
                    unit_of_measurement=UNIT_MILLIGRAMS_PER_CUBIC_METER,
                ),
                cv.Optional(CONF_PERCENT): sensor.sensor_schema(
                    accuracy_decimals=2,
                    device_class=DEVICE_CLASS_EMPTY,
                    icon=ICON_PERCENT,
                    state_class=STATE_CLASS_MEASUREMENT,
                    unit_of_measurement=UNIT_PERCENT,
                ),
            }
        ).extend(i2c.i2c_device_schema(104)),
        CONF_EC: BASE_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(EZOSensorEC),
                cv.Optional(CONF_CONDUCTIVITY): sensor.sensor_schema(
                    accuracy_decimals=2,
                    device_class=DEVICE_CLASS_EMPTY,
                    icon=ICON_SIGMA,
                    state_class=STATE_CLASS_MEASUREMENT,
                    unit_of_measurement=UNIT_MICROSIEMENS_PER_CENTIMETER,
                ),
                cv.Optional(CONF_TOTAL_DISSOLVED_SOLIDS): sensor.sensor_schema(
                    accuracy_decimals=0,
                    device_class=DEVICE_CLASS_EMPTY,
                    icon=ICON_EMPTY,
                    state_class=STATE_CLASS_MEASUREMENT,
                    unit_of_measurement=UNIT_PARTS_PER_MILLION,
                ),
                cv.Optional(CONF_SALINITY): sensor.sensor_schema(
                    accuracy_decimals=2,
                    device_class=DEVICE_CLASS_EMPTY,
                    icon=ICON_EMPTY,
                    state_class=STATE_CLASS_MEASUREMENT,
                    unit_of_measurement=UNIT_EMPTY,
                ),
                cv.Optional(CONF_SPECIFIC_GRAVITY): sensor.sensor_schema(
                    accuracy_decimals=3,
                    device_class=DEVICE_CLASS_EMPTY,
                    icon=ICON_EMPTY,
                    state_class=STATE_CLASS_MEASUREMENT,
                    unit_of_measurement=UNIT_EMPTY,
                ),
            }
        ).extend(i2c.i2c_device_schema(100)),
        CONF_HUM: BASE_SCHEMA.extend(
            {
                cv.Optional(CONF_HUMIDITY): sensor.sensor_schema(
                    accuracy_decimals=1,
                    device_class=DEVICE_CLASS_HUMIDITY,
                    icon=ICON_EMPTY,
                    state_class=STATE_CLASS_MEASUREMENT,
                    unit_of_measurement=UNIT_PERCENT,
                ),
                cv.Optional(CONF_TEMPERATURE): sensor.sensor_schema(
                    accuracy_decimals=2,
                    device_class=DEVICE_CLASS_TEMPERATURE,
                    icon=ICON_THERMOMETER,
                    state_class=STATE_CLASS_MEASUREMENT,
                    unit_of_measurement=UNIT_CELSIUS,
                ),
                cv.Optional(CONF_DEWPOINT): sensor.sensor_schema(
                    accuracy_decimals=2,
                    device_class=DEVICE_CLASS_TEMPERATURE,
                    icon=ICON_THERMOMETER,
                    state_class=STATE_CLASS_MEASUREMENT,
                    unit_of_measurement=UNIT_CELSIUS,
                ),
            }
        ).extend(i2c.i2c_device_schema(111)),
        CONF_ORP: BASE_SCHEMA.extend(
            sensor.sensor_schema(
                EZOSensorSingle,
                accuracy_decimals=4,
                device_class=DEVICE_CLASS_VOLTAGE,
                icon=ICON_FLASH,
                state_class=STATE_CLASS_MEASUREMENT,
                unit_of_measurement=UNIT_VOLT,
            )
        ).extend(i2c.i2c_device_schema(98)),
        CONF_PH: BASE_SCHEMA.extend(
            sensor.sensor_schema(
                EZOSensorSingle,
                accuracy_decimals=3,
                device_class=DEVICE_CLASS_EMPTY,
                icon=ICON_EMPTY,
                state_class=STATE_CLASS_MEASUREMENT,
                unit_of_measurement=UNIT_PH,
            )
        ).extend(i2c.i2c_device_schema(99)),
        CONF_RTD: BASE_SCHEMA.extend(
            sensor.sensor_schema(
                EZOSensorSingle,
                accuracy_decimals=3,
                device_class=DEVICE_CLASS_TEMPERATURE,
                icon=ICON_THERMOMETER,
                state_class=STATE_CLASS_MEASUREMENT,
                unit_of_measurement=UNIT_CELSIUS,
            )
        ).extend(i2c.i2c_device_schema(102)),
    },
    default_type=CONF_SINGLE,
    lower=True,
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    ezo_type = config[CONF_TYPE]
    if ezo_type == CONF_SINGLE:
        await sensor.register_sensor(var, config)
    elif ezo_type == CONF_MULTI:
        sensors = [await sensor.new_sensor(conf) for conf in config[CONF_SENSORS]]
        cg.add(var.set_sensors(sensors))
    elif ezo_type == CONF_DO:
        if conf := config.get(CONF_MG):
            sens = await sensor.new_sensor(conf)
            cg.add(var.set_mg_sensor(sens))
        if conf := config.get(CONF_PERCENT):
            sens = await sensor.new_sensor(conf)
            cg.add(var.set_mg_sensor(sens))
    elif ezo_type == CONF_EC:
        if conf := config.get(CONF_CONDUCTIVITY):
            sens = await sensor.new_sensor(conf)
            cg.add(var.set_conductivity_sensor(sens))
        if conf := config.get(CONF_TOTAL_DISSOLVED_SOLIDS):
            sens = await sensor.new_sensor(conf)
            cg.add(var.set_tds_sensor(sens))
        if conf := config.get(CONF_SALINITY):
            sens = await sensor.new_sensor(conf)
            cg.add(var.set_salinity_sensor(sens))
        if conf := config.get(CONF_SPECIFIC_GRAVITY):
            sens = await sensor.new_sensor(conf)
            cg.add(var.set_specific_gravity_sensor(sens))

    for conf in config.get(CONF_ON_CUSTOM, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [(cg.std_string, "x")], conf)

    for conf in config.get(CONF_ON_LED, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [(bool, "x")], conf)

    for conf in config.get(CONF_ON_DEVICE_INFORMATION, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [(cg.std_string, "x")], conf)

    for conf in config.get(CONF_ON_SLOPE, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [(cg.std_string, "x")], conf)

    for conf in config.get(CONF_ON_CALIBRATION, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [(cg.std_string, "x")], conf)

    for conf in config.get(CONF_ON_T, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [(cg.std_string, "x")], conf)
