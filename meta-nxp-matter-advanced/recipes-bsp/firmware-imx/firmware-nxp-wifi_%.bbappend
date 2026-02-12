# Use the latest revision

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI += "file://conf-files/wifi_mod_para.conf"
SRC_URI += "file://conf-files/sdiw610_WlanCalData_ext.conf"

addtask do_set_firmware_path after do_unpack before do_patch

do_set_firmware_path() {
    set -e
    if [ -d "${S}/nxp" ]; then
        SRCDIR="${S}/nxp"
    else
        SRCDIR="${S}"
    fi

    ln -frs "${SRCDIR}" "${WORKDIR}/srctree"
}

do_install:prepend() {
    SRCDIR="${WORKDIR}/srctree"

    cp ${UNPACKDIR}/conf-files/sdiw610_WlanCalData_ext.conf ${SRCDIR}/FwImage_IW610_SD

    rm ${SRCDIR}/wifi_mod_para.conf
    cp ${UNPACKDIR}/conf-files/wifi_mod_para.conf ${SRCDIR}
}

FILES:${PN}-nxpiw610-sdio += " \
    ${nonarch_base_libdir}/firmware/nxp/sdiw610_WlanCalData_ext.conf \
"
