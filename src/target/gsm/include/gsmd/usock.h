#ifndef _GSMD_USOCK_H
#define _GSMD_USOCK_H

#include <gsmd/event.h>

#define GSMD_UNIX_SOCKET "\0gsmd"
//#define GSMD_UNIX_SOCKET_TYPE SOCK_SEQPACKET
#define GSMD_UNIX_SOCKET_TYPE SOCK_STREAM

#define GSMD_PROTO_VERSION	1

#define GSMD_MSGSIZE_MAX	4096

enum gsmd_msg_type {
	GSMD_MSG_NONE		= 0,
	GSMD_MSG_EVENT		= 1,
	GSMD_MSG_PASSTHROUGH	= 2,
	GSMD_MSG_VOICECALL	= 3,
	GSMD_MSG_DATACALL	= 4,
	GSMD_MSG_PHONEBOOK	= 5,
	GSMD_MSG_NETWORK	= 6,
	GSMD_MSG_PHONE		= 7,
	GSMD_MSG_PIN		= 8,
	GSMD_MSG_SMS		= 9,
	__NUM_GSMD_MSGS
};

enum gsmd_passthrough_type {
	GSMD_PASSTHROUGH_NONE	= 0,
	GSMD_PASSTHROUGH_REQ	= 1,
	GSMD_PASSTHROUGH_RESP	= 2,
};

enum gsmd_msg_voicecall_type {
	GSMD_VOICECALL_DIAL	= 1,
	GSMD_VOICECALL_HANGUP	= 2,
	GSMD_VOICECALL_ANSWER	= 3,
	GSMD_VOICECALL_DTMF	= 4,
	GSMD_VOICECALL_VOL_SET	= 5,
	GSMD_VOICECALL_VOL_GET	= 6,
};

/* Handset / MT related commands */
enum gsmd_msg_phone_type {
	GSMD_PHONE_VOLUME	= 1,
	GSMD_PHONE_VIBRATOR	= 2,
};

enum gsmd_msg_pin_type {
	GSMD_PIN_INPUT		= 1,
};

enum gsmd_msg_phone {
	GSMD_PHONE_POWERUP	= 1,
	GSMD_PHONE_POWERDOWN	= 2,
};

enum gsmd_msg_cb {
	GSMD_CB_SUBSCRIBE	= 1,
	GSMD_CB_UNSUBSCRIBE	= 2,
};

enum gsmd_msg_network {
	GSMD_NETWORK_REGISTER	= 1,
	GSMD_NETWORK_SIGQ_GET	= 2,
	GSMD_NETWORK_VMAIL_GET	= 3,
	GSMD_NETWORK_VMAIL_SET	= 4,
	GSMD_NETWORK_OPER_GET	= 5,
	GSMD_NETWORK_OPER_LIST	= 6,
	GSMD_NETWORK_CIND_GET	= 7,
	GSMD_NETWORK_DEREGISTER	= 8,
};

enum gsmd_msg_sms {
	GSMD_SMS_LIST			= 1,
	GSMD_SMS_READ			= 2,
	GSMD_SMS_SEND			= 3,
	GSMD_SMS_WRITE			= 4,
	GSMD_SMS_DELETE			= 5,
	GSMD_SMS_GET_MSG_STORAGE	= 6,
	GSMD_SMS_SET_MSG_STORAGE	= 7,
	GSMD_SMS_GET_SERVICE_CENTRE	= 8,
	GSMD_SMS_SET_SERVICE_CENTRE	= 9,
};

/* SMS stat from 3GPP TS 07.05, Clause 3.1 */
enum gsmd_msg_sms_type {
	GSMD_SMS_REC_UNREAD	= 0,
	GSMD_SMS_REC_READ	= 1,
	GSMD_SMS_STO_UNSENT	= 2,
	GSMD_SMS_STO_SENT	= 3,
	GSMD_SMS_ALL		= 4,
};

/* SMS format from 3GPP TS 07.05, Clause 3.2.3 */
enum gsmd_msg_sms_fmt {
	GSMD_SMS_FMT_PDU	= 0,
	GSMD_SMS_FMT_TEXT	= 1,
};

/* Data Coding Scheme, refer to GSM 03.38 Clause 4 */
#define B5_COMPRESSED	(1<<5)
#define B4_CLASSMEANING	(1<<4)
enum {
	MESSAGE_CLASS_CLASS0		= 0x00,
	MESSAGE_CLASS_CLASS1		= 0x01,
	MESSAGE_CLASS_CLASS2		= 0x10,
	MESSAGE_CLASS_CLASS3		= 0x11,
};

enum gsmd_sms_alphabet {
	ALPHABET_DEFAULT		= (0x00<<2),
	ALPHABET_8BIT			= (0x01<<2),
	ALPHABET_UCS2			= (0x10<<2),
	ALPHABET_RESERVED		= (0x11<<2),
};

/* Refer to GSM 03.40 subclause 9.2.3.1 */
enum gsmd_sms_tp_mti {
	GSMD_SMS_TP_MTI_DELIVER		= 0,
	GSMD_SMS_TP_MTI_DELIVER_REPORT	= 0,
	GSMD_SMS_TP_MTI_STATUS_REPORT	= 2,
	GSMD_SMS_TP_MTI_COMMAND		= 2,
	GSMD_SMS_TP_MTI_SUBMIT		= 1,
	GSMD_SMS_TP_MTI_SUBMIT_REPORT	= 1,
	GSMD_SMS_TP_MTI_RESERVED	= 3,
};

/* Refer to GSM 03.40 subclause 9.2.3.2, */
/* for SMS-DELIVER, SMS-STATUS-REPORT */
enum gsmd_sms_tp_mms {
	GSMD_SMS_TP_MMS_MORE		= (0<<2),
	GSMD_SMS_TP_MMS_NO_MORE		= (1<<2),
};

/* Refer to GSM 03.40 subclause 9.2.3.3, */
/* for SMS-SUBMIT */
enum gsmd_sms_tp_vpf {
	GSMD_SMS_TP_VPF_NOT_PRESENT	= (0<<3),
	GSMD_SMS_TP_VPF_RESERVED	= (1<<3),
	GSMD_SMS_TP_VPF_RELATIVE	= (2<<3),	
	GSMD_SMS_TP_VPF_ABSOLUTE	= (3<<3),
};

/* Refer to GSM 03.40 subclause 9.2.3.4, */
/* for SMS-DELIVER */
enum gsmd_sms_tp_sri {
	GSMD_SMS_TP_SRI_NOT_RETURN	= (0<<5),
	GSMD_SMS_TP_SRI_STATUS_RETURN	= (1<<5),
};

/* Refer to GSM 03.40 subclause 9.2.3.5, */
/* for SMS-SUBMIT, SMS-COMMAND */
enum gsmd_sms_tp_srr {
	GSMD_SMS_TP_SRR_NOT_REQUEST	= (0<<5),
	GSMD_SMS_TP_SRR_STATUS_REQUEST	= (1<<5),
};

/* Refer to GSM 03.40 subclause 9.2.3.17, */
/* for SMS-SUBMIT, SMS-DELIVER */
enum gsmd_sms_tp_rp {
	GSMD_SMS_TP_RP_NOT_SET		= (0<<7),
	GSMD_SMS_TP_RP_SET		= (1<<7),
};

/* Refer to GSM 03.40 subclause 9.2.3.23 */
/* for SMS-SUBMIT, SMS-DELIVER */
enum gsmd_sms_tp_udhi {
	GSMD_SMS_TP_UDHI_NO_HEADER	= (0<<6),
	GSMD_SMS_TP_UDHI_WITH_HEADER	= (1<<6),
};

/* SMS delflg from 3GPP TS 07.05, Clause 3.5.4 */
enum gsmd_msg_sms_delflg {
	GSMD_SMS_DELFLG_INDEX		= 0,
	GSMD_SMS_DELFLG_READ		= 1,
	GSMD_SMS_DELFLG_READ_SENT	= 2,
	GSMD_SMS_DELFLG_LEAVING_UNREAD	= 3,
	GSMD_SMS_DELFLG_ALL		= 4,
};

enum gsmd_msg_phonebook {
	GSMD_PHONEBOOK_FIND		= 1,
	GSMD_PHONEBOOK_READ		= 2,
	GSMD_PHONEBOOK_READRG		= 3,
	GSMD_PHONEBOOK_WRITE		= 4,
	GSMD_PHONEBOOK_DELETE		= 5,	
	GSMD_PHONEBOOK_GET_SUPPORT	= 6,
};

/* Type-of-Address, Numbering-Plan-Identification field, GSM 03.40, 9.1.2.5 */
enum gsmd_toa_npi {
	GSMD_TOA_NPI_UNKNOWN		= 0x0,
	GSMD_TOA_NPI_ISDN		= 0x1,
	GSMD_TOA_NPI_DATA		= 0x3,
	GSMD_TOA_NPI_TELEX		= 0x4,
	GSMD_TOA_NPI_NATIONAL		= 0x8,
	GSMD_TOA_NPI_PRIVATE		= 0x9,
	GSMD_TOA_NPI_ERMES		= 0xa,
	GSMD_TOA_NPI_RESERVED		= 0xf,
};

/* Type-of-Address, Type-of-Number field, GSM 03.40, Subclause 9.1.2.5 */
enum gsmd_toa_ton {
	GSMD_TOA_TON_UNKNOWN		= (0<<4),
	GSMD_TOA_TON_INTERNATIONAL	= (1<<4),
	GSMD_TOA_TON_NATIONAL		= (2<<4),
	GSMD_TOA_TON_NETWORK		= (3<<4),
	GSMD_TOA_TON_SUBSCRIBER		= (4<<4),
	GSMD_TOA_TON_ALPHANUMERIC	= (5<<4),
	GSMD_TOA_TON_ABBREVIATED	= (6<<4),
	__GSMD_TOA_TON_MASK		= (7<<4),
};

/* Type-of-Address, bit 7 always 1 */
enum gsmd_toa_reserved {
	GSMD_TOA_RESERVED		= (1<<7),
};

/* Length from 3GPP TS 04.08, Clause 10.5.4.7 */

#define GSMD_ADDR_MAXLEN	32
struct gsmd_addr {
	u_int8_t type;
	char number[GSMD_ADDR_MAXLEN+1];
} __attribute__ ((packed));

struct gsmd_dtmf {
	u_int8_t len;
	char dtmf[0];
} __attribute__ ((packed));

struct gsmd_signal_quality {
	u_int8_t rssi;
	u_int8_t ber;
} __attribute__ ((packed));

struct gsmd_voicemail {
	u_int8_t enable;
	struct gsmd_addr addr;
} __attribute__ ((packed));

#define GSMD_PIN_MAXLEN		8
struct gsmd_pin {
	enum gsmd_pin_type type;
	char pin[GSMD_PIN_MAXLEN+1];
	char newpin[GSMD_PIN_MAXLEN+1];
} __attribute__ ((packed));

struct gsmd_evt_auxdata {
	union {
		struct {
			enum gsmd_call_type type;
		} call;
		struct {
			struct gsmd_addr addr;
		} clip;
		struct {
			struct gsmd_addr addr;
		} colp;
		struct {
			u_int8_t memtype;
			int index;
		} sms;
		struct {
			enum gsmd_pin_type type;
		} pin;
		struct {
			enum gsmd_netreg_state state;
			u_int16_t lac;
			u_int16_t ci;
		} netreg;
		struct {
			u_int8_t tz;
		} timezone;
		struct {
			struct gsmd_signal_quality sigq;
		} signal;
		struct {
			enum gsmd_call_progress prog;
			struct gsmd_addr addr;
			u_int8_t ibt:1,
				 tch:1,
				 dir:2;
		} call_status;
		struct {
			u_int16_t flags;
			u_int16_t net_state_gsm;
			u_int16_t net_state_gprs;
		} cipher;
	} u;
} __attribute__((packed));

/* Refer to GSM 07.05 subclause 3.5.4 */
struct gsmd_sms_delete {
	u_int8_t index;
	u_int8_t delflg;	
} __attribute__ ((packed));

/* Refer to GSM 03.40 subclause 9.2.2.2 and GSM 07.05 subclause 4.3 */
#define GSMD_SMS_DATA_MAXLEN	164 
struct gsmd_sms {
	u_int8_t length;	
	u_int8_t coding_scheme;
	int has_header;
	char data[GSMD_SMS_DATA_MAXLEN+1];	
} __attribute__ ((packed));

/* Refer to GSM 03.40 subclause 9.2.2.2 */
struct gsmd_sms_submit {
	struct gsmd_addr addr;
	struct gsmd_sms payload;
};

/* Refer to GSM 07.05 subclause 4.4 */
struct gsmd_sms_write {
	u_int8_t stat;
	struct gsmd_sms_submit sms;
} __attribute__ ((packed));

/* Refer to GSM 03.40 subclause 9.2.2.1 */
struct gsmd_sms_list {
	/* FIXME Defined as in range of location numbers supported by memory */
	u_int8_t index;
	enum gsmd_msg_sms_type stat;
	char time_stamp[7];	
	struct gsmd_addr addr;
	struct gsmd_sms payload;
	int is_last;
};

/* Refer to GSM 07.07 subclause 8.12 */
struct gsmd_phonebook_readrg {
	u_int8_t index1;
	u_int8_t index2;	
} __attribute__ ((packed));

/* Refer to GSM 07.07 subclause 8.14 */
/* FIXME: the nlength and tlength depend on SIM, use +CPBR=? to get */ 
#define	GSMD_PB_NUMB_MAXLEN	44
#define GSMD_PB_TEXT_MAXLEN	14
struct gsmd_phonebook {
	u_int8_t index;
	char numb[GSMD_PB_NUMB_MAXLEN+1];
	u_int8_t type;
	char text[GSMD_PB_TEXT_MAXLEN+1];
} __attribute__ ((packed));


/* Refer to GSM 07.07 subclause 8.13 */
/* FIXME: the tlength depends on SIM, use +CPBR=? to get */ 
struct gsmd_phonebook_find {	
	char findtext[GSMD_PB_TEXT_MAXLEN+1];
} __attribute__ ((packed));

/* Refer to GSM 07.07 subclause 8.12 */
struct gsmd_phonebook_support {	
	u_int8_t index;
	u_int8_t nlength;
	u_int8_t tlength;
} __attribute__ ((packed));

/* Operator status from 3GPP TS 07.07, Clause 7.3 */
enum gsmd_oper_status {
	GSMD_OPER_UNKNOWN,
	GSMD_OPER_AVAILABLE,
	GSMD_OPER_CURRENT,
	GSMD_OPER_FORBIDDEN,
};

/* Theoretically numeric operator code is five digits long but some
 * operators apparently use six digit codes.  */
typedef char gsmd_oper_numeric[6];

struct gsmd_msg_oper {
	enum gsmd_oper_status stat;
	int is_last;
	char opname_longalpha[16];
	char opname_shortalpha[8];
	gsmd_oper_numeric opname_num;
};

struct gsmd_msg_hdr {
	u_int8_t version;
	u_int8_t msg_type;
	u_int8_t msg_subtype;
	u_int8_t _pad;
	u_int16_t id;
	u_int16_t len;
	u_int8_t data[];
} __attribute__((packed));

#ifdef __GSMD__

#include <common/linux_list.h>

#include <gsmd/usock.h>
#include <gsmd/gsmd.h>

struct gsmd_user;

struct gsmd_ucmd {
	struct llist_head list;
	struct gsmd_msg_hdr hdr;
	char buf[];
} __attribute__ ((packed));

extern struct gsmd_ucmd *ucmd_alloc(int extra_size);
extern int usock_init(struct gsmd *g);
extern void usock_cmd_enqueue(struct gsmd_ucmd *ucmd, struct gsmd_user *gu);
extern struct gsmd_ucmd *usock_build_event(u_int8_t type, u_int8_t subtype, u_int8_t len);
extern int usock_evt_send(struct gsmd *gsmd, struct gsmd_ucmd *ucmd, u_int32_t evt);

#endif /* __GSMD__ */

#endif

