/*
 * Copyright 2024-2025 NXP
 *
 * NXP CONFIDENTIAL
 * The source code contained or described herein and all documents related to
 * the source code ("Materials") are owned by NXP ( NXP ), its
 * suppliers and/or its licensors. Title to the Materials remains with NXP,
 * its suppliers and/or its licensors. The Materials contain
 * trade secrets and proprietary and confidential information of NXP, its
 * suppliers and/or its licensors. The Materials are protected by worldwide copyright
 * and trade secret laws and treaty provisions. No part of the Materials may be
 * used, copied, reproduced, modified, published, uploaded, posted,
 * transmitted, distributed, or disclosed in any way without NXP's prior
 * express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise. Any license under such intellectual property rights must be
 * express and approved by NXP in writing.
 */

#ifndef OTA_NXP_DEFINITIONS_H
#define OTA_NXP_DEFINITIONS_H 1


#define ZBOSS_OTA_SERVER_DIR         "ZBOSS_OTA_SERVER_DIR"
#define ZBOSS_OTA_SERVER_DEFAULT_DIR "./ota-server-files"


enum zb_zcl_ota_upgrade_nxp_image_type_e {
    /* 0x0000 - 0xFFBF: Manufacturer Specific */
    

    /* File format:
     * <manufacture code>-<Image Type>-<Version>-<Name>.zigbee
     * note: <Name> is carried in header string
     */

        IMAGE_TYPE_NXP_ZBOSS_OFFSET = 0x1000, /* Range 0x1000 ~ 0x1FFF */
        /*
         * zboss_host_tag is <CC>.DD>                       (example 16.01)
         * zboss_stack is zboss<AA>-stabilization-phase<BB> (example zoi_release-4.1.4.2)
         *
         * => version: <AA><BB><CC><DD>                     (example 16014142)
         *
         * with AA: app release, BB: app build, CC: stack release, DD: stack build
         * (refer to zigbee cluster library specificatio 11.4.2.7)
         */
            IMAGE_TYPE_NXP_ZBOSS_MODULE_OFFSET     = IMAGE_TYPE_NXP_ZBOSS_OFFSET + 0x0000,
            IMAGE_TYPE_NXP_ZBOSS_LIB_ZC_OFFSET     = IMAGE_TYPE_NXP_ZBOSS_OFFSET + 0x0010, /* 15 libs  */
            IMAGE_TYPE_NXP_ZBOSS_LIB_ZR_OFFSET     = IMAGE_TYPE_NXP_ZBOSS_OFFSET + 0x0030, /* Same as ZC */
            IMAGE_TYPE_NXP_ZBOSS_LIB_ED_OFFSET     = IMAGE_TYPE_NXP_ZBOSS_OFFSET + 0x0050, /* 14 libs */
            IMAGE_TYPE_NXP_ZBOSS_SCRIPT_OFFSET     = IMAGE_TYPE_NXP_ZBOSS_OFFSET + 0x0070, 
            IMAGE_TYPE_NXP_ZBOSS_SERVICE_OFFSET    = IMAGE_TYPE_NXP_ZBOSS_OFFSET + 0x0080,
            IMAGE_TYPE_NXP_ZBOSS_APPLI_OFFSET      = IMAGE_TYPE_NXP_ZBOSS_OFFSET + 0x0100,
            IMAGE_TYPE_NXP_ZBOSS_EXAMPLE_OFFSET    = IMAGE_TYPE_NXP_ZBOSS_OFFSET + 0x0400,
            IMAGE_TYPE_NXP_ZBOSS_MAX_OFFSET        = IMAGE_TYPE_NXP_ZBOSS_OFFSET + 0x1000,

    /* 0x1070: imx-dualpan.sh */
    OTA_UPGRADE_HEADER_IMAGE_TYPE_NXP_SCRIPT_MANUAL_TEST                = IMAGE_TYPE_NXP_ZBOSS_SCRIPT_OFFSET   + 0x0000,
    /* 0x1071: imx-wifi.sh */
    OTA_UPGRADE_HEADER_IMAGE_TYPE_NXP_SCRIPT_WIFI_TEST                  = IMAGE_TYPE_NXP_ZBOSS_SCRIPT_OFFSET   + 0x0001,
    /* 0x1075: x86-zigbee.sh */
    OTA_UPGRADE_HEADER_IMAGE_TYPE_NXP_SCRIPT_X86_TEST                   = IMAGE_TYPE_NXP_ZBOSS_SCRIPT_OFFSET   + 0x0005,

    /* 0x1080: zb_config.service */
    OTA_UPGRADE_HEADER_IMAGE_TYPE_NXP_SERVICE_ZB_CFG_SRV                = IMAGE_TYPE_NXP_ZBOSS_SERVICE_OFFSET  + 0x0000,
    /* 0x1081: zb_config.sh */
    OTA_UPGRADE_HEADER_IMAGE_TYPE_NXP_SERVICE_ZB_CFG_SCRIPT             = IMAGE_TYPE_NXP_ZBOSS_SERVICE_OFFSET  + 0x0001,
    /* 0x1083: zb_mux.service */
    OTA_UPGRADE_HEADER_IMAGE_TYPE_NXP_SERVICE_ZB_MUX_SRV                = IMAGE_TYPE_NXP_ZBOSS_SERVICE_OFFSET  + 0x0003,
    /* 0x1084: zb_mux.sh */
    OTA_UPGRADE_HEADER_IMAGE_TYPE_NXP_SERVICE_ZB_MUX_SCRIPT             = IMAGE_TYPE_NXP_ZBOSS_SERVICE_OFFSET  + 0x0004,
    /* 0x1085: zb_mux.env */
    OTA_UPGRADE_HEADER_IMAGE_TYPE_NXP_SERVICE_ZB_MUX_CFG                = IMAGE_TYPE_NXP_ZBOSS_SERVICE_OFFSET  + 0x0005,
    /* 0x1086: zb_app.service */
    OTA_UPGRADE_HEADER_IMAGE_TYPE_NXP_SERVICE_ZB_APP_SRV                = IMAGE_TYPE_NXP_ZBOSS_SERVICE_OFFSET  + 0x0006,
    /* 0x1087: zb_app.sh */
    OTA_UPGRADE_HEADER_IMAGE_TYPE_NXP_SERVICE_ZB_APP_SCRIPT             = IMAGE_TYPE_NXP_ZBOSS_SERVICE_OFFSET  + 0x0007,
    /* 0x1088: zb_app.env */
    OTA_UPGRADE_HEADER_IMAGE_TYPE_NXP_SERVICE_ZB_APP_CFG                = IMAGE_TYPE_NXP_ZBOSS_SERVICE_OFFSET  + 0x0008,

    /* 0x1100: zb_mux */
    OTA_UPGRADE_HEADER_IMAGE_TYPE_NXP_APPLI_ZB_MUX                      = IMAGE_TYPE_NXP_ZBOSS_APPLI_OFFSET    + 0x0000,
    
    /* 0x1400: simple_gw */
    OTA_UPGRADE_HEADER_IMAGE_TYPE_NXP_APPLI_EXAMPLE_SIMPLE_GW           = IMAGE_TYPE_NXP_ZBOSS_EXAMPLE_OFFSET  + 0x0000,
    /* 0x1401: on_off_output_zc */
    OTA_UPGRADE_HEADER_IMAGE_TYPE_NXP_APPLI_EXAMPLE_ONOFF_SERVER_ZC     = IMAGE_TYPE_NXP_ZBOSS_EXAMPLE_OFFSET  + 0x0001,
    /* 0x1402: on_off_switch_zed */
    OTA_UPGRADE_HEADER_IMAGE_TYPE_NXP_APPLI_EXAMPLE_ONOFF_SERVER_ED     = IMAGE_TYPE_NXP_ZBOSS_EXAMPLE_OFFSET  + 0x0002,
    /* 0x1403: light_zc */
    OTA_UPGRADE_HEADER_IMAGE_TYPE_NXP_APPLI_EXAMPLE_LIGHT_ZC            = IMAGE_TYPE_NXP_ZBOSS_EXAMPLE_OFFSET  + 0x0003,
    /* 0x1404: bulb */
    OTA_UPGRADE_HEADER_IMAGE_TYPE_NXP_APPLI_EXAMPLE_LIGHT_ZR            = IMAGE_TYPE_NXP_ZBOSS_EXAMPLE_OFFSET  + 0x0004,
    /* 0x1405: light_control */
    OTA_UPGRADE_HEADER_IMAGE_TYPE_NXP_APPLI_EXAMPLE_LIGHT_ED            = IMAGE_TYPE_NXP_ZBOSS_EXAMPLE_OFFSET  + 0x0005,
    /* 0x1406: on_off_distrib_output_zr */
    OTA_UPGRADE_HEADER_IMAGE_TYPE_NXP_APPLI_EXAMPLE_ONOFF_DISTRIB_ZC    = IMAGE_TYPE_NXP_ZBOSS_EXAMPLE_OFFSET  + 0x0006,
    /* 0x1407: on_off_distrib_switch_zed */
    OTA_UPGRADE_HEADER_IMAGE_TYPE_NXP_APPLI_EXAMPLE_ONOFF_DISTRIB_ED    = IMAGE_TYPE_NXP_ZBOSS_EXAMPLE_OFFSET  + 0x0007,
    /* 0x1408: thermostat_zc */
    OTA_UPGRADE_HEADER_IMAGE_TYPE_NXP_APPLI_EXAMPLE_THERMOSTAT_ZC       = IMAGE_TYPE_NXP_ZBOSS_EXAMPLE_OFFSET  + 0x0008,
    /* 0x1409: thermostat_zr */
    OTA_UPGRADE_HEADER_IMAGE_TYPE_NXP_APPLI_EXAMPLE_THERMOSTAT_ZR       = IMAGE_TYPE_NXP_ZBOSS_EXAMPLE_OFFSET  + 0x0009,
    /* 0x140A: ota_simple_download_zc */
    OTA_UPGRADE_HEADER_IMAGE_TYPE_NXP_APPLI_EXAMPLE_OTA_DOWNLOAD_ZC     = IMAGE_TYPE_NXP_ZBOSS_EXAMPLE_OFFSET  + 0x000A,
    /* 0x140B: ota_simple_download_zr */
    OTA_UPGRADE_HEADER_IMAGE_TYPE_NXP_APPLI_EXAMPLE_OTA_DOWNLOAD_ZR     = IMAGE_TYPE_NXP_ZBOSS_EXAMPLE_OFFSET  + 0x000B,
    /* 0x140C: ota_server_zc */
    OTA_UPGRADE_HEADER_IMAGE_TYPE_NXP_APPLI_EXAMPLE_OTA_SERVER_ZC       = IMAGE_TYPE_NXP_ZBOSS_EXAMPLE_OFFSET  + 0x000C,
    /* 0x140D: ota_client_zr */
    OTA_UPGRADE_HEADER_IMAGE_TYPE_NXP_APPLI_EXAMPLE_OTA_CLIENT_ZR       = IMAGE_TYPE_NXP_ZBOSS_EXAMPLE_OFFSET  + 0x000D,
    /* NEXT... */
    

        IMAGE_TYPE_NXP_IW612_OFFSET = 0x6120, /* Range 0x6120 ~ 0x612F */
        /* 
         * header string:
         * - sd_w61x.bin(.se)
         * - sduart_nw61x.bin(.se)
         * - uartuart_n61x.bin(.se)
         * - uartspi_n61x.bin(.se)
         * 
         * iw612 version: <CC>.<DD>.<AA>p<BB> (example 18.99.3p4.x)
         * => version: <AA><BB><CC><DD>       (example 03041899)
         *
         * with AA: app release, BB: app build, CC: stack release, DD: stack build
         * (refer to zigbee cluster library specificatio 11.4.2.7)
         */

             IMAGE_TYPE_NXP_IW612_SD_ALONE_OFFSET    = IMAGE_TYPE_NXP_IW612_OFFSET + 0x0000,
             IMAGE_TYPE_NXP_IW612_SD_UART_OFFSET     = IMAGE_TYPE_NXP_IW612_OFFSET + 0x0004,
             IMAGE_TYPE_NXP_IW612_UART_UART_OFFSET   = IMAGE_TYPE_NXP_IW612_OFFSET + 0x0008,
             IMAGE_TYPE_NXP_IW612_UART_SPI_OFFSET    = IMAGE_TYPE_NXP_IW612_OFFSET + 0x000C,
             IMAGE_TYPE_NXP_IW612_MAX_OFFSET         = IMAGE_TYPE_NXP_IW612_OFFSET + 0x0010,

             IMAGE_TYPE_NXP_IW612_SIGNED_PROD_OFFSET = 0x0000,
             IMAGE_TYPE_NXP_IW612_SIGNED_ENG_OFFSET  = 0x0001,
             IMAGE_TYPE_NXP_IW612_UNSIGNED_OFFSET    = 0x0002,
             IMAGE_TYPE_NXP_IW612_SIGNATURE_MASK     = 0x0003,

     /* 0x6120: sd_w61x.bin.se */
     OTA_UPGRADE_HEADER_IMAGE_TYPE_NXP_IW612_SD_ALONE_SIGNED_PROD   = IMAGE_TYPE_NXP_IW612_SD_ALONE_OFFSET  + IMAGE_TYPE_NXP_IW612_SIGNED_PROD_OFFSET,
     /* 0x6121: sd_w61x.bin.se */
     OTA_UPGRADE_HEADER_IMAGE_TYPE_NXP_IW612_SD_ALONE_SIGNED_ENG    = IMAGE_TYPE_NXP_IW612_SD_ALONE_OFFSET  + IMAGE_TYPE_NXP_IW612_SIGNED_ENG_OFFSET,
     /* 0x6122: sd_w61x.bin    */
     OTA_UPGRADE_HEADER_IMAGE_TYPE_NXP_IW612_SD_ALONE_UNSIGNED      = IMAGE_TYPE_NXP_IW612_SD_ALONE_OFFSET  + IMAGE_TYPE_NXP_IW612_UNSIGNED_OFFSET,
     /* 0x6124: sduart_nw61x.bin.se */
     OTA_UPGRADE_HEADER_IMAGE_TYPE_NXP_IW612_SD_UART_SIGNED_PROD    = IMAGE_TYPE_NXP_IW612_SD_UART_OFFSET   + IMAGE_TYPE_NXP_IW612_SIGNED_PROD_OFFSET,
     /* 0x6125: sduart_nw61x.bin.se */
     OTA_UPGRADE_HEADER_IMAGE_TYPE_NXP_IW612_SD_UART_SIGNED_ENG     = IMAGE_TYPE_NXP_IW612_SD_UART_OFFSET   + IMAGE_TYPE_NXP_IW612_SIGNED_ENG_OFFSET,
     /* 0x6126: sduart_nw61x.bin    */
     OTA_UPGRADE_HEADER_IMAGE_TYPE_NXP_IW612_SD_UART_UNSIGNED       = IMAGE_TYPE_NXP_IW612_SD_UART_OFFSET   + IMAGE_TYPE_NXP_IW612_UNSIGNED_OFFSET,
     /* 0x6128: uartuart_n61x.bin.se */
     OTA_UPGRADE_HEADER_IMAGE_TYPE_NXP_IW612_UART_UART_SIGNED_PROD  = IMAGE_TYPE_NXP_IW612_UART_UART_OFFSET + IMAGE_TYPE_NXP_IW612_SIGNED_PROD_OFFSET,
     /* 0x6129: uartuart_n61x.bin.se */
     OTA_UPGRADE_HEADER_IMAGE_TYPE_NXP_IW612_UART_UART_SIGNED_ENG   = IMAGE_TYPE_NXP_IW612_UART_UART_OFFSET + IMAGE_TYPE_NXP_IW612_SIGNED_ENG_OFFSET,
     /* 0x612A: uartuart_n61x.bin    */
     OTA_UPGRADE_HEADER_IMAGE_TYPE_NXP_IW612_UART_UART_UNSIGNED     = IMAGE_TYPE_NXP_IW612_UART_UART_OFFSET + IMAGE_TYPE_NXP_IW612_UNSIGNED_OFFSET,
     /* 0x612C: uartspi_n61x.bin.se */
     OTA_UPGRADE_HEADER_IMAGE_TYPE_NXP_IW612_UART_SPI_SIGNED_PROD   = IMAGE_TYPE_NXP_IW612_UART_SPI_OFFSET  + IMAGE_TYPE_NXP_IW612_SIGNED_PROD_OFFSET,
     /* 0x612D: uartspi_n61x.bin.se */
     OTA_UPGRADE_HEADER_IMAGE_TYPE_NXP_IW612_UART_SPI_SIGNED_ENG    = IMAGE_TYPE_NXP_IW612_UART_SPI_OFFSET  + IMAGE_TYPE_NXP_IW612_SIGNED_ENG_OFFSET,
     /* 0x612E: uartspi_n61x.bin    */
     OTA_UPGRADE_HEADER_IMAGE_TYPE_NXP_IW612_UART_SPI_UNSIGNED      = IMAGE_TYPE_NXP_IW612_UART_SPI_OFFSET  + IMAGE_TYPE_NXP_IW612_UNSIGNED_OFFSET,
     
};


#endif /* OTA_NXP_DEFINITIONS_H */

