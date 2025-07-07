SUMMARY = "Matter IDL Python parser from Project CHIP"
HOMEPAGE = "https://github.com/project-chip/connectedhomeip"
LICENSE = "Apache-2.0"
PN = "matter-idl"
LIC_FILES_CHKSUM = "file://../../LICENSE;md5=8f4f267a15a862b535b914a274e2a873"

SRC_URI = "git://github.com/project-chip/connectedhomeip.git;protocol=https;branch=master"
SRCREV = "9151e79d6b5c010f18e3a3da0b048f38196f06c6"
BBCLASSEXTEND = "native"

inherit setuptools3

S = "${WORKDIR}/git/scripts/py_matter_idl"

RDEPENDS:${PN} += " \
    python3-click-native \
    python3-coloredlogs-native \
"
