cmake_minimum_required(VERSION VERSION 3.10.2)

set(zc_src "")
set(zr_src "")
set(zed_src "")
set(zc_exe "")
set(zr_exe "")
set(zed_exe "")

set(app_proj "examples/ias_zone_sensor")
project(${app_proj})
message("Project: ${app_proj}")

set(zed_exe "ias_zone_sensor_zed")
list(APPEND zed_src ${app_proj}/izs_avg_val.c ${app_proj}/izs_battery.c ${app_proj}/izs_device.c ${app_proj}/izs_device_utils.c ${app_proj}/izs_ota_osif.c ${app_proj}/izs_nvram.c ${app_proj}/izs_start.c  ${app_proj}/izs_hal_stub.c )
option(ZED_EXE "Build ZED" ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/zigbee_rcp_app.cmake)
