FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:${THISDIR}/files:"

SRC_URI += "file://ota.sh"
DEPENDS += "${@bb.utils.contains_any('MACHINE', "imx8mmevk-matter imx8mnddr3levk-matter imx8mnevk-matter imx8mpevk-matter imx91evk-iwxxx-matter imx93evk-iwxxx-matter ", ' zigbee-rcp-sdk ', '', d)}"
RDEPENDS:${PN} += "${@bb.utils.contains_any('MACHINE', "imx8mmevk-matter imx8mnddr3levk-matter imx8mnevk-matter imx8mpevk-matter imx91evk-iwxxx-matter imx93evk-iwxxx-matter ", ' zigbee-rcp-sdk ', '', d)}"


#DEPLOY_TRUSTY = "${@bb.utils.contains('MACHINE_FEATURES', 'trusty', 'true', 'false', d)}"

# Append advanced applications to the central list. These use the standard 'aarch64' output directory
# and do not require extra GN arguments.
# Format: 'app-path|binary-name|output-dir|extra-gn-args|install-binary-name'
#MATTER_APPLICATIONS += " \
#    'nxp-media-app/linux|nxp-media-app|aarch64||nxp-media-app' \
#    'nxp-network-manager-app/linux|matter-nxp-network-manager-app|aarch64||matter-nxp-network-manager-app' \
#    'nxp-battery-storage-app/linux|chip-nxp-battery-storage-app|aarch64||chip-nxp-battery-storage-app' \
#    'nxp-device-energy-management-app/linux|chip-nxp-device-energy-management-app|aarch64||chip-nxp-device-energy-management-app' \
#    'nxp-evse-app/linux|chip-nxp-evse-app|aarch64||chip-nxp-evse-app' \
#    'nxp-heat-pump-app/linux|chip-nxp-heat-pump-app|aarch64||chip-nxp-heat-pump-app' \
#    'nxp-solar-power-app/linux|chip-nxp-solar-power-app|aarch64||chip-nxp-solar-power-app' \
#    'nxp-water-heater-app/linux|chip-nxp-water-heater-app|aarch64||chip-nxp-water-heater-app' \
#"

# For platform which support M2Zigbee RCP bridge
#MATTER_APPLICATIONS:append = " ${@bb.utils.contains_any('MACHINE', 'imx8mmevk-matter imx8mnddr3levk-matter imx8mnevk-matter imx8mpevk-matter imx91evk-iwxxx-matter imx93evk-iwxxx-matter', \
#    "'bridge-app/nxp/linux-M2ZigbeeRcp-bridge|M2ZigbeeRcp-bridge|aarch64||M2ZigbeeRcp-bridge'", '', d)}"

#    'nxp-thermostat/linux|nxp-thermostat-app|aarch64-trusty|chip_with_trusty_os=true|nxp-thermostat-app-trusty'
#     'nxp-media-app/linux|nxp-media-app|aarch64-trusty|chip_with_trusty_os=true|nxp-media-app-trusty'

MATTER_APPLICATIONS:append = " ${@bb.utils.contains('MACHINE_FEATURES', 'trusty', " \
    'lighting-app/linux|chip-lighting-app|aarch64-trusty|chip_with_trusty_os=true|chip-lighting-app-trusty' \
    'chip-tool|chip-tool|aarch64-trusty|chip_with_trusty_os=true|chip-tool-trusty' \
", '', d)}"

do_configure:prepend() {
    cd ${S}/
#    if ${DEPLOY_TRUSTY}; then
#        git submodule update --init
#        ./scripts/checkout_submodules.py
#    fi
    cd ${S}
}

do_install:append() {
    install -m 755 "${UNPACKDIR}/ota.sh" "${D}${bindir}"
}
