cmake_minimum_required(VERSION VERSION 3.10.2)

set(zc_src "")
set(zr_src "")
set(zed_src "")
set(zc_exe "")
set(zr_exe "")
set(zed_exe "")

set(app_proj "examples/simple_gw")
project(${app_proj})
message("Project: ${app_proj}")

set(zc_exe "simple_gw_zc")
list(APPEND zc_src ${app_proj}/simple_gw.c)
list(APPEND zc_src ${app_proj}/ias_cie_addon.c)
option(ZC_EXE "Build ZC" ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/zigbee_rcp_app.cmake)
