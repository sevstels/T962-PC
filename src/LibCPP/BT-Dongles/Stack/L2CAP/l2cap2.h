#ifndef _L2CAP2_H_
#define	_L2CAP2_H_

enum L2CAP_SIGNALING_COMMAND_CODES {
	L2CAP_COMMAND_REJECT = 0x01,
	L2CAP_CONNECTION_REQUEST = 0x02,
	L2CAP_CONNECTION_RESPONSE = 0x03,
	L2CAP_CONFIGURATION_REQUEST = 0x04,
	L2CAP_CONFIGURATION_RESPONSE = 0x05,
	L2CAP_DISCONNECTION_REQUEST = 0x06,
	L2CAP_DISCONNECTION_RESPONSE = 0x07,
	L2CAP_ECHO_REQUEST = 0x08,
	L2CAP_ECHO_RESPONSE = 0x09,
	L2CAP_INFORMATION_REQUEST = 0x0A,
	L2CAP_INFORMATION_RESPONSE = 0x0B,
	L2CAP_CREATE_CHANNEL_REQUEST = 0x0C,
	L2CAP_CREATE_CHANNEL_RESPONSE = 0x0D,
	L2CAP_MOVE_CHANNEL_REQUEST = 0x0E,
	L2CAP_MOVE_CHANNEL_RESPONSE = 0x0F,
	L2CAP_MOVE_CHANNEL_CONFIRMATION = 0x10,
	L2CAP_MOVE_CHANNEL_CONFIRMATION_RESPONSE = 0x11,
	L2CAP_CONNECTION_PARAMETER_UPDATE_REQUEST = 0x12,
	L2CAP_CONNECTION_PARAMETER_UPDATE_RESPONSE = 0x13,
	L2CAP_LE_CREDIT_BASED_CONNECTION_REQUEST = 0x14,
	L2CAP_LE_CREDIT_BASED_CONNECTION_RESPONSE = 0x15,
	L2CAP_LE_FLOW_CONTROL_CREDIT = 0x16
};

#pragma pack(push,1)
struct l2cap_header {
  unsigned short length;
  unsigned short channel;
  unsigned char data[];
};

struct l2cap_command {
	unsigned char code;
	unsigned char identifier;
	unsigned short length;
	unsigned char data[];
};

struct l2cap_connection_request {
	unsigned short psm;
	unsigned short source_cid;
} );

struct l2cap_connection_response {
	unsigned short dest_cid;
	unsigned short source_cid;
	unsigned short result;
	unsigned short status;
} ;

struct l2cap_config_request {
	unsigned short dest_cid;
	unsigned short flags;
	unsigned char config[4];
};

struct l2cap_config_response {
	unsigned short source_cid;
	unsigned short flags;
	unsigned short result;
	unsigned char config[8];
};

struct l2cap_disconnection_request {
	unsigned short dest_cid;
	unsigned short source_cid;
} ;

struct l2cap_disconnection_response {
	unsigned short dest_cid;
	unsigned short source_cid;
} ;

struct l2cap_connection {
	bool active;
	unsigned short cid;
	unsigned short host_cid;
	unsigned short psm;
	void (*recv_data)(unsigned short hci_handle, unsigned char *, int len);
	int (*get_data)(unsigned short hci_handle, unsigned char *);
};
#pragma pack(pop)

#define NUM_L2CAP_CHANNELS   4
#define L2CAP_CMD_QUEUE_SIZE 8

void l2cap_init_connections(struct l2cap_connection connections[]);
void l2cap_init_counters();
void l2cap_queue_cmd(unsigned short hci_handle, unsigned char cmd, unsigned short cid, unsigned short id);
void l2cap_request_connection(unsigned short hci_handle, unsigned short psm);

void l2cap_recv_command(unsigned short hci_handle, unsigned char * buf, int len);
int  l2cap_get_command(unsigned short hci_handle, unsigned char * buf);

void l2cap_recv_data(unsigned short hci_handle, unsigned char * buf, int len);
int  l2cap_get_data(unsigned short hci_handle, unsigned char * buf);

#endif	/* L2CAP_H */
