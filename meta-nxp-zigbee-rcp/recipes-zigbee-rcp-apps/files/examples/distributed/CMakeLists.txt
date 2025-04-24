cmake_minimum_required(VERSION VERSION 3.10.2)

set(zc_src "")
set(zr_src "")
set(zed_src "")
set(zc_exe "")
set(zr_exe "")
set(zed_exe "")

set(app_proj "examples/distributed")
project(${app_proj})
message("Project: ${app_proj}")

set(zr_exe "on_off_distrib_output_zr")
list(APPEND zr_src ${app_proj}/on_off_distrib_output_zr.c)
option(ZR_EXE "Build ZR" ON)

set(zed_exe "on_off_distrib_switch_zed")
list(APPEND zed_src ${app_proj}/on_off_distrib_switch_zed.c)
option(ZED_EXE "Build ZED" ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/zigbee_rcp_app.cmake)

