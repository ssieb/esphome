import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components import climate, sensor
from esphome.const import CONF_AUTO_MODE, CONF_AWAY_CONFIG, CONF_COOL_ACTION, CONF_COOL_MODE, \
    CONF_DEFAULT_TARGET_TEMPERATURE, CONF_DEFAULT_TARGET_TEMPERATURE_HIGH, \
    CONF_DEFAULT_TARGET_TEMPERATURE_LOW, CONF_DRY_ACTION, CONF_DRY_MODE, \
    CONF_DUAL_TARGET_TEMPERATURE, CONF_FAN_MODE_ON_ACTION, CONF_FAN_MODE_OFF_ACTION, \
    CONF_FAN_MODE_AUTO_ACTION, CONF_FAN_MODE_LOW_ACTION, CONF_FAN_MODE_MEDIUM_ACTION, \
    CONF_FAN_MODE_HIGH_ACTION, CONF_FAN_MODE_MIDDLE_ACTION, CONF_FAN_MODE_FOCUS_ACTION, \
    CONF_FAN_MODE_DIFFUSE_ACTION, CONF_FAN_ONLY_ACTION, CONF_FAN_ONLY_MODE, CONF_HEAT_ACTION, \
    CONF_HEAT_MODE, CONF_HYSTERESIS, CONF_ID, CONF_IDLE_ACTION, CONF_OFF_MODE, CONF_SENSOR, \
    CONF_SINGLE_TARGET_TEMPERATURE, CONF_SWING_BOTH_ACTION, CONF_SWING_HORIZONTAL_ACTION, \
    CONF_SWING_OFF_ACTION, CONF_SWING_VERTICAL_ACTION

bang_bang_ns = cg.esphome_ns.namespace('bang_bang')
BangBangClimate = bang_bang_ns.class_('BangBangClimate', climate.Climate, cg.Component)
BangBangClimateTargetTempConfig = bang_bang_ns.struct('BangBangClimateTargetTempConfig')

CONFIG_SCHEMA = cv.All(climate.CLIMATE_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(BangBangClimate),
    cv.Required(CONF_SENSOR): cv.use_id(sensor.Sensor),
    cv.Exclusive(CONF_DUAL_TARGET_TEMPERATURE, 'type'): cv.Schema({
        cv.Required(CONF_IDLE_ACTION): automation.validate_automation(single=True),
        cv.Required(CONF_COOL_ACTION,): automation.validate_automation(single=True),
        cv.Optional(CONF_DRY_ACTION): automation.validate_automation(single=True),
        cv.Optional(CONF_FAN_ONLY_ACTION): automation.validate_automation(single=True),
        cv.Required(CONF_HEAT_ACTION): automation.validate_automation(single=True),
        cv.Optional(CONF_AUTO_MODE): automation.validate_automation(single=True),
        cv.Optional(CONF_COOL_MODE): automation.validate_automation(single=True),
        cv.Optional(CONF_DRY_MODE): automation.validate_automation(single=True),
        cv.Optional(CONF_FAN_ONLY_MODE): automation.validate_automation(single=True),
        cv.Optional(CONF_HEAT_MODE): automation.validate_automation(single=True),
        cv.Optional(CONF_OFF_MODE): automation.validate_automation(single=True),
        cv.Optional(CONF_FAN_MODE_ON_ACTION): automation.validate_automation(single=True),
        cv.Optional(CONF_FAN_MODE_OFF_ACTION): automation.validate_automation(single=True),
        cv.Optional(CONF_FAN_MODE_AUTO_ACTION): automation.validate_automation(single=True),
        cv.Optional(CONF_FAN_MODE_LOW_ACTION): automation.validate_automation(single=True),
        cv.Optional(CONF_FAN_MODE_MEDIUM_ACTION): automation.validate_automation(single=True),
        cv.Optional(CONF_FAN_MODE_HIGH_ACTION): automation.validate_automation(single=True),
        cv.Optional(CONF_FAN_MODE_MIDDLE_ACTION): automation.validate_automation(single=True),
        cv.Optional(CONF_FAN_MODE_FOCUS_ACTION): automation.validate_automation(single=True),
        cv.Optional(CONF_FAN_MODE_DIFFUSE_ACTION): automation.validate_automation(single=True),
        cv.Optional(CONF_SWING_BOTH_ACTION): automation.validate_automation(single=True),
        cv.Optional(CONF_SWING_HORIZONTAL_ACTION): automation.validate_automation(single=True),
        cv.Optional(CONF_SWING_OFF_ACTION): automation.validate_automation(single=True),
        cv.Optional(CONF_SWING_VERTICAL_ACTION): automation.validate_automation(single=True),
        cv.Optional(CONF_DEFAULT_TARGET_TEMPERATURE_HIGH): cv.temperature,
        cv.Optional(CONF_DEFAULT_TARGET_TEMPERATURE_LOW): cv.temperature,
        cv.Optional(CONF_HYSTERESIS, default=0.5): cv.temperature,
        cv.Optional(CONF_AWAY_CONFIG): cv.Schema({
            cv.Required(CONF_DEFAULT_TARGET_TEMPERATURE_HIGH): cv.temperature,
            cv.Required(CONF_DEFAULT_TARGET_TEMPERATURE_LOW): cv.temperature,
            cv.Optional(CONF_HYSTERESIS, default=0.5): cv.temperature,
        }),
    }),
    cv.Exclusive(CONF_SINGLE_TARGET_TEMPERATURE, 'type'): cv.Schema({
        cv.Required(CONF_IDLE_ACTION): automation.validate_automation(single=True),
        cv.Exclusive(CONF_COOL_ACTION, 'action',): automation.validate_automation(single=True),
        cv.Exclusive(CONF_DRY_ACTION, 'action'): automation.validate_automation(single=True),
        cv.Exclusive(CONF_FAN_ONLY_ACTION, 'action'): automation.validate_automation(single=True),
        cv.Exclusive(CONF_HEAT_ACTION, 'action'): automation.validate_automation(single=True),
        cv.Exclusive(CONF_COOL_MODE, 'mode'): automation.validate_automation(single=True),
        cv.Exclusive(CONF_DRY_MODE, 'mode'): automation.validate_automation(single=True),
        cv.Exclusive(CONF_FAN_ONLY_MODE, 'mode'): automation.validate_automation(single=True),
        cv.Exclusive(CONF_HEAT_MODE, 'mode'): automation.validate_automation(single=True),
        cv.Optional(CONF_OFF_MODE): automation.validate_automation(single=True),
        cv.Optional(CONF_FAN_MODE_ON_ACTION): automation.validate_automation(single=True),
        cv.Optional(CONF_FAN_MODE_OFF_ACTION): automation.validate_automation(single=True),
        cv.Optional(CONF_FAN_MODE_AUTO_ACTION): automation.validate_automation(single=True),
        cv.Optional(CONF_FAN_MODE_LOW_ACTION): automation.validate_automation(single=True),
        cv.Optional(CONF_FAN_MODE_MEDIUM_ACTION): automation.validate_automation(single=True),
        cv.Optional(CONF_FAN_MODE_HIGH_ACTION): automation.validate_automation(single=True),
        cv.Optional(CONF_FAN_MODE_MIDDLE_ACTION): automation.validate_automation(single=True),
        cv.Optional(CONF_FAN_MODE_FOCUS_ACTION): automation.validate_automation(single=True),
        cv.Optional(CONF_FAN_MODE_DIFFUSE_ACTION): automation.validate_automation(single=True),
        cv.Optional(CONF_SWING_BOTH_ACTION): automation.validate_automation(single=True),
        cv.Optional(CONF_SWING_HORIZONTAL_ACTION): automation.validate_automation(single=True),
        cv.Optional(CONF_SWING_OFF_ACTION): automation.validate_automation(single=True),
        cv.Optional(CONF_SWING_VERTICAL_ACTION): automation.validate_automation(single=True),
        cv.Optional(CONF_DEFAULT_TARGET_TEMPERATURE): cv.temperature,
        cv.Optional(CONF_HYSTERESIS, default=0.5): cv.temperature,
        cv.Optional(CONF_AWAY_CONFIG): cv.Schema({
            cv.Required(CONF_DEFAULT_TARGET_TEMPERATURE): cv.temperature,
            cv.Optional(CONF_HYSTERESIS, default=0.5): cv.temperature,
        }),
    }),
}).extend(cv.COMPONENT_SCHEMA), cv.has_at_least_one_key(CONF_DUAL_TARGET_TEMPERATURE,
                                                        CONF_SINGLE_TARGET_TEMPERATURE))


def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    yield climate.register_climate(var, config)

    sens = yield cg.get_variable(config[CONF_SENSOR])
    cg.add(var.set_sensor(sens))

    if CONF_DUAL_TARGET_TEMPERATURE in config:
        dual_config = config[CONF_DUAL_TARGET_TEMPERATURE]

        normal_config = BangBangClimateTargetTempConfig(
                        dual_config[CONF_DEFAULT_TARGET_TEMPERATURE_LOW],
                        dual_config[CONF_DEFAULT_TARGET_TEMPERATURE_HIGH],
                        dual_config[CONF_HYSTERESIS]
        )
        cg.add(var.set_normal_config(normal_config))

        yield automation.build_automation(var.get_idle_action_trigger(), [],
                                          dual_config[CONF_IDLE_ACTION])

        cg.add(var.set_supports_auto(True))

        if CONF_COOL_ACTION in dual_config:
            yield automation.build_automation(var.get_cool_action_trigger(), [],
                                              dual_config[CONF_COOL_ACTION])
            cg.add(var.set_supports_cool(True))
        if CONF_DRY_ACTION in dual_config:
            yield automation.build_automation(var.get_dry_action_trigger(), [],
                                              dual_config[CONF_DRY_ACTION])
            cg.add(var.set_supports_dry(True))
        if CONF_FAN_ONLY_ACTION in dual_config:
            yield automation.build_automation(var.get_fan_only_action_trigger(), [],
                                              dual_config[CONF_FAN_ONLY_ACTION])
            cg.add(var.set_supports_fan_only(True))
        if CONF_HEAT_ACTION in dual_config:
            yield automation.build_automation(var.get_heat_action_trigger(), [],
                                              dual_config[CONF_HEAT_ACTION])
            cg.add(var.set_supports_heat(True))
        if CONF_AUTO_MODE in dual_config:
            yield automation.build_automation(var.get_auto_mode_trigger(), [],
                                              dual_config[CONF_AUTO_MODE])
        if CONF_COOL_MODE in dual_config:
            yield automation.build_automation(var.get_cool_mode_trigger(), [],
                                              dual_config[CONF_COOL_MODE])
            cg.add(var.set_supports_cool(True))
        if CONF_DRY_MODE in dual_config:
            yield automation.build_automation(var.get_dry_mode_trigger(), [],
                                              dual_config[CONF_DRY_MODE])
            cg.add(var.set_supports_dry(True))
        if CONF_FAN_ONLY_MODE in dual_config:
            yield automation.build_automation(var.get_fan_only_mode_trigger(), [],
                                              dual_config[CONF_FAN_ONLY_MODE])
            cg.add(var.set_supports_fan_only(True))
        if CONF_HEAT_MODE in dual_config:
            yield automation.build_automation(var.get_heat_mode_trigger(), [],
                                              dual_config[CONF_HEAT_MODE])
            cg.add(var.set_supports_heat(True))
        if CONF_OFF_MODE in dual_config:
            yield automation.build_automation(var.get_off_mode_trigger(), [],
                                              dual_config[CONF_OFF_MODE])
        if CONF_FAN_MODE_ON_ACTION in dual_config:
            yield automation.build_automation(var.get_fan_mode_on_trigger(), [],
                                              dual_config[CONF_FAN_MODE_ON_ACTION])
            cg.add(var.set_supports_fan_mode_on(True))
        if CONF_FAN_MODE_OFF_ACTION in dual_config:
            yield automation.build_automation(var.get_fan_mode_off_trigger(), [],
                                              dual_config[CONF_FAN_MODE_OFF_ACTION])
            cg.add(var.set_supports_fan_mode_off(True))
        if CONF_FAN_MODE_AUTO_ACTION in dual_config:
            yield automation.build_automation(var.get_fan_mode_auto_trigger(), [],
                                              dual_config[CONF_FAN_MODE_AUTO_ACTION])
            cg.add(var.set_supports_fan_mode_auto(True))
        if CONF_FAN_MODE_LOW_ACTION in dual_config:
            yield automation.build_automation(var.get_fan_mode_low_trigger(), [],
                                              dual_config[CONF_FAN_MODE_LOW_ACTION])
            cg.add(var.set_supports_fan_mode_low(True))
        if CONF_FAN_MODE_MEDIUM_ACTION in dual_config:
            yield automation.build_automation(var.get_fan_mode_medium_trigger(), [],
                                              dual_config[CONF_FAN_MODE_MEDIUM_ACTION])
            cg.add(var.set_supports_fan_mode_medium(True))
        if CONF_FAN_MODE_HIGH_ACTION in dual_config:
            yield automation.build_automation(var.get_fan_mode_high_trigger(), [],
                                              dual_config[CONF_FAN_MODE_HIGH_ACTION])
            cg.add(var.set_supports_fan_mode_high(True))
        if CONF_FAN_MODE_MIDDLE_ACTION in dual_config:
            yield automation.build_automation(var.get_fan_mode_middle_trigger(), [],
                                              dual_config[CONF_FAN_MODE_MIDDLE_ACTION])
            cg.add(var.set_supports_fan_mode_middle(True))
        if CONF_FAN_MODE_FOCUS_ACTION in dual_config:
            yield automation.build_automation(var.get_fan_mode_focus_trigger(), [],
                                              dual_config[CONF_FAN_MODE_FOCUS_ACTION])
            cg.add(var.set_supports_fan_mode_focus(True))
        if CONF_FAN_MODE_DIFFUSE_ACTION in dual_config:
            yield automation.build_automation(var.get_fan_mode_diffuse_trigger(), [],
                                              dual_config[CONF_FAN_MODE_DIFFUSE_ACTION])
            cg.add(var.set_supports_fan_mode_diffuse(True))
        if CONF_SWING_BOTH_ACTION in dual_config:
            yield automation.build_automation(var.get_swing_mode_both_trigger(), [],
                                              dual_config[CONF_SWING_BOTH_ACTION])
            cg.add(var.set_supports_swing_mode_both(True))
        if CONF_SWING_HORIZONTAL_ACTION in dual_config:
            yield automation.build_automation(var.get_swing_mode_horizontal_trigger(), [],
                                              dual_config[CONF_SWING_HORIZONTAL_ACTION])
            cg.add(var.set_supports_swing_mode_horizontal(True))
        if CONF_SWING_OFF_ACTION in dual_config:
            yield automation.build_automation(var.get_swing_mode_off_trigger(), [],
                                              dual_config[CONF_SWING_OFF_ACTION])
            cg.add(var.set_supports_swing_mode_off(True))
        if CONF_SWING_VERTICAL_ACTION in dual_config:
            yield automation.build_automation(var.get_swing_mode_vertical_trigger(), [],
                                              dual_config[CONF_SWING_VERTICAL_ACTION])
            cg.add(var.set_supports_swing_mode_vertical(True))

        if CONF_AWAY_CONFIG in dual_config:
            away = dual_config[CONF_AWAY_CONFIG]

            away_config = BangBangClimateTargetTempConfig(
                away[CONF_DEFAULT_TARGET_TEMPERATURE_LOW],
                away[CONF_DEFAULT_TARGET_TEMPERATURE_HIGH],
                away[CONF_HYSTERESIS]
            )
            cg.add(var.set_away_config(away_config))
    elif CONF_SINGLE_TARGET_TEMPERATURE in config:
        single_config = config[CONF_SINGLE_TARGET_TEMPERATURE]

        normal_config = BangBangClimateTargetTempConfig(
                        single_config[CONF_DEFAULT_TARGET_TEMPERATURE],
                        single_config[CONF_HYSTERESIS]
        )
        cg.add(var.set_normal_config(normal_config))

        yield automation.build_automation(var.get_idle_action_trigger(), [],
                                          single_config[CONF_IDLE_ACTION])

        cg.add(var.set_supports_auto(False))

        if CONF_COOL_ACTION in single_config:
            yield automation.build_automation(var.get_cool_action_trigger(), [],
                                              single_config[CONF_COOL_ACTION])
            cg.add(var.set_supports_cool(True))
        if CONF_DRY_ACTION in single_config:
            yield automation.build_automation(var.get_dry_action_trigger(), [],
                                              single_config[CONF_DRY_ACTION])
            cg.add(var.set_supports_dry(True))
        if CONF_FAN_ONLY_ACTION in single_config:
            yield automation.build_automation(var.get_fan_only_action_trigger(), [],
                                              single_config[CONF_FAN_ONLY_ACTION])
            cg.add(var.set_supports_fan_only(True))
        if CONF_HEAT_ACTION in single_config:
            yield automation.build_automation(var.get_heat_action_trigger(), [],
                                              single_config[CONF_HEAT_ACTION])
            cg.add(var.set_supports_heat(True))
        if CONF_COOL_MODE in single_config:
            yield automation.build_automation(var.get_cool_mode_trigger(), [],
                                              single_config[CONF_COOL_MODE])
            cg.add(var.set_supports_cool(True))
        if CONF_DRY_MODE in single_config:
            yield automation.build_automation(var.get_dry_mode_trigger(), [],
                                              single_config[CONF_DRY_MODE])
            cg.add(var.set_supports_dry(True))
        if CONF_FAN_ONLY_MODE in single_config:
            yield automation.build_automation(var.get_fan_only_mode_trigger(), [],
                                              single_config[CONF_FAN_ONLY_MODE])
            cg.add(var.set_supports_fan_only(True))
        if CONF_HEAT_MODE in single_config:
            yield automation.build_automation(var.get_heat_mode_trigger(), [],
                                              single_config[CONF_HEAT_MODE])
            cg.add(var.set_supports_heat(True))
        if CONF_OFF_MODE in single_config:
            yield automation.build_automation(var.get_off_mode_trigger(), [],
                                              single_config[CONF_OFF_MODE])
        if CONF_FAN_MODE_ON_ACTION in single_config:
            yield automation.build_automation(var.get_fan_mode_on_trigger(), [],
                                              single_config[CONF_FAN_MODE_ON_ACTION])
            cg.add(var.set_supports_fan_mode_on(True))
        if CONF_FAN_MODE_OFF_ACTION in single_config:
            yield automation.build_automation(var.get_fan_mode_off_trigger(), [],
                                              single_config[CONF_FAN_MODE_OFF_ACTION])
            cg.add(var.set_supports_fan_mode_off(True))
        if CONF_FAN_MODE_AUTO_ACTION in single_config:
            yield automation.build_automation(var.get_fan_mode_auto_trigger(), [],
                                              single_config[CONF_FAN_MODE_AUTO_ACTION])
            cg.add(var.set_supports_fan_mode_auto(True))
        if CONF_FAN_MODE_LOW_ACTION in single_config:
            yield automation.build_automation(var.get_fan_mode_low_trigger(), [],
                                              single_config[CONF_FAN_MODE_LOW_ACTION])
            cg.add(var.set_supports_fan_mode_low(True))
        if CONF_FAN_MODE_MEDIUM_ACTION in single_config:
            yield automation.build_automation(var.get_fan_mode_medium_trigger(), [],
                                              single_config[CONF_FAN_MODE_MEDIUM_ACTION])
            cg.add(var.set_supports_fan_mode_medium(True))
        if CONF_FAN_MODE_HIGH_ACTION in single_config:
            yield automation.build_automation(var.get_fan_mode_high_trigger(), [],
                                              single_config[CONF_FAN_MODE_HIGH_ACTION])
            cg.add(var.set_supports_fan_mode_high(True))
        if CONF_FAN_MODE_MIDDLE_ACTION in single_config:
            yield automation.build_automation(var.get_fan_mode_middle_trigger(), [],
                                              single_config[CONF_FAN_MODE_MIDDLE_ACTION])
            cg.add(var.set_supports_fan_mode_middle(True))
        if CONF_FAN_MODE_FOCUS_ACTION in single_config:
            yield automation.build_automation(var.get_fan_mode_focus_trigger(), [],
                                              single_config[CONF_FAN_MODE_FOCUS_ACTION])
            cg.add(var.set_supports_fan_mode_focus(True))
        if CONF_FAN_MODE_DIFFUSE_ACTION in single_config:
            yield automation.build_automation(var.get_fan_mode_diffuse_trigger(), [],
                                              single_config[CONF_FAN_MODE_DIFFUSE_ACTION])
            cg.add(var.set_supports_fan_mode_diffuse(True))
        if CONF_SWING_BOTH_ACTION in single_config:
            yield automation.build_automation(var.get_swing_mode_both_trigger(), [],
                                              single_config[CONF_SWING_BOTH_ACTION])
            cg.add(var.set_supports_swing_mode_both(True))
        if CONF_SWING_HORIZONTAL_ACTION in single_config:
            yield automation.build_automation(var.get_swing_mode_horizontal_trigger(), [],
                                              single_config[CONF_SWING_HORIZONTAL_ACTION])
            cg.add(var.set_supports_swing_mode_horizontal(True))
        if CONF_SWING_OFF_ACTION in single_config:
            yield automation.build_automation(var.get_swing_mode_off_trigger(), [],
                                              single_config[CONF_SWING_OFF_ACTION])
            cg.add(var.set_supports_swing_mode_off(True))
        if CONF_SWING_VERTICAL_ACTION in single_config:
            yield automation.build_automation(var.get_swing_mode_vertical_trigger(), [],
                                              single_config[CONF_SWING_VERTICAL_ACTION])
            cg.add(var.set_supports_swing_mode_vertical(True))

        if CONF_AWAY_CONFIG in single_config:
            away = single_config[CONF_AWAY_CONFIG]

            away_config = BangBangClimateTargetTempConfig(
                away[CONF_DEFAULT_TARGET_TEMPERATURE],
                away[CONF_HYSTERESIS]
            )
            cg.add(var.set_away_config(away_config))
