inherit cmake
SRC_URI = "gitsm://github.com/openthread/ot-br-posix.git;branch=main;protocol=https"
SRCREV = "252be083041f3597c78b41a85939c6799a5942a7"

# patch files for otbr-posix
SRC_URI += "file://otbrpatches-085-d31bcee/otbr-0004-firewall.patch"
SRC_URI += "file://otbrpatches-085-d31bcee/otbr-0007-support-ot-fct.patch"
SRC_URI += "file://otbrpatches-085-d31bcee/otbr-0008-Fix-mcpu-cortex-a53-conflicts-with-march-armv8-a-crc.patch"
SRC_URI += "file://otbrpatches-085-d31bcee/otbr-0009-systemctl-mdns-otbr-options-set-by-otbr-agent-service.patch"
SRC_URI += "file://otbrpatches-085-d31bcee/otbr-0010-pd-daemon-restart-dhcpcd.patch"
SRC_URI += "file://start_openthread.sh"

# patch files for openthread
SRC_URI += "file://otbrpatches-085-d31bcee/openthread_patches/0001-coex-buff-issue-fix.patch;patchdir=third_party/openthread/repo/"
SRC_URI += "file://otbrpatches-085-d31bcee/openthread_patches/0002-csl-debug_prints.patch;patchdir=third_party/openthread/repo/"
SRC_URI += "file://otbrpatches-085-d31bcee/openthread_patches/0003_all_VSC_commands.patch;patchdir=third_party/openthread/repo/"
SRC_URI += "file://otbrpatches-085-d31bcee/openthread_patches/0006-ot-daemon-release-resources.patch;patchdir=third_party/openthread/repo/"
SRC_URI += "file://otbrpatches-085-d31bcee/openthread_patches/0019-remove-toggle-reset-pin.patch;patchdir=third_party/openthread/repo/"
SRC_URI += "file://otbrpatches-085-d31bcee/openthread_patches/0020-lnt-fix-spi-latency.patch;patchdir=third_party/openthread/repo/"
SRC_URI += "file://otbrpatches-085-d31bcee/openthread_patches/0025-spi-new-design-host.patch;patchdir=third_party/openthread/repo/"
SRC_URI += "file://otbrpatches-085-d31bcee/openthread_patches/0034-set-default-txpower-on-reset.patch;patchdir=third_party/openthread/repo/"
SRC_URI += "file://otbrpatches-085-d31bcee/openthread_patches/0036-set-default-rssi-on-reset-for-host.patch;patchdir=third_party/openthread/repo/"
SRC_URI += "file://otbrpatches-085-d31bcee/openthread_patches/0040-radio-spinel-stream-raw-retry-mechanism.patch;patchdir=third_party/openthread/repo/"
SRC_URI += "file://otbrpatches-085-d31bcee/openthread_patches/0051-csl-ch-switch-performance-improvement.patch;patchdir=third_party/openthread/repo/"
SRC_URI += "file://otbrpatches-085-d31bcee/openthread_patches/0057-reset-available-on-rcp-recovery.patch;patchdir=third_party/openthread/repo/"
SRC_URI += "file://otbrpatches-085-d31bcee/openthread_patches/0060-dualpan-otzb-pan-channel-sense.patch;patchdir=third_party/openthread/repo/"
SRC_URI += "file://otbrpatches-085-d31bcee/openthread_patches/0062-host-handle-power-save-mode-on-ssp-rxd.patch;patchdir=third_party/openthread/repo/"
SRC_URI += "file://otbrpatches-085-d31bcee/openthread_patches/0068-generic-mfgcmd-support.patch;patchdir=third_party/openthread/repo/"
SRC_URI += "file://otbrpatches-085-d31bcee/openthread_patches/0070-Specify-MBEDTLS_COMPILER_IS_GCC-inlibrary-common.h.patch;patchdir=third_party/openthread/repo/"
SRC_URI += "file://otbrpatches-085-d31bcee/openthread_patches/0071-mfgcmd-spifreq-support.patch;patchdir=third_party/openthread/repo/"
SRC_URI += "file://otbrpatches-085-d31bcee/openthread_patches/0072-Fix-link-error-in-tests-unit-ot-test-ncp-srp_server.patch;patchdir=third_party/openthread/repo/"
SRC_URI += "file://otbrpatches-085-d31bcee/openthread_patches/0076-Resolver-to-support-IPv6-addresses-for-servers.patch;patchdir=third_party/openthread/repo/"
SRC_URI += "file://otbrpatches-085-d31bcee/openthread_patches/0077-support-Recursive-DNS-Server-RDNSS-option.patch;patchdir=third_party/openthread/repo/"
SRC_URI += "file://otbrpatches-085-d31bcee/openthread_patches/0078-process-RDNSS-option.patch;patchdir=third_party/openthread/repo/"
SRC_URI += "file://otbrpatches-085-d31bcee/openthread_patches/0079-Resolver-to-support-RDNSS-discovered-recursive-DNS-servers.patch;patchdir=third_party/openthread/repo/"
SRC_URI += "file://otbrpatches-085-d31bcee/openthread_patches/0080-add-netif-option.patch;patchdir=third_party/openthread/repo/"
SRC_URI += "file://otbrpatches-085-d31bcee/openthread_patches/0081-update-dns-resolver-to_attempt-all-server-before-failure.patch;patchdir=third_party/openthread/repo/"
SRC_URI += "file://otbrpatches-085-d31bcee/openthread_patches/0082-DNS-resolver-to-handle-link-local-server-address.patch;patchdir=third_party/openthread/repo/"

EXTRA_OECMAKE += "-DCMAKE_CXX_STANDARD=17 -DBUILD_TESTING=OFF -DOTBR_BACKBONE_ROUTER=ON -DOTBR_BORDER_AGENT=ON -DOTBR_BORDER_ROUTING=ON -DOTBR_DBUS=ON -DOTBR_DNSSD_DISCOVERY_PROXY=ON -DOTBR_DUA_ROUTING=ON -DOTBR_INFRA_IF_NAME=eth0 -DOTBR_MDNS=mDNSResponder -DOTBR_REST=ON -DOTBR_SRP_ADVERTISING_PROXY=ON -DOTBR_TREL=ON -DOTBR_WEB=OFF -DOTBR_NAT64=ON -DOTBR_DHCP6_PD=ON -DOTBR_DNS_UPSTREAM_QUERY=ON -DOT_BACKBONE_ROUTER_MULTICAST_ROUTING=ON -DOT_BORDER_ROUTER=ON -DOT_BORDER_AGENT=ON -DOT_COMMISSIONER=ON -DOT_COAP=ON -DOT_COAP_BLOCK=OFF -DOT_COAP_OBSERVE=ON -DOT_COAPS=ON -DOT_DHCP6_SERVER=ON -DOT_DHCP6_CLIENT=ON -DOT_DNS_CLIENT=ON -DOT_DNSSD_SERVER=ON -DOT_DUA=ON -DOT_ECDSA=ON -DOT_FIREWALL=ON -DOT_FULL_LOGS=ON -DOT_JOINER=ON -DOT_LOG_LEVEL_DYNAMIC=OFF -DOT_LOG_LEVEL=DEBG -DOT_RCP_RESTORATION_MAX_COUNT=5 -DOT_REFERENCE_DEVICE=ON -DOT_SRP_CLIENT=ON -DOT_SRP_SERVER=ON -DOT_THREAD_VERSION=1.4 -DOT_NAT64_BORDER_ROUTING=ON -DOT_NAT64_TRANSLATOR=ON -DOT_VENDOR_NAME=NXP -DOT_VENDOR_MODEL=IWxxx -DOT_NETDIAG_VENDOR_INFO=OFF -DOT_POSIX_PRODUCT_CONFIG=/data/openthread/openthread.conf -DOT_MLE_MAX_CHILDREN=128 -DOT_MAC_CSL_REQUEST_AHEAD_US=16500 -DOT_POSIX_RCP_SPI_BUS=ON -DOT_POSIX_RCP_HDLC_BUS=ON"

addtask do_openthread_repo_revision_reset after do_unpack before do_patch
do_openthread_repo_revision_reset() {
    cd ${S}/third_party/openthread/repo
    echo "Applying on ${S}/third_party/openthread/repo:"
    echo "-> git reset --hard d31bcee81230bd82764838c76c9f291ba64814dd"
    git reset --hard d31bcee81230bd82764838c76c9f291ba64814dd
    result=$?
    if [ ${result} -ne 0 ]; then
        echo "Cannot reset openthread to version d31bcee81230bd82764838c76c9f291ba64814dd, ABORT"
        exit
    fi
}

do_install:append() {
    install -d -m 755 ${D}${sbindir}
    echo "Renaming ${D}${sbindir}/otbr-agent into ${D}${sbindir}/otbr-agent${BIN_NAME_PATTERN}"
    mv ${D}${sbindir}/otbr-agent ${D}${sbindir}/otbr-agent${BIN_NAME_PATTERN}
    echo "Renaming ${D}${sbindir}/ot-ctl into ${D}${sbindir}/ot-ctl${BIN_NAME_PATTERN}"
    mv ${D}${sbindir}/ot-ctl ${D}${sbindir}/ot-ctl${BIN_NAME_PATTERN}

    echo "Moving ${D}${systemd_system_unitdir}/imx_otbr_gpiochip_spidev_cfg.sh into ${D}${sbindir}"
    mv ${D}${systemd_system_unitdir}/imx_otbr_gpiochip_spidev_cfg.sh ${D}${sbindir}
    chmod +x ${D}${sbindir}/imx_otbr_gpiochip_spidev_cfg.sh
    echo "Copying ${UNPACKDIR}/start_openthread.sh into ${D}${sbindir}"
    cp ${UNPACKDIR}/start_openthread.sh ${D}${sbindir}
    chmod +x ${D}${sbindir}/start_openthread.sh

    echo "Moving ${D}${systemd_system_unitdir}/dhcp6_pd_daemon.py into ${D}${sysconfdir}"
    mv ${D}${systemd_system_unitdir}/dhcp6_pd_daemon.py ${D}${sysconfdir}
    chmod +x ${D}${sysconfdir}/dhcp6_pd_daemon.py
    echo "Moving ${D}${systemd_system_unitdir}/dhcpcd.enter-hook into ${D}${sysconfdir}"
    mv ${D}${systemd_system_unitdir}/dhcpcd.enter-hook ${D}${sysconfdir}
    echo "Moving ${D}${systemd_system_unitdir}/dhcpcd.exit-hook into ${D}${sysconfdir}"
    mv ${D}${systemd_system_unitdir}/dhcpcd.exit-hook ${D}${sysconfdir}

    echo "Renaming ${D}${sysconfdir}/default/otbr-agent into ${D}${sysconfdir}/default/otbr-agent${BIN_NAME_PATTERN}"
    mv ${D}${sysconfdir}/default/otbr-agent ${D}${sysconfdir}/default/otbr-agent${BIN_NAME_PATTERN}

    echo "Copying ${S}/script/otbr-firewall into ${D}${sysconfdir}/init.d"
    mkdir ${D}${sysconfdir}/init.d
    cp ${S}/script/otbr-firewall ${D}${sysconfdir}/init.d
    chmod +x ${D}${sysconfdir}/init.d/otbr-firewall
    echo "Copying ${S}/script/README_firewall.txt into ${D}${sysconfdir}/init.d"
    cp ${S}/script/README_firewall.txt ${D}${sysconfdir}/init.d
}
