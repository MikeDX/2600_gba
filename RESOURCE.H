/*
  The data is held here.
*/

extern struct resource {
	int rr;
	int debug;
	char bank;
	char pad_sens;
	char sc;
	char autoconfig;
	char left;
	char right;
	char swap;
} app_data;

#define STICK 	0x01
#define PADDLE 	0x02
#define KEYPAD  0x03
