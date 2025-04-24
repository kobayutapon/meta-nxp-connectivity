cmake_minimum_required(VERSION VERSION 3.10.2)

set(zc_src "")
set(zr_src "")
set(zed_src "")
set(zc_exe "")
set(zr_exe "")
set(zed_exe "")

set(app_proj "examples/manuf_specific_cmds_and_attrs")
project(${app_proj})
message("Project: ${app_proj}")

set(zc_exe "manuf_onoff_output_zc")
list(APPEND zc_src ${app_proj}/manuf_onoff_output_zc.c)
option(ZC_EXE "Build ZC" ON)

set(zed_exe "manuf_onoff_switch_zed")
list(APPEND zed_src ${app_proj}/manuf_onoff_switch_zed.c)
option(ZED_EXE "Build ZED" ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/zigbee_rcp_app.cmake)
