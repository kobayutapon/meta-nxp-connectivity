cmake_minimum_required(VERSION VERSION 3.10.2)

set(zc_src "")
set(zr_src "")
set(zed_src "")
set(zc_exe "")
set(zr_exe "")
set(zed_exe "")

set(app_proj "examples/cli_nxp")
project(${app_proj})
message("Project: ${app_proj}")

set(zc_exe "cli_nxp_zc")
list(APPEND zc_src ${app_proj}/cli_main.c ${app_proj}/cli_menu.c ${app_proj}/cli_config.c ${app_proj}/cli_network.c ${app_proj}/cli_endpoint.c ${app_proj}/cli_cluster.c ${app_proj}/cli_zdo.c ${app_proj}/cli_installcode.c ${app_proj}/cli_nvram.c ${app_proj}/cli_tools)
option(ZC_EXE "Build ZC" ON)

set(zed_exe "cli_nxp_zed")
list(APPEND zed_src ${app_proj}/cli_main.c ${app_proj}/cli_menu.c ${app_proj}/cli_config.c ${app_proj}/cli_network.c ${app_proj}/cli_endpoint.c ${app_proj}/cli_cluster.c ${app_proj}/cli_zdo.c ${app_proj}/cli_installcode.c ${app_proj}/cli_nvram.c ${app_proj}/cli_tools)
option(ZED_EXE "Build ZED" ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/zigbee_rcp_app.cmake)
