cmake_minimum_required(VERSION VERSION 3.10.2)

set(zc_src "")
set(zr_src "")
set(zed_src "")

set(app_proj "examples/gp_proxy")
project(${app_proj})
message("Project: ${app_proj}")

set(zr_exe "gp_proxy_zr")
list(APPEND zr_src ${app_proj}/gp_proxy.c)
option(ZR_EXE "Build ZR" ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/zigbee_rcp_app.cmake)
