#ifndef _SYSLOGPROT_H_
#define _SYSLOGPROT_H_

#include <AppKit.h>


class SyslogProt {
public:
	SyslogProt(const char *ip_address);
	~SyslogProt();
	
	
	status_t	SendSyslogMessage(char facility, char severity, const char *content);
	// reads ip address from /boot/home/config/settings/syslog.ini
	static status_t	Syslog(char facility, char severity, const char *format, ...);
	// dont use the following function for devices. only for relaying!
	static status_t	SendSyslogMessage(const char *ip_address, const char *datagram);
	void		EnableOutput(bool state) {output=state;};
	
private:

	int		sockethandle;
	char		ipaddress[16];
	bool		output;
};

#endif
