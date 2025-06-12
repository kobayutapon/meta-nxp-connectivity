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

set(zc_exe "cli_nxp_zczr")
list(APPEND zc_src ${app_proj}/cli_main.c ${app_proj}/cli_menu_console.c ${app_proj}/cli_config.c ${app_proj}/cli_network.c ${app_proj}/cli_endpoint.c ${app_proj}/cli_cluster.c ${app_proj}/cli_zdo.c ${app_proj}/cli_installcode.c ${app_proj}/cli_nvram.c ${app_proj}/cli_tools.c)
option(ZC_EXE "Build ZC" ON)

set(zed_exe "cli_nxp_zed")
list(APPEND zed_src ${app_proj}/cli_main.c ${app_proj}/cli_menu_console.c ${app_proj}/cli_config.c ${app_proj}/cli_network.c ${app_proj}/cli_endpoint.c ${app_proj}/cli_cluster.c ${app_proj}/cli_zdo.c ${app_proj}/cli_installcode.c ${app_proj}/cli_nvram.c ${app_proj}/cli_tools.c)
option(ZED_EXE "Build ZED" ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/zigbee_rcp_app.cmake)

set(zc_src "")
set(zr_src "")
set(zed_src "")
set(zc_exe "")
set(zr_exe "")
set(zed_exe "")

set(zc_exe "zb_daemon_zczr")
list(APPEND zc_src ${app_proj}/cli_main.c ${app_proj}/cli_menu_socket.c ${app_proj}/cli_config.c ${app_proj}/cli_network.c ${app_proj}/cli_endpoint.c ${app_proj}/cli_cluster.c ${app_proj}/cli_zdo.c ${app_proj}/cli_installcode.c ${app_proj}/cli_nvram.c ${app_proj}/cli_tools.c)
option(ZC_EXE "Build ZC" ON)

set(zed_exe "zb_daemon_zed")
list(APPEND zed_src ${app_proj}/cli_main.c ${app_proj}/cli_menu_socket.c ${app_proj}/cli_config.c ${app_proj}/cli_network.c ${app_proj}/cli_endpoint.c ${app_proj}/cli_cluster.c ${app_proj}/cli_zdo.c ${app_proj}/cli_installcode.c ${app_proj}/cli_nvram.c ${app_proj}/cli_tools.c)
option(ZED_EXE "Build ZED" ON)

set(zr_exe "zb-ctl")
list(APPEND zr_src ${app_proj}/zb-ctl.c)
option(ZR_EXE "Build ZR" ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/zigbee_rcp_app.cmake)
