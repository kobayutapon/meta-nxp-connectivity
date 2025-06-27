FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI += "${@bb.utils.contains_any('MACHINE', "imx8mmevk-matter imx93evk-iwxxx-matter ", 'file://0001-MATTER-3026-1-enable-Trusty-OS-for-matter.patch', '', d)}"
SRC_URI += "${@bb.utils.contains('MACHINE', "imx8mmevk-matter", 'file://0002-MATTER-3026-2-enable-Trusty-OS-for-matter.patch', '', d)}"
SRC_URI += "${@bb.utils.contains('MACHINE', "imx8mmevk-matter", 'file://0001-Add-MCU-NCP-Host-driver.patch', '', d)}"
SRC_URI += "${@bb.utils.contains('MACHINE', "imx8mmevk-matter", 'file://kernel-config/Enable-Trusty.cfg', '', d)}"
SRC_URI += "${@bb.utils.contains('MACHINE', "imx8mmevk-matter", 'file://kernel-config/Enable-NCP-Host.cfg', '', d)}"
