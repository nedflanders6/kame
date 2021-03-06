/*
 * Copyright (c) 1997, 1998 Hellmuth Michaelis. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *---------------------------------------------------------------------------
 *
 *	i4b_ioctl.h - messages kernel <--> userland
 *	-------------------------------------------
 *
 *	$Id: i4b_ioctl.h,v 1.1 1998/12/27 21:46:56 phk Exp $ 
 *
 *      last edit-date: [Tue Dec 22 20:33:46 1998]
 *
 *---------------------------------------------------------------------------*/

#ifndef _I4B_IOCTL_H_
#define _I4B_IOCTL_H_

#if defined(__FreeBSD__) && __FreeBSD__ >= 3
#ifndef _MACHINE_TYPES_H_
#include <machine/types.h>
#endif /* _MACHINE_TYPES_H_ */
#endif /* __FreeBSD__ */

/*---------------------------------------------------------------------------*
 *	version and release number for isdn4bsd package
 *---------------------------------------------------------------------------*/
#define	VERSION		0		/* version number	*/
#define	REL		70		/* release number	*/
#define STEP		00		/* release step		*/

/*---------------------------------------------------------------------------*
 * date/time format in i4b log messages
 * ------------------------------------
 * Being year 2000 clean is not easy with the current state of the
 * ANSI C library standard and it's implementation for some locales.
 * You might like to use the "%c" format of "strftime" sometimes,
 * but this breaks Y2K in some locales. Also the old standard logfile
 * format "%d.%m.%y %H:%M:%S" is non compliant.
 * NetBSD's current toolset warns about this problems, and we compile
 * with -Werror, so this problems need to be resolved.
 *---------------------------------------------------------------------------*/
#define I4B_TIME_FORMAT	"%d.%m.%Y %H:%M:%S"

/*---------------------------------------------------------------------------*
 *	max number of controllers in system
 *---------------------------------------------------------------------------*/
#define	MAX_CONTROLLERS	8		/* max number of controllers	*/

/*---------------------------------------------------------------------------*
 *	controller types
 *---------------------------------------------------------------------------*/
#define CTRL_INVALID	(-1)		/* invalid, error		*/
#define CTRL_UNKNOWN	0		/* unknown controller type	*/
#define CTRL_PASSIVE	1		/* passive ISDN controller cards*/
#define CTRL_DAIC	2		/* Diehl active controller cards*/
#define	CTRL_NUMTYPES	3		/* number of controller types */

/*---------------------------------------------------------------------------*
 *	card types for CTRL_PASSIVE 
 *---------------------------------------------------------------------------*/
#define CARD_TYPEP_INVAL	(-1)	/* invalid, error		*/
#define CARD_TYPEP_UNK		0	/* unknown			*/
#define CARD_TYPEP_8		1	/* Teles, S0/8 			*/
#define CARD_TYPEP_16		2	/* Teles, S0/16			*/
#define CARD_TYPEP_16_3		3	/* Teles, S0/16.3		*/
#define CARD_TYPEP_AVMA1	4	/* AVM A1 or AVM Fritz!Card	*/
#define CARD_TYPEP_163P		5	/* Teles, S0/16.3 PnP		*/
#define CARD_TYPEP_CS0P		6	/* Creatix, S0 PnP		*/
#define CARD_TYPEP_USRTA	7	/* US Robotics ISDN TA internal	*/
#define CARD_TYPEP_DRNNGO	8	/* Dr. Neuhaus Niccy GO@	*/
#define CARD_TYPEP_SWS		9	/* Sedlbauer Win Speed		*/
#define CARD_TYPEP_DYNALINK	10	/* Dynalink IS64PH		*/
#define CARD_TYPEP_BLMASTER	11	/* ISDN Blaster / ISDN Master	*/
#define	CARD_TYPEP_PCFRITZ	12	/* AVM PCMCIA Fritz!Card	*/
#define CARD_TYPEP_ELSAQS1ISA	13	/* ELSA QuickStep 1000pro ISA	*/
#define CARD_TYPEP_ELSAQS1PCI	14	/* ELSA QuickStep 1000pro PCI	*/
#define CARD_TYPEP_SIEMENSITALK	15	/* Siemens I-Talk		*/
#define	CARD_TYPEP_ELSAMLIMC	16	/* ELSA MicroLink ISDN/MC	*/
#define	CARD_TYPEP_ELSAMLMCALL	17	/* ELSA MicroLink MCall		*/
#define	CARD_TYPEP_ITKIX1	18	/* ITK ix1 micro 		*/

/*
 * in case you add support for more cards, please update:
 *
 *	isdnd:		support.c, name_of_controller()
 *	diehl/diehlctl:	main.c, listall()
 *
 * and adjust CARD_TYPEP_MAX below.
 */

#define CARD_TYPEP_MAX		18	/* max type */

/*---------------------------------------------------------------------------*
 *	card types for CTRL_DAIC
 *---------------------------------------------------------------------------*/
#define CARD_TYPEA_DAIC_UNK	0
#define	CARD_TYPEA_DAIC_S	1
#define	CARD_TYPEA_DAIC_SX	2
#define	CARD_TYPEA_DAIC_SCOM	3
#define	CARD_TYPEA_DAIC_QUAD	4

/*---------------------------------------------------------------------------*
 *	max length of some strings
 *---------------------------------------------------------------------------*/
#define TELNO_MAX	41  /* max length of a telephone number (+ '\0')  */
#define DISPLAY_MAX	91  /* max length of display information (+ '\0') */
#define DATETIME_MAX	21  /* max length of datetime information (+ '\0')*/

/*---------------------------------------------------------------------------*
 *	in case the src or dst telephone number is empty
 *---------------------------------------------------------------------------*/
#define TELNO_EMPTY	"NotAvailable"	

/*---------------------------------------------------------------------------*
 *	B channel parameters
 *---------------------------------------------------------------------------*/
#define BCH_MAX_DATALEN	2048	/* max length of a B channel frame */

/*---------------------------------------------------------------------------*
 * userland driver types
 * ---------------------
 * a "driver" is defined here as a piece of software interfacing an 
 * ISDN B channel with a userland service, such as IP, Telephony etc.
 *---------------------------------------------------------------------------*/
#define BDRV_RBCH	0       /* raw b-channel interface driver       */
#define BDRV_TEL	1       /* telephone (speech) interface driver  */
#define BDRV_IPR	2       /* IP over raw HDLC interface driver    */
#define BDRV_ISPPP	3       /* sync Kernel PPP interface driver     */

/*---------------------------------------------------------------------------*
 * B channel protocol
 *---------------------------------------------------------------------------*/
#define BPROT_NONE	0	/* no protocol at all, raw data		*/
#define BPROT_RHDLC	1	/* raw HDLC: flag, data, crc, flag	*/

/*---------------------------------------------------------------------------*
 * causes data type
 *---------------------------------------------------------------------------*/
typedef	unsigned int cause_t;		/* 32 bit unsigned int	*/

/*---------------------------------------------------------------------------*
 * call descriptor id (cdid) definitions
 *---------------------------------------------------------------------------*/
#define CDID_UNUSED	0	/* cdid is invalid and unused		*/
#define CDID_MAX	99999	/* highest valid cdid, wraparound to 1	*/

 
/****************************************************************************

	outgoing call:
	--------------

		userland		kernel
		--------		------

		CDID_REQ ----------------->

	            <------------------ cdid
	
		CONNECT_REQ -------------->

		    <------------------ PROCEEDING_IND (if connect req ok)

	            <------------------ CONNECT_ACTIVE_IND (if connection ok)

		or

	            <------------------ DISCONNECT_IND (if connection failed)
	            
		

	incoming call:
	--------------

		userland		kernel
		--------		------

	            <------------------ CONNECT_IND

		CONNECT_RESP ------------->

	            <------------------ CONNECT_ACTIVE_IND (if accepted)



	active disconnect:
	------------------

		userland		kernel
		--------		------

		DISCONNECT_REQ ------------>

	            <------------------ DISCONNECT_IND
	            

	passive disconnect:
	-------------------

		userland		kernel
		--------		------

	            <------------------ DISCONNECT_IND
	            

****************************************************************************/


/*===========================================================================*
 *===========================================================================*
 *	"read" messages from kernel -> userland
 *===========================================================================* 
 *===========================================================================*/

 
/*---------------------------------------------------------------------------*
 *	message header, included in every message
 *---------------------------------------------------------------------------*/
typedef struct {
	char		type;		/* message identifier		*/
#define MSG_CONNECT_IND		'a'
#define MSG_CONNECT_ACTIVE_IND	'b'
#define MSG_DISCONNECT_IND	'c'
#define MSG_DIALOUT_IND		'd'
#define MSG_IDLE_TIMEOUT_IND	'e'
#define MSG_ACCT_IND		'f'
#define MSG_CHARGING_IND	'g'
#define MSG_PROCEEDING_IND	'h'
#define MSG_ALERT_IND		'i'
#define MSG_DRVRDISC_REQ	'j'
#define MSG_L12STAT_IND		'k'
#define MSG_TEIASG_IND		'l'
#define MSG_PDEACT_IND		'm'
#define	MSG_NEGCOMP_IND		'n'
#define	MSG_IFSTATE_CHANGED_IND	'o'
	int		cdid;		/* call descriptor id		*/
} msg_hdr_t;

/*---------------------------------------------------------------------------*
 *	connect indication
 *		indicates incoming connection
 *---------------------------------------------------------------------------*/
typedef struct {
	msg_hdr_t	header;		/* common header		*/
	int		controller;	/* controller number		*/
	int		channel;	/* channel number		*/
#define  CHAN_B1  0		/* this _must_ be 0, HSCX B1 is also 0	*/
#define  CHAN_B2  1		/* this _must_ be 1, HSCX B2 is also 1	*/
#define  CHAN_ANY (-1)		/* outgoing, not possible for incoming	*/
#define  CHAN_NO  (-2)		/* call waiting (CW) for incoming	*/
	int		bprot;	/* b channel protocot, see BPROT_XXX	*/
	char		dst_telno[TELNO_MAX];	/* destination telno	*/
	char		src_telno[TELNO_MAX];	/* source telno		*/
	int		scr_ind;/* screening indicator			*/
#define  SCR_NONE     0		/* no screening indicator transmitted	*/
#define  SCR_USR_NOSC 1		/* screening user provided, not screened*/
#define  SCR_USR_PASS 2		/* screening user provided, verified & passed */
#define  SCR_USR_FAIL 3		/* screening user provided, verified & failed */
#define  SCR_NET      4		/* screening network provided		*/
	char		display[DISPLAY_MAX];	/* content of display IE*/
} msg_connect_ind_t;

/*---------------------------------------------------------------------------*
 *	connect active indication
 *		indicates active connection
 *---------------------------------------------------------------------------*/
typedef struct {
	msg_hdr_t	header;		/* common header		   */
	int		controller;	/* controller number actually used */
	int		channel;	/* channel number actually used    */
	char		datetime[DATETIME_MAX];	/* content of date/time IE */
} msg_connect_active_ind_t;

/*---------------------------------------------------------------------------*
 *	disconnect indication
 *		indicates a disconnect
 *---------------------------------------------------------------------------*/
typedef struct {
	msg_hdr_t	header;		/* common header	*/
	cause_t		cause;		/* cause code		*/
} msg_disconnect_ind_t;

/*---------------------------------------------------------------------------*
 *	negotiation complete
 *		indicates an interface is completely up & running
 *---------------------------------------------------------------------------*/
typedef struct {
	msg_hdr_t	header;		/* common header	*/
} msg_negcomplete_ind_t;

/*---------------------------------------------------------------------------*
 *	interface changes internal state
 *		indicates an interface has somehow switched its FSM
 *---------------------------------------------------------------------------*/
typedef struct {
	msg_hdr_t	header;		/* common header	*/
	int state;			/* new interface state */
} msg_ifstatechg_ind_t;

/*---------------------------------------------------------------------------*
 *	initiate a call to a remote site
 *		i.e. the IP driver got a packet and wants a connection
 *---------------------------------------------------------------------------*/
typedef struct {
	msg_hdr_t	header;		/* common header	*/
	int		driver;		/* driver type		*/
	int		driver_unit;	/* driver unit number	*/
} msg_dialout_ind_t;

/*---------------------------------------------------------------------------*
 *	idle timeout disconnect sent indication
 *		kernel has sent disconnect request because of b-ch idle
 *---------------------------------------------------------------------------*/
typedef struct {
	msg_hdr_t	header;		/* common header	*/
} msg_idle_timeout_ind_t;

/*---------------------------------------------------------------------------*
 *	accounting information from userland interface driver to daemon
 *---------------------------------------------------------------------------*/
typedef struct {
	msg_hdr_t	header;		/* common header		*/
	int		accttype;	/* accounting type		*/
#define  ACCT_DURING 0
#define  ACCT_FINAL  1
	int		driver;		/* driver type			*/
	int		driver_unit;	/* driver unit number		*/
	int		ioutbytes;	/* ISDN # of bytes sent		*/
	int		iinbytes;	/* ISDN # of bytes received	*/
	int		outbps;		/* bytes per sec out		*/
	int		inbps;		/* bytes per sec in		*/
	int		outbytes;	/* driver # of bytes sent	*/
	int		inbytes;	/* driver # of bytes received	*/
} msg_accounting_ind_t;

/*---------------------------------------------------------------------------*
 *	charging information from isdn driver to daemon
 *---------------------------------------------------------------------------*/
typedef struct {
	msg_hdr_t	header;		/* common header		*/
	int		units;		/* number of units		*/
	int		units_type;	/* type of units info		*/
#define  CHARGE_INVALID	0	/* invalid, unknown */
#define  CHARGE_AOCD	1	/* advice of charge during call */
#define  CHARGE_AOCE	2	/* advice of charge at end of call */
#define  CHARGE_CALC	3	/* locally calculated from rates information */
} msg_charging_ind_t;

/*---------------------------------------------------------------------------*
 *	call proceeding indication
 *		indicates outgoing SETUP has been acknowleged
 *---------------------------------------------------------------------------*/
typedef struct {
	msg_hdr_t	header;		/* common header		   */
	int		controller;	/* controller number actually used */
	int		channel;	/* channel number actually used    */
} msg_proceeding_ind_t;

/*---------------------------------------------------------------------------*
 *	alert indication
 *		indicates remote user side "rings"
 *---------------------------------------------------------------------------*/
typedef struct {
	msg_hdr_t	header;		/* common header		   */
} msg_alert_ind_t;

/*---------------------------------------------------------------------------*
 *	driver requests to disconnect line
 *---------------------------------------------------------------------------*/
typedef struct {
	msg_hdr_t	header;		/* common header	*/
	int		driver;		/* driver type		*/
	int		driver_unit;	/* driver unit number	*/
} msg_drvrdisc_req_t;

/*---------------------------------------------------------------------------*
 *	state of layer 1/2
 *---------------------------------------------------------------------------*/
typedef struct {
	msg_hdr_t	header;		/* common header	*/
	int		controller;	/* controller unit 	*/
	int		layer;		/* layer number (1/2)	*/
#define LAYER_ONE	1
#define LAYER_TWO	2
	int		state;		/* state info		*/
#define LAYER_IDLE	0
#define LAYER_ACTIVE	1
} msg_l12stat_ind_t;

/*---------------------------------------------------------------------------*
 *	TEI assignment messages
 *---------------------------------------------------------------------------*/
typedef struct {
	msg_hdr_t	header;		/* common header	*/
	int		controller;	/* controller unit 	*/
	int		tei;		/* TEI or -1 if invalid */
} msg_teiasg_ind_t;

/*---------------------------------------------------------------------------*
 *	persistent deactivation state of stack
 *---------------------------------------------------------------------------*/
typedef struct {
	msg_hdr_t	header;		/* common header	*/
	int		controller;	/* controller unit 	*/
	int		numactive;	/* number of active connections */
} msg_pdeact_ind_t;


/*===========================================================================*
 *===========================================================================*
 *	"ioctl" messages from userland -> kernel
 *===========================================================================* 
 *===========================================================================*/
 

/*---------------------------------------------------------------------------*
 *	request a unique cdid (to setup an outgoing call)
 *---------------------------------------------------------------------------*/
typedef struct {
	int		cdid;			/* call descriptor id	*/
} msg_cdid_req_t;
 
#define	I4B_CDID_REQ		_IOWR('4', 0, int)

/*---------------------------------------------------------------------------*
 *	connect request
 *		requests an outgoing connection
 *---------------------------------------------------------------------------*/
typedef struct {
	int		cdid;		/* call descriptor id		     */
	int		controller;	/* controller to use		     */
	int		channel;	/* channel to use		     */
	int		txdelay;	/* tx delay after connect	     */
	int		bprot;		/* b channel protocol		     */
	int		driver;		/* driver to route b channel data to */
	int		driver_unit;	/*      unit number for above driver */
	int		unitlen_time;	/* length of a charging unit	     */
	int		idle_time;	/* time without activity on b ch     */
	int		earlyhup_time;	/* safety area at end of unit	     */
	int		unitlen_method;	/* how to calculate the unitlength   */
#define  ULEN_METHOD_STATIC  0	/* use unitlen_time value (see above) */
#define  ULEN_METHOD_DYNAMIC 1	/* use AOCD */	
	char		dst_telno[TELNO_MAX];	/* destination telephone no  */
	char		src_telno[TELNO_MAX];	/* source telephone number   */
} msg_connect_req_t;

#define	I4B_CONNECT_REQ	_IOW('4', 1, msg_connect_req_t)

/*---------------------------------------------------------------------------*
 *	connect response
 *		this is the answer to an incoming connect indication
 *---------------------------------------------------------------------------*/
typedef struct {
	int	cdid;		/* call descriptor id			*/
	int	response;	/* what to do with incoming call	*/
#define  SETUP_RESP_DNTCRE 0	/* dont care, call is not for me	*/
#define  SETUP_RESP_REJECT 1	/* reject call				*/
#define  SETUP_RESP_ACCEPT 2	/* accept call				*/
	cause_t	cause;		/* cause for case SETUP_RESP_REJECT	*/
		/* the following are only used for SETUP_RESP_ACCEPT !! */
	int	txdelay;	/* tx delay after connect		*/
	int	bprot;		/* B chan protocol			*/
	int	driver;		/* driver to route b channel data to	*/
	int	driver_unit;	/*      unit number for above driver	*/
	int	max_idle_time;	/* max time without activity on b ch	*/	
} msg_connect_resp_t;
	
#define	I4B_CONNECT_RESP	_IOW('4', 2, msg_connect_resp_t)

/*---------------------------------------------------------------------------*
 *	disconnect request
 *		active disconnect request
 *---------------------------------------------------------------------------*/
typedef struct {
	int	cdid;		/* call descriptor id			*/
	cause_t	cause;		/* protocol independent cause		*/
} msg_discon_req_t;
	
#define	I4B_DISCONNECT_REQ	_IOW('4', 3, msg_discon_req_t)

/*---------------------------------------------------------------------------*
 *	controller info request
 *---------------------------------------------------------------------------*/
typedef struct {
	int	controller;	/* controller number			*/
	int	ncontroller;	/* number of controllers in system	*/
	int	ctrl_type;	/* controller type passive/active	*/
	int	card_type;	/* brand / version			*/
	int	tei;		/* tei controller probably has		*/
} msg_ctrl_info_req_t;
	
#define	I4B_CTRL_INFO_REQ	_IOWR('4', 4, msg_ctrl_info_req_t)

/*---------------------------------------------------------------------------*
 *	dialout response
 *		status report to driver who requested a dialout
 *---------------------------------------------------------------------------*/
typedef struct {
	int		driver;		/* driver to route b channel data to */
	int		driver_unit;	/*      unit number for above driver */
	int		stat;		/* state of dialout request	     */
#define  DSTAT_NONE	0
#define  DSTAT_TFAIL	1		/* transient failure */
#define  DSTAT_PFAIL	2		/* permanent failure */
#define  DSTAT_INONLY	3		/* no outgoing dials allowed */
} msg_dialout_resp_t;

#define	I4B_DIALOUT_RESP	_IOW('4', 5, msg_dialout_resp_t)
	
/*---------------------------------------------------------------------------*
 *	timeout value update
 *---------------------------------------------------------------------------*/
typedef struct {
	int	cdid;		/* call descriptor id			*/
	int	unitlen_time;	/* length of a charging unit		*/
	int	idle_time;	/* time without activity on b ch	*/
	int	earlyhup_time;	/* safety area at end of unit		*/
} msg_timeout_upd_t;
	
#define	I4B_TIMEOUT_UPD		_IOW('4', 6, msg_timeout_upd_t)

/*---------------------------------------------------------------------------*
 *	soft enable/disable
 *---------------------------------------------------------------------------*/
typedef struct {
	int		driver;		/* driver to route b channel data to */
	int		driver_unit;	/*      unit number for above driver */
	int		updown;		/* what to do			     */
#define  SOFT_ENA	0	/* enable interface */
#define  SOFT_DIS	1	/* disable interface */
} msg_updown_ind_t;

#define	I4B_UPDOWN_IND		_IOW('4', 7, msg_updown_ind_t)

/*---------------------------------------------------------------------------*
 *	send alert request
 *---------------------------------------------------------------------------*/
typedef struct {
	int	cdid;		/* call descriptor id			*/
} msg_alert_req_t;
	
#define	I4B_ALERT_REQ		_IOW('4', 8, msg_alert_req_t)

/*---------------------------------------------------------------------------*
 *	request version and release info from kernel part
 *---------------------------------------------------------------------------*/
typedef struct {
	int	version;	/* version number */
	int	release;	/* release number */
	int	step;		/* release step number */	
} msg_vr_req_t;
 
#define	I4B_VR_REQ		_IOR('4', 9, msg_vr_req_t)

/*---------------------------------------------------------------------------*
 *	Protocol download to active cards
 *---------------------------------------------------------------------------*/
struct isdn_dr_prot {
	size_t bytecount;	/* length of code */
	u_int8_t *microcode;	/* pointer to microcode */
};

struct isdn_download_request {
	int controller;		/* controller number */
	int numprotos;		/* number of protocols in 'protocols' */
	struct isdn_dr_prot *protocols;
};

#define	I4B_CTRL_DOWNLOAD	_IOW('4', 100, struct isdn_download_request)

/*---------------------------------------------------------------------------*
 *	Generic diagnostic interface for active cards
 *---------------------------------------------------------------------------*/
struct isdn_diagnostic_request {
	int controller;		/* controller number */
	u_int32_t cmd;		/* diagnostic command to execute */
	size_t in_param_len;	/* length of additional input parameter */
	void *in_param;		/* optional input parameter */
	size_t out_param_len;	/* available output space */
	void *out_param;	/* output data goes here */
};

#define	I4B_ACTIVE_DIAGNOSTIC	_IOW('4', 102, struct isdn_diagnostic_request)

#endif /* _I4B_IOCTL_H_ */
