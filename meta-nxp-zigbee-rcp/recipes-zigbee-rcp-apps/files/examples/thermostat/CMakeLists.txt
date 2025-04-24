cmake_minimum_required(VERSION VERSION 3.10.2)

set(zc_src "")
set(zr_src "")
set(zed_src "")
set(zc_exe "")
set(zr_exe "")
set(zed_exe "")

set(app_proj "examples/thermostat")
project(${app_proj})
message("Project: ${app_proj}")

set(zc_exe "thermostat_zc")
list(APPEND zc_src ${app_proj}/thermostat_zc.c)
option(ZC_EXE "Build ZC" ON)

set(zr_exe "thermostat_zr")
list(APPEND zr_src ${app_proj}/thermostat_zr.c)
option(ZR_EXE "Build ZR" ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/zigbee_rcp_app.cmake)
