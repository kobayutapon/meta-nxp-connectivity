cmake_minimum_required(VERSION VERSION 3.10.2)

set(zc_src "")
set(zr_src "")
set(zed_src "")
set(zc_exe "")
set(zr_exe "")
set(zed_exe "")

set(app_proj "examples/r23_new_api")
project(${app_proj})
message("Project: ${app_proj}")

set(zc_exe "r23_zc")
list(APPEND zc_src ${app_proj}/r23_zc.c)
option(ZC_EXE "Build ZC" ON)

set(zr_exe "r23_zr")
list(APPEND zr_src ${app_proj}/r23_zr.c)
option(ZR_EXE "Build ZR" ON)

set(zed_exe "r23_zed")
list(APPEND zed_src ${app_proj}/r23_zed.c)
option(ZED_EXE "Build ZED" ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/zigbee_rcp_app.cmake)

set(zc_src "")
set(zr_src "")
set(zed_src "")
set(zc_exe "")
set(zr_exe "")
set(zed_exe "")
set(zr_exe "r22_zr")
list(APPEND zr_src ${app_proj}/r22_zr.c)
option(ZR_EXE "Build ZR" ON)
include(${CMAKE_CURRENT_SOURCE_DIR}/zigbee_rcp_app.cmake)
