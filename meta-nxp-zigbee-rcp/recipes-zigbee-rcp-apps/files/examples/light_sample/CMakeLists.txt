cmake_minimum_required(VERSION VERSION 3.10.2)

set(zc_src "")
set(zr_src "")
set(zed_src "")
set(zc_exe "")
set(zr_exe "")
set(zed_exe "")

set(app_proj "examples/light_sample")
project(${app_proj})
message("Project: ${app_proj}")

set(zc_exe "light_zc")
list(APPEND zc_src ${app_proj}/light_coordinator/light_zc.c ${app_proj}/light_coordinator/light_zc_hal.c)
option(ZC_EXE "Build ZC" ON)

set(zr_exe "bulb")
list(APPEND zr_src  ${app_proj}/dimmable_light/bulb.c ${app_proj}/dimmable_light/bulb_hal.c)
option(ZR_EXE "Build ZR" ON)

set(zed_exe "light_control")
list(APPEND zed_src ${app_proj}/light_control/light_control.c ${app_proj}/light_control/light_control_hal.c)
option(ZED_EXE "Build ZED" ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/zigbee_rcp_app.cmake)
