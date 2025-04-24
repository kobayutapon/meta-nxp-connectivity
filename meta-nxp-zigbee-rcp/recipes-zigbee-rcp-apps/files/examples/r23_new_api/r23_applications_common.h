/* ZBOSS Zigbee software protocol stack
 *
 * Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
 * www.dsr-zboss.com
 * www.dsr-corporation.com
 * All rights reserved.
 *
 * This is unpublished proprietary source code of DSR Corporation
 * The copyright notice does not evidence any actual or intended
 * publication of such source code.
 *
 * ZBOSS is a registered trademark of Data Storage Research LLC d/b/a DSR
 * Corporation
 *
 * Commercial Usage
 * Licensees holding valid DSR Commercial licenses may use
 * this file in accordance with the DSR Commercial License
 * Agreement provided with the Software or, alternatively, in accordance
 * with the terms contained in a written agreement between you and
 * DSR.
 */
/* PURPOSE: Common definitions for testing Scenes (Server) for HA profile
*/

#ifndef SCENES_TEST_H
#define SCENES_TEST_H 1

#define IEEE_ADDRESS_R23_ZC {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define IEEE_ADDRESS_R22_ZR {0xd1, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11}
#define IEEE_ADDRESS_R23_ZED {0xed, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#define IEEE_ADDRESS_R23_ZR {0xd2, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22}

#define APPLICATION_CHANNEL_PAGE 0U
#define APPLICATION_CHANNEL 21U
#define APPLICATION_CHANNEL_MASK (1l << APPLICATION_CHANNEL)

#define APPLICATION_R22_ZR_INSTALL_CODE "966b9f3ef98ae605 9708"
#define APPLICATION_R23_ZED_PASSCODE {0x52, 0x8F, 0x0A, 0x31}

#endif /* SCENES_TEST_H */
