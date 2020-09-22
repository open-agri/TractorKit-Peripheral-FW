file(GLOB_RECURSE SOURCES main.c)

idf_component_register(
                    SRCS ${SOURCES}
                    INCLUDE_DIRS ""
                    REQUIRES bt)
