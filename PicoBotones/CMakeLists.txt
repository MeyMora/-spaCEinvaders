cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# == NO TOCAR ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()

set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)

set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)

if(EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# =================

set(PICO_BOARD pico_w)

include(pico_sdk_import.cmake)

project(PicoBotones C CXX ASM)

pico_sdk_init()

add_executable(PicoBotones
    PicoBotones.c
)

target_link_libraries(PicoBotones
    pico_stdlib
    pico_cyw43_arch_none
)

target_include_directories(PicoBotones PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

pico_enable_stdio_usb(PicoBotones 1)
pico_enable_stdio_uart(PicoBotones 0)

pico_set_program_name(PicoBotones "PicoBotones")
pico_set_program_version(PicoBotones "1.0")

pico_add_extra_outputs(PicoBotones)