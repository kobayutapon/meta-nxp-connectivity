# Use the latest revision

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"
SRC_URI += "file://IW612-Q4-25-RC1-p27/sduart_nw61x_v1.bin.se"
SRC_URI += "file://IW612-Q4-25-RC1-p27/sd_w61x_v1.bin.se"
SRC_URI += "file://IW612-Q4-25-RC1-p27/uartspi_n61x_v1.bin.se"

SRC_URI += "file://IW610-Q4-25-RC1-p78/sd_iw610.bin.se"
SRC_URI += "file://IW610-Q4-25-RC1-p78/sduart_iw610.bin.se"
SRC_URI += "file://IW610-Q4-25-RC1-p78/uart_iw610_bt.bin.se"
SRC_URI += "file://IW610-Q4-25-RC1-p78/uartspi_iw610.bin.se"
SRC_URI += "file://IW610-Q4-25-RC1-p78/sduartspi_iw610.bin.se"

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

    rm -f ${SRCDIR}/FwImage_IW612_SD/*.se
    cp ${UNPACKDIR}/IW612-Q4-25-RC1-p27/sduart_nw61x_v1.bin.se ${SRCDIR}/FwImage_IW612_SD
    cp ${UNPACKDIR}/IW612-Q4-25-RC1-p27/sd_w61x_v1.bin.se ${SRCDIR}/FwImage_IW612_SD
    cp ${UNPACKDIR}/IW612-Q4-25-RC1-p27/uartspi_n61x_v1.bin.se ${SRCDIR}/FwImage_IW612_SD

    rm -f ${SRCDIR}/FwImage_IW610_SD/*
    cp ${UNPACKDIR}/IW610-Q4-25-RC1-p78/sd_iw610.bin.se ${SRCDIR}/FwImage_IW610_SD
    cp ${UNPACKDIR}/IW610-Q4-25-RC1-p78/sduart_iw610.bin.se ${SRCDIR}/FwImage_IW610_SD
    cp ${UNPACKDIR}/IW610-Q4-25-RC1-p78/uart_iw610_bt.bin.se ${SRCDIR}/FwImage_IW610_SD
    cp ${UNPACKDIR}/IW610-Q4-25-RC1-p78/uartspi_iw610.bin.se ${SRCDIR}/FwImage_IW610_SD
    cp ${UNPACKDIR}/IW610-Q4-25-RC1-p78/sduartspi_iw610.bin.se ${SRCDIR}/FwImage_IW610_SD
    cp ${UNPACKDIR}/conf-files/sdiw610_WlanCalData_ext.conf ${SRCDIR}/FwImage_IW610_SD

    rm ${SRCDIR}/wifi_mod_para.conf
    cp ${UNPACKDIR}/conf-files/wifi_mod_para.conf ${SRCDIR}
}

FILES:${PN}-nxpiw610-sdio += " \
    ${nonarch_base_libdir}/firmware/nxp/sdiw610_WlanCalData_ext.conf \
"
