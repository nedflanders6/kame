/*	$NetBSD: hd64570reg.h,v 1.2 1998/10/28 16:26:01 kleink Exp $	*/

/*
 * Copyright (c) 1998 Vixie Enterprises
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of Vixie Enterprises nor the names
 *    of its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY VIXIE ENTERPRISES AND
 * CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL VIXIE ENTERPRISES OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This software has been written for Vixie Enterprises by Michael Graff
 * <explorer@flame.org>.  To learn more about Vixie Enterprises, see
 * ``http://www.vix.com''.
 */

#ifndef _DEV_IC_HD64570REG_H_
#define _DEV_IC_HD64570REG_H_

/* XXX
 * This is really HDLC specific stuff, but...
 */
#define CISCO_MULTICAST         0x8f    /* Cisco multicast address */
#define CISCO_UNICAST           0x0f    /* Cisco unicast address */
#define CISCO_KEEPALIVE         0x8035  /* Cisco keepalive protocol */
#define CISCO_ADDR_REQ          0       /* Cisco address request */
#define CISCO_ADDR_REPLY        1       /* Cisco address reply */
#define CISCO_KEEPALIVE_REQ     2       /* Cisco keepalive request */

typedef struct cisco_pkt {
        u_int32_t	type;
        u_int32_t	par1;
        u_int32_t	par2;
        u_int16_t	rel;
        u_int16_t	time0;
        u_int16_t	time1;
} cisco_pkt_t;
#define CISCO_PKT_LEN	18	/* sizeof doesn't work right... */

#define HDLC_PROTOCOL_IP	0x0800	/* IP */

typedef struct hdlc_header {
	u_int8_t	addr;
	u_int8_t	control;
	u_int16_t	protocol;
} hdlc_header_t;
#define HDLC_HDRLEN	4

/*
 * Hitachi HD64570  defininitions 
 */

/*
 * At least one implementation uses a somewhat strange register address
 * mapping.  If a card doesn't, define this to be a pass-through
 * macro.  (The ntwo driver needs this...)
 */
#ifndef SCADDR
#define SCADDR(y)  (((y) & 0x0002) ? (((y) & 0x00fd) + 0x100) : (y))
#endif

/*  SCA Control Registers  */
#define  SCA_PABR0 2
#define  SCA_PABR1 3
#define  SCA_WCRL  4        /* Wait Control reg */
#define  SCA_WCRM  5        /* Wait Control reg */
#define  SCA_WCRH  6        /* Wait Control reg */
#define  SCA_PCR   8        /* DMA priority control reg */

/*   Interrupt registers  */
#define  SCA_ISR0   0x10    /* Interrupt status register 0  */
#define  SCA_ISR1   0x11    /* Interrupt status register 1  */
#define  SCA_ISR2   0x12    /* Interrupt status register 2  */
#define  SCA_IER0   0x14    /* Interrupt enable register 0  */
#define  SCA_IER1   0x15    /* Interrupt enable register 1  */
#define  SCA_IER2   0x16    /* Interrupt enable register 2  */
#define  SCA_ITCR   0x18    /* interrupt control register */
#define  SCA_IVR    0x1a    /* interrupt vector */
#define  SCA_IMVR   0x1c    /* modified interrupt vector */

/*  MSCI  Channel 0 Registers  */
#define  SCA_TRBL0  0x20    /* TX/RX buffer reg */ 
#define  SCA_TRBH0  0x21    /* TX/RX buffer reg */ 
#define  SCA_ST00   0x22     /* Status reg 0 */
#define  SCA_ST10   0x23     /* Status reg 1 */
#define  SCA_ST20   0x24     /* Status reg 2 */
#define  SCA_ST30   0x25     /* Status reg 3 */
#define  SCA_FST0   0x26     /* frame Status reg  */
#define  SCA_IE00   0x28     /* Interrupt enable reg 0 */
#define  SCA_IE10   0x29     /* Interrupt enable reg 1 */
#define  SCA_IE20   0x2a     /* Interrupt enable reg 2 */
#define  SCA_FIE0   0x2b     /* Frame Interrupt enable reg  */
#define  SCA_CMD0   0x2c     /* Command reg */
#define  SCA_MD00   0x2e     /* Mode reg 0 */
#define  SCA_MD10   0x2f     /* Mode reg 1 */
#define  SCA_MD20   0x30     /* Mode reg 2 */
#define  SCA_CTL0   0x31     /* Control reg */
#define  SCA_SA00   0x32     /* Syn Address reg 0 */
#define  SCA_SA10   0x33     /* Syn Address reg 1 */
#define  SCA_IDL0   0x34    /* Idle register */
#define  SCA_TMC0   0x35     /* Time constant */
#define  SCA_RXS0   0x36     /* RX clock source */
#define  SCA_TXS0   0x37     /* TX clock source */
#define  SCA_TRC00  0x38    /* TX Ready control reg 0 */ 
#define  SCA_TRC10  0x39    /* TX Ready control reg 1 */ 
#define  SCA_RRC0   0x3A    /* RX Ready control reg */ 

/*  MSCI  Channel 1 Registers  */
#define  SCA_TRBL1  0x40    /* TX/RX buffer reg */ 
#define  SCA_TRBH1  0x41    /* TX/RX buffer reg */ 
#define  SCA_ST01   0x42     /* Status reg 0 */
#define  SCA_ST11   0x43     /* Status reg 1 */
#define  SCA_ST21   0x44     /* Status reg 2 */
#define  SCA_ST31   0x45     /* Status reg 3 */
#define  SCA_FST1   0x46     /* Frame Status reg  */
#define  SCA_IE01   0x48     /* Interrupt enable reg 0 */
#define  SCA_IE11   0x49     /* Interrupt enable reg 1 */
#define  SCA_IE21   0x4a     /* Interrupt enable reg 2 */
#define  SCA_FIE1   0x4b     /* Frame Interrupt enable reg  */
#define  SCA_CMD1   0x4c     /* Command reg */
#define  SCA_MD01   0x4e     /* Mode reg 0 */
#define  SCA_MD11   0x4f     /* Mode reg 1 */
#define  SCA_MD21   0x50     /* Mode reg 2 */
#define  SCA_CTL1   0x51     /* Control reg */
#define  SCA_SA01   0x52     /* Syn Address reg 0 */
#define  SCA_SA11   0x53     /* Syn Address reg 1 */
#define  SCA_IDL1   0x54    /* Idle register */
#define  SCA_TMC1   0x55     /* Time constant */
#define  SCA_RXS1   0x56     /* RX clock source */
#define  SCA_TXS1   0x57     /* TX clock source */
#define  SCA_TRC01  0x58    /* TX Ready control reg 0 */ 
#define  SCA_TRC11  0x59    /* TX Ready control reg 1 */ 
#define  SCA_RRC1   0x5A    /* RX Ready control reg */ 


/*  SCA  DMA  registers  */

#define  SCA_DMER   0x9     /* DMA Master Enable reg */

/*   DMA   Channel 0   Registers (MSCI -> memory, or rx) */
#define  SCA_BARL0  0x80    /* buffer address reg  */
#define  SCA_BARH0  0x81    /* buffer address reg  */
#define  SCA_BARB0  0x82    /* buffer address reg  */
#define  SCA_DARL0  0x80    /* Dest. address reg  */
#define  SCA_DARH0  0x81    /* Dest. address reg  */
#define  SCA_DARB0  0x82    /* Dest. address reg  */
#define  SCA_CPB0   0x86    /* Chain pointer base  */
#define  SCA_CDAL0  0x88    /* Current descriptor address  */
#define  SCA_CDAH0  0x89    /* Current descriptor address  */
#define  SCA_EDAL0  0x8A    /* Error descriptor address  */
#define  SCA_EDAH0  0x8B    /* Error descriptor address  */
#define  SCA_BFLL0  0x8C    /* RX buffer length Low  */
#define  SCA_BFLH0  0x8D    /* RX buffer length High */
#define  SCA_BCRL0  0x8E    /* Byte Count reg  */
#define  SCA_BCRH0  0x8F    /* Byte Count reg  */
#define  SCA_DSR0   0x90    /* DMA Status reg  */
#define  SCA_DMR0   0x91    /* DMA Mode reg    */
#define  SCA_FCT0   0x93    /* Frame end interrupt Counter */
#define  SCA_DIR0   0x94    /* DMA interrupt enable */
#define  SCA_DCR0   0x95    /* DMA Command reg  */

/*   DMA  Channel 1   Registers (memory -> MSCI, or tx) */
#define  SCA_BARL1  0xA0    /* buffer address reg  */
#define  SCA_BARH1  0xA1    /* buffer address reg  */
#define  SCA_BARB1  0xA2    /* buffer address reg  */
#define  SCA_SARL1  0xA4    /* Source address reg  */
#define  SCA_SARH1  0xA5    /* Source address reg  */
#define  SCA_SARB1  0xA6    /* Source address reg  */
#define  SCA_CPB1   0xA6    /* Chain pointer base  */
#define  SCA_CDAL1  0xA8    /* Current descriptor address  */
#define  SCA_CDAH1  0xA9    /* Current descriptor address  */
#define  SCA_EDAL1  0xAA    /* Error descriptor address  */
#define  SCA_EDAH1  0xAB    /* Error descriptor address  */
#define  SCA_BCRL1  0xAE    /* Byte Count reg  */
#define  SCA_BCRH1  0xAF    /* Byte Count reg  */
#define  SCA_DSR1   0xB0    /* DMA Status reg  */
#define  SCA_DMR1   0xB1    /* DMA Mode reg    */
#define  SCA_FCT1   0xB3    /* Frame end interrupt Counter */
#define  SCA_DIR1   0xB4    /* DMA interrupt enable */
#define  SCA_DCR1   0xB5    /* DMA Command reg  */

/*   DMA   Channel 2   Registers (MSCI -> memory) */
#define  SCA_BARL2  0xC0    /* buffer address reg  */
#define  SCA_BARH2  0xC1    /* buffer address reg  */
#define  SCA_BARB2  0xC2    /* buffer address reg  */
#define  SCA_DSR2   0xD0    /* DMA Status reg  */

/*   DMA   Channel 3   Registers (memory -> MSCI) */
#define  SCA_BARL3  0xE0    /* buffer address reg  */
#define  SCA_BARH3  0xE1    /* buffer address reg  */
#define  SCA_BARB3  0xE2    /* buffer address reg  */
#define  SCA_DSR3   0xF0    /* DMA Status reg  */

/*
 * SCA HD64570 Register Definitions
 */

#define ST3_CTS   8    /* modem input  /CTS bit */
#define ST3_DCD   4    /* modem input  /DCD bit */

/*
 * SCA commands
 */
#define SCA_CMD_TXRESET         0x01
#define SCA_CMD_TXENABLE        0x02
#define SCA_CMD_TXDISABLE       0x03
#define SCA_CMD_TXCRCINIT       0x04
#define SCA_CMD_TXCRCEXCL       0x05
#define SCA_CMS_TXEOM           0x06
#define SCA_CMD_TXABORT         0x07
#define SCA_CMD_MPON            0x08
#define SCA_CMD_TXBCLEAR        0x09

#define SCA_CMD_RXRESET         0x11
#define SCA_CMD_RXENABLE        0x12
#define SCA_CMD_RXDISABLE       0x13
#define SCA_CMD_RXCRCINIT       0x14
#define SCA_CMD_RXMSGREJ        0x15
#define SCA_CMD_MPSEARCH        0x16
#define SCA_CMD_RXCRCEXCL       0x17
#define SCA_CMD_RXCRCCALC       0x18

#define SCA_CMD_NOP             0x00
#define SCA_CMD_RESET           0x21
#define SCA_CMD_SEARCH          0x31

#define SCA_MD0_CRC_1           0x01
#define SCA_MD0_CRC_CCITT       0x02
#define SCA_MD0_CRC_ENABLE      0x04
#define SCA_MD0_AUTO_ENABLE     0x10
#define SCA_MD0_MODE_ASYNC      0x00
#define SCA_MD0_MODE_BYTESYNC1  0x20
#define SCA_MD0_MODE_BISYNC     0x40
#define SCA_MD0_MODE_BYTESYNC2  0x60
#define SCA_MD0_MODE_HDLC       0x80

#define SCA_MD1_NOADDRCHK       0x00
#define SCA_MD1_SNGLADDR1       0x40
#define SCA_MD1_SNGLADDR2       0x80
#define SCA_MD1_DUALADDR        0xC0

#define SCA_MD2_DUPLEX          0x00
#define SCA_MD2_ECHO            0x01
#define SCA_MD2_LOOPBACK        0x03
#define SCA_MD2_ADPLLx8         0x00
#define SCA_MD2_ADPLLx16        0x08
#define SCA_MD2_ADPLLx32        0x10
#define SCA_MD2_NRZ             0x00
#define SCA_MD2_NRZI            0x20
#define SCA_MD2_MANCHESTER      0x80
#define SCA_MD2_FM0             0xC0
#define SCA_MD2_FM1             0xA0

#define SCA_CTL_RTS             0x01
#define SCA_CTL_IDLPAT          0x10
#define SCA_CTL_UDRNC           0x20

#define SCA_RXS_DIV_MASK        0x0F
#define SCA_RXS_DIV1            0x00
#define SCA_RXS_DIV2            0x01
#define SCA_RXS_DIV4            0x02
#define SCA_RXS_DIV8            0x03
#define SCA_RXS_DIV16           0x04
#define SCA_RXS_DIV32           0x05
#define SCA_RXS_DIV64           0x06
#define SCA_RXS_DIV128          0x07
#define SCA_RXS_DIV256          0x08
#define SCA_RXS_DIV512          0x09
#define SCA_RXS_CLK_RXC0        0x00
#define SCA_RXS_CLK_RXC1        0x20
#define SCA_RXS_CLK_INT         0x40
#define SCA_RXS_CLK_ADPLL_OUT   0x60
#define SCA_RXS_CLK_ADPLL_IN    0x70

#define SCA_TXS_DIV_MASK        0x0F
#define SCA_TXS_DIV1            0x00
#define SCA_TXS_DIV2            0x01
#define SCA_TXS_DIV4            0x02
#define SCA_TXS_DIV8            0x03
#define SCA_TXS_DIV16           0x04
#define SCA_TXS_DIV32           0x05
#define SCA_TXS_DIV64           0x06
#define SCA_TXS_DIV128          0x07
#define SCA_TXS_DIV256          0x08
#define SCA_TXS_DIV512          0x09
#define SCA_TXS_CLK_TXC         0x00
#define SCA_TXS_CLK_INT         0x40
#define SCA_TXS_CLK_RX          0x60

#define SCA_ST0_RXRDY           0x01
#define SCA_ST0_TXRDY           0x02
#define SCA_ST0_RXINT           0x40
#define SCA_ST0_TXINT           0x80

#define SCA_ST1_IDLST           0x01
#define SCA_ST1_ABTST           0x02
#define SCA_ST1_DCDCHG          0x04
#define SCA_ST1_CTSCHG          0x08
#define SCA_ST1_FLAG            0x10
#define SCA_ST1_TXIDL           0x40
#define SCA_ST1_UDRN            0x80

/* ST2 and FST look the same */
#define SCA_FST_CRCERR          0x04
#define SCA_FST_OVRN            0x08
#define SCA_FST_RESFRM          0x10
#define SCA_FST_ABRT            0x20
#define SCA_FST_SHRT            0x40
#define SCA_FST_EOM             0x80

#define SCA_ST3_RXENA           0x01
#define SCA_ST3_TXENA           0x02
#define SCA_ST3_DCD             0x04
#define SCA_ST3_CTS             0x08
#define SCA_ST3_ADPLLSRCH       0x10
#define SCA_ST3_TXDATA          0x20

#define SCA_FIE_EOMFE           0x80

#define SCA_IE0_RXRDY           0x01
#define SCA_IE0_TXRDY           0x02
#define SCA_IE0_RXINT           0x40
#define SCA_IE0_TXINT           0x80

#define SCA_IE1_IDLDE           0x01
#define SCA_IE1_ABTDE           0x02
#define SCA_IE1_DCD             0x04
#define SCA_IE1_CTS             0x08
#define SCA_IE1_FLAG            0x10
#define SCA_IE1_IDL             0x40
#define SCA_IE1_UDRN            0x80

#define SCA_IE2_CRCERR          0x04
#define SCA_IE2_OVRN            0x08
#define SCA_IE2_RESFRM          0x10
#define SCA_IE2_ABRT            0x20
#define SCA_IE2_SHRT            0x40
#define SCA_IE2_EOM             0x80

/* This is for RRC, TRC0 and TRC1. */
#define SCA_RCR_MASK            0x1F

#define SCA_IE1_

#define SCA_IV_CHAN0            0x00
#define SCA_IV_CHAN1            0x20

#define SCA_IV_RXRDY            0x04
#define SCA_IV_TXRDY            0x06
#define SCA_IV_RXINT            0x08
#define SCA_IV_TXINT            0x0A

#define SCA_IV_DMACH0           0x00
#define SCA_IV_DMACH1           0x08
#define SCA_IV_DMACH2           0x20
#define SCA_IV_DMACH3           0x28

#define SCA_IV_DMIA             0x14
#define SCA_IV_DMIB             0x16

#define SCA_IV_TIMER0           0x1C
#define SCA_IV_TIMER1           0x1E
#define SCA_IV_TIMER2           0x3C
#define SCA_IV_TIMER3           0x3E

/*
 * DMA registers
 */
#define SCA_DSR_EOT             0x80
#define SCA_DSR_EOM             0x40
#define SCA_DSR_BOF             0x20
#define SCA_DSR_COF             0x10
#define SCA_DSR_DE              0x02
#define SCA_DSR_DEWD            0x01	/* write DISABLE DE bit */

#define SCA_DMR_TMOD            0x10
#define SCA_DMR_NF              0x04
#define SCA_DMR_CNTE            0x02

#define SCA_DMER_EN             0x80

#define SCA_DCR_ABRT            0x01
#define SCA_DCR_FCCLR           0x02  /* Clear frame end intr counter */

#define SCA_DIR_EOT             0x80
#define SCA_DIR_EOM             0x40
#define SCA_DIR_BOF             0x20
#define SCA_DIR_COF             0x10

#define SCA_PCR_BRC             0x10
#define SCA_PCR_CCC             0x08
#define SCA_PCR_PR2             0x04
#define SCA_PCR_PR1             0x02
#define SCA_PCR_PR0             0x01

/* 
 * Descriptor Status byte bit definitions:
 *
 *  Bit    Receive Status            Transmit Status
 * -------------------------------------------------
 *   7         EOM                       EOM
 *   6         Short Frame               ...
 *   5         Abort                     ...
 *   4         Residual bit              ...
 *   3         Overrun                   ...
 *   2         CRC                       ...
 *   1         ...                       ...
 *   0         ...                       EOT
 * -------------------------------------------------
 */

#define  ST_EOM    0x80    /* End of frame  */
#define  ST_SHRT   0x40    /* Short frame  */
#define  ST_ABT    0x20    /* Abort detected */
#define  ST_RBIT   0x10    /* Residual bit detected */
#define  ST_OVRN   0x8     /* Overrun error */
#define  ST_CRCE   0x4     /* CRC Error */
#define  ST_OVFL   0x1     /* Buffer OverFlow error  (software defined) */

#define  ST_EOT      1     /* End of transmit command */
                   

/*  DMA  Status register (DSR)  bit definitions  */
#define  DSR_EOT  0x80      /* end of transfer EOT bit */
#define  DSR_EOM  0x40      /* end of frame EOM bit */
#define  DSR_BOF  0x20      /* buffer overflow BOF bit */
#define  DSR_COF  0x10      /* counter overflow  COF bit */
#define  DSR_DWE     1      /* write disable DWE bit */

/*  MSCI Status register 0 bits  */

#define  RXRDY_BIT  1       /* RX ready */
#define  TXRDY_BIT  2       /* TX ready */

#define ST3_CTS   8    /* modem input  /CTS bit */
#define ST3_DCD   4    /* modem input  /DCD bit */


/*  TX and RX Clock Source  */
#define CLK_LINE	0x00	/* TX/RX line input */
#define CLK_BRG		0x40	/* internal baud rate generator */
#define CLK_RXC		0x60	/* receive clock */

/*   Clocking options  */                                        
#define  CLK_INT   0        /* Internal - Baud Rate generator output */
#define  CLK_EXT   1        /* External - both clocks */
#define  CLK_RXCI  2        /* External - Receive Clock only */
#define  CLK_EETC  3        /* EETC clock:  TX = int. / RX = ext.*/

#define SCA_DMAC_OFF_0		0x00	/* offset of DMAC for port 0 */
#define SCA_DMAC_OFF_1		0x40	/* offset of DMAC for port 1 */
#define SCA_MSCI_OFF_0		0x00	/* offset of MSCI for port 0 */
#define SCA_MSCI_OFF_1		0x20	/* offset of MSCI for port 1 */

/*
 * DMA constraints
 */
#define SCA_DMA_ALIGNMENT	(64 * 1024)	/* 64 KB alignment */
#define SCA_DMA_BOUNDRY		(16 * 1024 * 1024)	/* 16 MB region */

#endif /* _DEV_IC_HD64570REG_H_ */
