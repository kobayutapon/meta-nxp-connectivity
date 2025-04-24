cmake_minimum_required(VERSION VERSION 3.10.2)

set(zc_src "")
set(zr_src "")
set(zed_src "")
set(zc_exe "")
set(zr_exe "")
set(zed_exe "")

set(app_proj "examples/level_control_sample")
project(${app_proj})
message("Project: ${app_proj}")

set(zc_exe "level_control_zc")
list(APPEND zc_src ${app_proj}/level_control_zc.c)
option(ZC_EXE "Build ZC" ON)

set(zed_exe "level_control_zed_auto_mode")
list(APPEND zed_src ${app_proj}/level_control_zed_auto_mode.c)
option(ZED_EXE "Build ZED" ON)

#set(zed_exe "level_control_zed_auto_mode")
#list(APPEND zed_src ${app_proj}/level_control_zed_compatibity_mode.c)
#option(ZED_EXE "Build ZED" ON)

#set(zed_exe "level_control_zed_auto_mode")
#list(APPEND zed_src ${app_proj}/level_control_zed_legacy_mode.c)
#option(ZED_EXE "Build ZED" ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/zigbee_rcp_app.cmake)
