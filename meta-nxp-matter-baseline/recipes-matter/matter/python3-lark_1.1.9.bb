SUMMARY = "A modern parsing library for Python"
HOMEPAGE = "https://github.com/lark-parser/lark"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=d18d413d751561045239e0edc91629d8"
SRC_URI[sha256sum] = "15fa5236490824c2c4aba0e22d2d6d823575dcaf4cdd1848e34b6ad836240fba"

inherit pypi
BBCLASSEXTEND = "native"

DEPENDS += " \
    python3-build-native \
    python3-installer-native \
    python3-setuptools-native \
    python3-wheel-native \
"

do_compile() {
    cd ${S}
    ${STAGING_BINDIR_NATIVE}/python3-native/python3 -m build --wheel --no-isolation
}

do_install() {
    cd ${S}
    ${STAGING_BINDIR_NATIVE}/python3-native/python3 -m installer --destdir ${D} dist/*.whl
}
