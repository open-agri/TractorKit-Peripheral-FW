file(GLOB_RECURSE SOURCES main.c drivers/*/*.c)

idf_component_register(
                    SRCS ${SOURCES}
                    INCLUDE_DIRS ""
                    REQUIRES app_update bt)
