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
/* PURPOSE: Common definitions for TCSO sample
*/

#ifndef TCSO_TEST_H
#define TCSO_TEST_H 1

#define IEEE_ADDRESS_ZC {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define IEEE_ADDRESS_ZC_SWAPPED {0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab}
#define IEEE_ADDRESS_ZED {0xed, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

#define APPLICATION_CHANNEL_PAGE 0U
#define APPLICATION_CHANNEL 21U
#define APPLICATION_CHANNEL_MASK (1l << APPLICATION_CHANNEL)

#define APPLICATION_ZED_INSTALL_CODE  "966b9f3ef98ae605 9708"

#endif /* TCSO_TEST_H */
