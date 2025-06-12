inherit cmake
SRC_URI = "gitsm://github.com/openthread/openthread.git;branch=main;protocol=https"
SRCREV = "d31bcee81230bd82764838c76c9f291ba64814dd"

# patch files for openthread
SRC_URI += "file://otpatches-088-d31bcee/0001-coex-buff-issue-fix.patch"
SRC_URI += "file://otpatches-088-d31bcee/0002-csl-debug_prints.patch"
SRC_URI += "file://otpatches-088-d31bcee/0003_all_VSC_commands.patch"
SRC_URI += "file://otpatches-088-d31bcee/0006-ot-daemon-release-resources.patch"
SRC_URI += "file://otpatches-088-d31bcee/0019-remove-toggle-reset-pin.patch"
SRC_URI += "file://otpatches-088-d31bcee/0020-lnt-fix-spi-latency.patch"
SRC_URI += "file://otpatches-088-d31bcee/0025-spi-new-design-host.patch"
SRC_URI += "file://otpatches-088-d31bcee/0034-set-default-txpower-on-reset.patch"
SRC_URI += "file://otpatches-088-d31bcee/0036-set-default-rssi-on-reset-for-host.patch"
SRC_URI += "file://otpatches-088-d31bcee/0040-radio-spinel-stream-raw-retry-mechanism.patch"
SRC_URI += "file://otpatches-088-d31bcee/0051-csl-ch-switch-performance-improvement.patch"
SRC_URI += "file://otpatches-088-d31bcee/0057-reset-available-on-rcp-recovery.patch"
SRC_URI += "file://otpatches-088-d31bcee/0060-dualpan-otzb-pan-channel-sense.patch"
SRC_URI += "file://otpatches-088-d31bcee/0062-host-handle-power-save-mode-on-ssp-rxd.patch"
SRC_URI += "file://otpatches-088-d31bcee/0068-generic-mfgcmd-support.patch"
SRC_URI += "file://otpatches-088-d31bcee/0070-Specify-MBEDTLS_COMPILER_IS_GCC-inlibrary-common.h.patch"
SRC_URI += "file://otpatches-088-d31bcee/0071-mfgcmd-spifreq-support.patch"
SRC_URI += "file://otpatches-088-d31bcee/0072-Fix-link-error-in-tests-unit-ot-test-ncp-srp_server.patch"
SRC_URI += "file://otpatches-088-d31bcee/0076-Resolver-to-support-IPv6-addresses-for-servers.patch"
SRC_URI += "file://otpatches-088-d31bcee/0077-support-Recursive-DNS-Server-RDNSS-option.patch"
SRC_URI += "file://otpatches-088-d31bcee/0078-process-RDNSS-option.patch"
SRC_URI += "file://otpatches-088-d31bcee/0079-Resolver-to-support-RDNSS-discovered-recursive-DNS-servers.patch"
SRC_URI += "file://otpatches-088-d31bcee/0080-add-netif-option.patch"
SRC_URI += "file://otpatches-088-d31bcee/0081-update-dns-resolver-to_attempt-all-server-before-failure.patch"
SRC_URI += "file://otpatches-088-d31bcee/0082-DNS-resolver-to-handle-link-local-server-address.patch"

EXTRA_OECMAKE += "-DOT_SLAAC=ON -DOT_ANYCAST_LOCATOR=ON -DOT_BLE_TCAT=ON -DOT_BORDER_AGENT=ON -DOT_BORDER_AGENT_EPSKC=ON -DOT_BORDER_AGENT_ID=ON -DOT_BORDER_ROUTER=ON -DOT_CHANNEL_MANAGER=ON -DOT_CHANNEL_MONITOR=ON -DOT_COAP=ON -DOT_COAPS=ON -DOT_COAP_BLOCK=ON -DOT_COAP_OBSERVE=ON -DOT_COMMISSIONER=ON -DOT_COMPILE_WARNING_AS_ERROR=ON -DOT_COVERAGE=ON -DOT_DATASET_UPDATER=ON -DOT_DHCP6_CLIENT=ON -DOT_DHCP6_SERVER=ON -DOT_DIAGNOSTIC=ON -DOT_DNSSD_SERVER=ON -DOT_DNS_CLIENT=ON -DOT_ECDSA=ON -DOT_HISTORY_TRACKER=ON -DOT_IP6_FRAGM=ON -DOT_JAM_DETECTION=ON -DOT_JOINER=ON -DOT_LOG_LEVEL_DYNAMIC=ON -DOT_MAC_FILTER=ON -DOT_NEIGHBOR_DISCOVERY_AGENT=ON -DOT_NETDATA_PUBLISHER=ON -DOT_NETDIAG_CLIENT=ON -DOT_PING_SENDER=ON -DOT_RCP_RESTORATION_MAX_COUNT=2 -DOT_RCP_TX_WAIT_TIME_SECS=5 -DOT_REFERENCE_DEVICE=ON -DOT_SERVICE=ON -DOT_SNTP_CLIENT=ON -DOT_SRP_CLIENT=ON -DOT_SRP_SERVER=ON -DOT_UPTIME=ON -DOT_TCP=OFF -DOT_LOG_OUTPUT=PLATFORM_DEFINED -DOT_POSIX_MAX_POWER_TABLE=ON -DOT_PLATFORM=posix -DCMAKE_BUILD_TYPE=Release -DOT_COVERAGE=0 -DOT_COMPILE_WARNING_AS_ERROR=0 -DOT_DAEMON=1 -DOT_BACKBONE_ROUTER=1 -DOT_FULL_LOGS=1 -DOT_DUA=1 -DOT_LINK_METRICS_INITIATOR=1 -DOT_LINK_METRICS_SUBJECT=1 -DOT_MLR=1 -DOT_RCP_RESTORATION_MAX_COUNT=5 -DOT_TCP=OFF -DOT_THREAD_VERSION=1.4 -DOT_CHANNEL_MONITOR=0 -DOT_VENDOR_NAME=NXP -DOT_VENDOR_MODEL=IWxxx -DOT_NETDIAG_VENDOR_INFO=OFF -DOT_POSIX_PRODUCT_CONFIG=/data/openthread/openthread.conf -DOT_MLE_MAX_CHILDREN=128 -DOT_MAC_CSL_REQUEST_AHEAD_US=16500 -DOT_POSIX_RCP_SPI_BUS=1 -DOT_POSIX_RCP_HDLC_BUS=1 -DOT_DIAGNOSTIC=ON -DOT_BORDER_ROUTING_DHCP6_PD=ON -DOT_DNS_UPSTREAM_QUERY=ON"

do_install:append() {
    install -d -m 755 ${D}${bindir}
    echo "Renaming ${D}${sbindir}/ot-daemon into ${D}${bindir}/ot-daemon${BIN_NAME_PATTERN}"
    mv ${D}${sbindir}/ot-daemon ${D}${bindir}/ot-daemon${BIN_NAME_PATTERN}
    echo "Renaming ${D}${bindir}/ot-ctl into ${D}${bindir}/ot-client${BIN_NAME_PATTERN}"
    mv ${D}${bindir}/ot-ctl ${D}${bindir}/ot-client${BIN_NAME_PATTERN}
    install -d -m 755 ${D}${sbindir}
    rm -rf ${D}${sbindir}
}
