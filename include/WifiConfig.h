#pragma once

#include <Arduino.h>
#include <WiFi.h>

class WiFiConfig {

  public:

	enum EapTypes {
		WPA2_ENTERPRISE_OFF = -1,
		WPA2_ENTERPRISE_TLS = WPA2_AUTH_TLS,
		WPA2_ENTERPRISE_PEAP = WPA2_AUTH_PEAP,
		WPA2_ENTERPRISE_TTLS = WPA2_AUTH_TTLS
	};

	enum EapPhase2Types {
		EAP_TTLS_PHASE2_OFF = -1,
		EAP_TTLS_PHASE2_EAP = 0,
		EAP_TTLS_PHASE2_MSCHAPV2 = 1,
		EAP_TTLS_PHASE2_MSCHAP = 2,
		EAP_TTLS_PHASE2_PAP = 3,
		EAP_TTLS_PHASE2_CHAP = 4
	};

  protected:

	String ssid;
	String password;

	//eap fields
	EapTypes method = WPA2_ENTERPRISE_OFF;               // tls/peap/ttls
	EapPhase2Types ttlsPhase2Type = EAP_TTLS_PHASE2_OFF; // 0 - eap, 1 - mschapv2, 2 - mschap, 3 - pap, 4 - chap
	String identity;
	String username;
	String caCert;
	String clientCert;
	String clientKey;

	//static IP config
	IPAddress ip; //if this is 0 then dhcp is used
	IPAddress gateway;
	IPAddress subnet;
	IPAddress dns1;
	IPAddress dns2;

  public:

	String getSSID() const { return ssid; }
	String getPassword() const { return password; }

	EapTypes getEapMethod() const { return method; }
	EapPhase2Types getEapPhase2Method() const { return ttlsPhase2Type; }
	String getEapIdentity() const { return identity; }
	String getEapUsername() const { return username; }
	String getEapCaCert() const { return caCert; }
	String getEapClientCert() const { return clientCert; }
	String getEapClientKey() const { return clientKey; }

	IPAddress getIP() const { return ip; }
	IPAddress getGateway() const { return gateway; }
	IPAddress getSubnet() const { return subnet; }
	IPAddress getDNS1() const { return dns1; }
	IPAddress getDNS2() const { return dns2; }

	bool isConfigured() const { return !ssid.isEmpty(); }
	bool isStaticIP() const { return ip && gateway && subnet; }
	bool isEAP() const { return method != WPA2_ENTERPRISE_OFF; }
};