FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:${THISDIR}/files:"

SRC_URI += "file://ota.sh"
DEPENDS += "${@bb.utils.contains_any('MACHINE', "imx8mmevk-matter imx8mnddr3levk-matter imx8mnevk-matter imx8mpevk-matter imx91evk-iwxxx-matter imx93evk-iwxxx-matter ", ' zigbee-rcp-sdk ', '', d)}"
RDEPENDS:${PN} += "${@bb.utils.contains_any('MACHINE', "imx8mmevk-matter imx8mnddr3levk-matter imx8mnevk-matter imx8mpevk-matter imx91evk-iwxxx-matter imx93evk-iwxxx-matter ", ' zigbee-rcp-sdk ', '', d)}"


DEPLOY_TRUSTY = "${@bb.utils.contains('MACHINE_FEATURES', 'trusty', 'true', 'false', d)}"
BUILD_M2ZIGBEE = "${@bb.utils.contains_any('MACHINE', "imx8mmevk-matter imx8mnddr3levk-matter imx8mnevk-matter imx8mpevk-matter imx91evk-iwxxx-matter imx93evk-iwxxx-matter ",'true', 'false', d)}"

common_configure() {
    PKG_CONFIG_SYSROOT_DIR=${PKG_CONFIG_SYSROOT_DIR} \
    PKG_CONFIG_LIBDIR=${PKG_CONFIG_PATH} \
    gn gen out/aarch64 --script-executable="${MATTER_PY_PATH}" --args='treat_warnings_as_errors=false target_os="linux" target_cpu="${TARGET_CPU}" arm_arch="${TARGET_ARM_ARCH}" arm_cpu="${TARGET_ARM_CPU}" build_without_pw=true chip_with_imx_ele=${USE_ELE} enable_exceptions=true chip_code_pre_generated_directory="${S}/zzz_pregencodes"
        import("//build_overrides/build.gni")
        target_cflags=[
                        "-DCHIP_DEVICE_CONFIG_WIFI_STATION_IF_NAME=\"mlan0\""
                       ]
        custom_toolchain="${build_root}/toolchain/custom"
        target_cc="${CC}"
        target_cxx="${CXX}"
        target_ar="${AR}"'
}
trusty_configure() {
    PKG_CONFIG_SYSROOT_DIR=${PKG_CONFIG_SYSROOT_DIR} \
    PKG_CONFIG_LIBDIR=${PKG_CONFIG_PATH} \
    gn gen out/aarch64-trusty --script-executable="${MATTER_PY_PATH}" --args='treat_warnings_as_errors=false target_os="linux" target_cpu="${TARGET_CPU}" arm_arch="${TARGET_ARM_ARCH}" arm_cpu="${TARGET_ARM_CPU}" build_without_pw=true chip_with_trusty_os=1 enable_exceptions=true chip_code_pre_generated_directory="${S}/zzz_pregencodes"

        import("//build_overrides/build.gni")
        target_cflags=[
                        "-DCHIP_DEVICE_CONFIG_WIFI_STATION_IF_NAME=\"mlan0\""
                       ]
        custom_toolchain="${build_root}/toolchain/custom"
        target_cc="${CC}"
        target_cxx="${CXX}"
        target_ar="${AR}"'
}

do_configure:append() {
    cd ${S}/examples/nxp-media-app/linux
    common_configure
    if ${BUILD_M2ZIGBEE}; then
        cd ${S}/examples/bridge-app/nxp/linux-M2ZigbeeRcp-bridge
        common_configure
    fi

    cd ${S}/examples/nxp-network-manager-app/linux
    common_configure

    cd ${S}/examples/nxp-battery-storage-app/linux
    common_configure

    cd ${S}/examples/nxp-device-energy-management-app/linux
    common_configure

    cd ${S}/examples/nxp-evse-app/linux
    common_configure

    cd ${S}/examples/nxp-heat-pump-app/linux
    common_configure

    cd ${S}/examples/nxp-solar-power-app/linux
    common_configure

    cd ${S}/examples/nxp-water-heater-app/linux
    common_configure

    if ${DEPLOY_TRUSTY}; then
        cd ${S}/examples/lighting-app/linux
        trusty_configure

        cd ${S}/examples/chip-tool
        trusty_configure

        cd ${S}/examples/nxp-thermostat/linux
        trusty_configure

        cd ${S}/examples/nxp-media-app/linux
        trusty_configure
    fi
}

do_compile:append() {

    cd ${S}/examples/nxp-media-app/linux
    ninja -C out/aarch64

    cd ${S}/examples/nxp-network-manager-app/linux
    ninja -C out/aarch64

    cd ${S}/examples/nxp-battery-storage-app/linux
    ninja -C out/aarch64

    cd ${S}/examples/nxp-device-energy-management-app/linux
    ninja -C out/aarch64

    cd ${S}/examples/nxp-evse-app/linux
    ninja -C out/aarch64

    cd ${S}/examples/nxp-heat-pump-app/linux
    ninja -C out/aarch64

    cd ${S}/examples/nxp-solar-power-app/linux
    ninja -C out/aarch64

    cd ${S}/examples/nxp-water-heater-app/linux
    ninja -C out/aarch64

    if ${BUILD_M2ZIGBEE}; then
        cd ${S}/examples/bridge-app/nxp/linux-M2ZigbeeRcp-bridge
        ninja -C out/aarch64
    fi

    if ${DEPLOY_TRUSTY}; then
        cd ${S}/examples/lighting-app/linux
        ninja -C out/aarch64-trusty

        cd ${S}/examples/nxp-thermostat/linux
        ninja -C out/aarch64-trusty

        cd ${S}/examples/nxp-media-app/linux
        ninja -C out/aarch64-trusty

        cd ${S}/examples/chip-tool
        ninja -C out/aarch64-trusty
    fi
}

do_install:append() {
    install ${S}/examples/nxp-media-app/linux/out/aarch64/nxp-media-app ${D}${bindir}

    install ${S}/examples/nxp-network-manager-app/linux/out/aarch64/matter-nxp-network-manager-app ${D}${bindir}
    install ${S}/examples/nxp-battery-storage-app/linux/out/aarch64/chip-nxp-battery-storage-app ${D}${bindir}
    install ${S}/examples/nxp-device-energy-management-app/linux/out/aarch64/chip-nxp-device-energy-management-app ${D}${bindir}
    install ${S}/examples/nxp-evse-app/linux/out/aarch64/chip-nxp-evse-app ${D}${bindir}
    install ${S}/examples/nxp-heat-pump-app/linux/out/aarch64/chip-nxp-heat-pump-app ${D}${bindir}
    install ${S}/examples/nxp-solar-power-app/linux/out/aarch64/chip-nxp-solar-power-app ${D}${bindir}
    install ${S}/examples/nxp-water-heater-app/linux/out/aarch64/chip-nxp-water-heater-app ${D}${bindir}

    if ${BUILD_M2ZIGBEE}; then
        install ${S}/examples/bridge-app/nxp/linux-M2ZigbeeRcp-bridge/out/aarch64/M2ZigbeeRcp-bridge ${D}${bindir}
    fi

    if ${DEPLOY_TRUSTY}; then
        install ${S}/examples/lighting-app/linux/out/aarch64-trusty/chip-lighting-app ${D}${bindir}/chip-lighting-app-trusty
        install ${S}/examples/nxp-thermostat/linux/out/aarch64-trusty/nxp-thermostat-app ${D}${bindir}/nxp-thermostat-app-trusty
        install ${S}/examples/nxp-media-app/linux/out/aarch64-trusty/nxp-media-app ${D}${bindir}/nxp-media-app-trusty
        install ${S}/examples/chip-tool/out/aarch64-trusty/chip-tool ${D}${bindir}/chip-tool-trusty
    fi

    # Install ota.sh
    install -m 755 ${UNPACKDIR}/ota.sh ${D}${bindir}
}
