import esphome.codegen as cg

CODEOWNERS = ["@ssieb"]

storage_ns = cg.esphome_ns.namespace("storage")
FileProvider = storage_ns.class_("FileProvider")
RawStorage = storage_ns.class_("RawStorage")
