add_subdirectory(esp8266-cmake/main)
add_subdirectory(esp8266-cmake/arduino)

add_subdirectory(Olimex-OLED-128x64)

### Adafruit GFX library
file(GLOB ADAFRUIT_GFX_LIB_SRC
    ${CMAKE_CURRENT_LIST_DIR}/Adafruit-GFX-Library/*.c
    ${CMAKE_CURRENT_LIST_DIR}/Adafruit-GFX-Library/*.cpp
)

add_library(adafruit_gfx STATIC
    ${ADAFRUIT_GFX_LIB_SRC}
)

target_include_directories(adafruit_gfx PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/Adafruit-GFX-Library
    ${CMAKE_CURRENT_LIST_DIR}/Adafruit-GFX-Library/Fonts
)

target_link_libraries(adafruit_gfx PUBLIC
    arduino
    esp8266wire
)


### MQTT library
file(GLOB PUBSUBCLIENT_LIB_SRC
    ${CMAKE_CURRENT_LIST_DIR}/pubsubclient/src/*.c
    ${CMAKE_CURRENT_LIST_DIR}/pubsubclient/src/*.cpp
)

add_library(pubsubclient STATIC
    ${PUBSUBCLIENT_LIB_SRC}
)

target_include_directories(pubsubclient PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/pubsubclient/src
)

target_link_libraries(pubsubclient PUBLIC
    arduino
    esp8266wifi
)

