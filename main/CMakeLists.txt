file(GLOB_RECURSE SOURCES main.c)

if(CONFIG_TK_ONBOARD_LED_ENABLE)
    file(GLOB_RECURSE SOURCES ${SOURCES} drivers/LED/led_onboard.c)
endif()


idf_component_register(
                    SRCS ${SOURCES}
                    INCLUDE_DIRS ""
                    REQUIRES app_update bt)
