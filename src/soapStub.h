/* soapStub.h
   Generated by gSOAP 2.8.14 from soap1.h

Copyright(C) 2000-2013, Robert van Engelen, Genivia Inc. All Rights Reserved.
The generated code is released under ONE of the following licenses:
GPL or Genivia's license for commercial use.
This program is released under the GPL with the additional exemption that
compiling, linking, and/or using OpenSSL is allowed.
*/

#ifndef soapStub_H
#define soapStub_H
#include <vector>
#define SOAP_NAMESPACE_OF_ns21	"http://server.ws.util.dom.dadi.com/"
#include "stdsoap2.h"
#if GSOAP_VERSION != 20814
# error "GSOAP VERSION MISMATCH IN GENERATED CODE: PLEASE REINSTALL PACKAGE"
#endif


/******************************************************************************\
 *                                                                            *
 * Enumerations                                                               *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Types with Custom Serializers                                              *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Classes and Structs                                                        *
 *                                                                            *
\******************************************************************************/


#if 0 /* volatile type: do not declare here, declared elsewhere */

#endif

#if 0 /* volatile type: do not declare here, declared elsewhere */

#endif

#ifndef SOAP_TYPE_ns21__SendinfoTask
#define SOAP_TYPE_ns21__SendinfoTask (8)
/* ns21:SendinfoTask */
class SOAP_CMAC ns21__SendinfoTask
{
public:
	std::string nodeCode;	/* required element of type xsd:string */
	std::string nodeName;	/* required element of type xsd:string */
	std::string dateLine;	/* required element of type xsd:string */
	std::string finishTime;	/* required element of type xsd:string */
	std::string taskState;	/* required element of type xsd:string */
	std::string taskRate;	/* required element of type xsd:string */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 8; } /* = unique id SOAP_TYPE_ns21__SendinfoTask */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         ns21__SendinfoTask() { ns21__SendinfoTask::soap_default(NULL); }
	virtual ~ns21__SendinfoTask() { }
};
#endif

#ifndef SOAP_TYPE_ns21__PackInfo
#define SOAP_TYPE_ns21__PackInfo (9)
/* ns21:PackInfo */
class SOAP_CMAC ns21__PackInfo
{
public:
	std::string *uuid;	/* optional element of type xsd:string */
	std::string *packTitle;	/* optional element of type xsd:string */
	std::string *packName;	/* optional element of type xsd:string */
	std::string *packSize;	/* optional element of type xsd:string */
	std::string *ftpAdress;	/* optional element of type xsd:string */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 9; } /* = unique id SOAP_TYPE_ns21__PackInfo */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         ns21__PackInfo() { ns21__PackInfo::soap_default(NULL); }
	virtual ~ns21__PackInfo() { }
};
#endif

#ifndef SOAP_TYPE_ns21__WNode
#define SOAP_TYPE_ns21__WNode (10)
/* ns21:WNode */
class SOAP_CMAC ns21__WNode
{
public:
	std::string nodeName;	/* required element of type xsd:string */
	std::string nodeCode;	/* required element of type xsd:string */
	std::string city;	/* required element of type xsd:string */
	int cinemaType;	/* required element of type xsd:int */
	std::string *userName;	/* optional element of type xsd:string */
	std::string *tel;	/* optional element of type xsd:string */
	std::string transferMode;	/* required element of type xsd:string */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 10; } /* = unique id SOAP_TYPE_ns21__WNode */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         ns21__WNode() { ns21__WNode::soap_default(NULL); }
	virtual ~ns21__WNode() { }
};
#endif

#ifndef SOAP_TYPE_ns21__transferResult
#define SOAP_TYPE_ns21__transferResult (11)
/* ns21:transferResult */
class SOAP_CMAC ns21__transferResult
{
public:
	std::string *arg0;	/* optional element of type xsd:string */
	bool arg1;	/* required element of type xsd:boolean */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 11; } /* = unique id SOAP_TYPE_ns21__transferResult */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         ns21__transferResult() { ns21__transferResult::soap_default(NULL); }
	virtual ~ns21__transferResult() { }
};
#endif

#ifndef SOAP_TYPE_ns21__transferResultResponse
#define SOAP_TYPE_ns21__transferResultResponse (12)
/* ns21:transferResultResponse */
class SOAP_CMAC ns21__transferResultResponse
{
public:
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 12; } /* = unique id SOAP_TYPE_ns21__transferResultResponse */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         ns21__transferResultResponse() { ns21__transferResultResponse::soap_default(NULL); }
	virtual ~ns21__transferResultResponse() { }
};
#endif

#ifndef SOAP_TYPE_ns21__TransferResultSecond
#define SOAP_TYPE_ns21__TransferResultSecond (13)
/* ns21:TransferResultSecond */
class SOAP_CMAC ns21__TransferResultSecond
{
public:
	std::string *arg0;	/* optional element of type xsd:string */
	bool arg1;	/* required element of type xsd:boolean */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 13; } /* = unique id SOAP_TYPE_ns21__TransferResultSecond */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         ns21__TransferResultSecond() { ns21__TransferResultSecond::soap_default(NULL); }
	virtual ~ns21__TransferResultSecond() { }
};
#endif

#ifndef SOAP_TYPE_ns21__TransferResultSecondResponse
#define SOAP_TYPE_ns21__TransferResultSecondResponse (14)
/* ns21:TransferResultSecondResponse */
class SOAP_CMAC ns21__TransferResultSecondResponse
{
public:
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 14; } /* = unique id SOAP_TYPE_ns21__TransferResultSecondResponse */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         ns21__TransferResultSecondResponse() { ns21__TransferResultSecondResponse::soap_default(NULL); }
	virtual ~ns21__TransferResultSecondResponse() { }
};
#endif

#ifndef SOAP_TYPE_ns21__getSendinfo
#define SOAP_TYPE_ns21__getSendinfo (15)
/* ns21:getSendinfo */
class SOAP_CMAC ns21__getSendinfo
{
public:
	std::string *arg0;	/* optional element of type xsd:string */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 15; } /* = unique id SOAP_TYPE_ns21__getSendinfo */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         ns21__getSendinfo() { ns21__getSendinfo::soap_default(NULL); }
	virtual ~ns21__getSendinfo() { }
};
#endif

#ifndef SOAP_TYPE_ns21__getSendinfoResponse
#define SOAP_TYPE_ns21__getSendinfoResponse (16)
/* ns21:getSendinfoResponse */
class SOAP_CMAC ns21__getSendinfoResponse
{
public:
	std::vector<ns21__SendinfoTask * >return_;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* optional element of type ns21:SendinfoTask */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 16; } /* = unique id SOAP_TYPE_ns21__getSendinfoResponse */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         ns21__getSendinfoResponse() { ns21__getSendinfoResponse::soap_default(NULL); }
	virtual ~ns21__getSendinfoResponse() { }
};
#endif

#ifndef SOAP_TYPE_ns21__dispense
#define SOAP_TYPE_ns21__dispense (17)
/* ns21:dispense */
class SOAP_CMAC ns21__dispense
{
public:
	ns21__PackInfo *arg0;	/* optional element of type ns21:PackInfo */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 17; } /* = unique id SOAP_TYPE_ns21__dispense */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         ns21__dispense() { ns21__dispense::soap_default(NULL); }
	virtual ~ns21__dispense() { }
};
#endif

#ifndef SOAP_TYPE_ns21__dispenseResponse
#define SOAP_TYPE_ns21__dispenseResponse (18)
/* ns21:dispenseResponse */
class SOAP_CMAC ns21__dispenseResponse
{
public:
	bool return_;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type xsd:boolean */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 18; } /* = unique id SOAP_TYPE_ns21__dispenseResponse */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         ns21__dispenseResponse() { ns21__dispenseResponse::soap_default(NULL); }
	virtual ~ns21__dispenseResponse() { }
};
#endif

#ifndef SOAP_TYPE_ns21__addNode
#define SOAP_TYPE_ns21__addNode (19)
/* ns21:addNode */
class SOAP_CMAC ns21__addNode
{
public:
	ns21__WNode *arg0;	/* optional element of type ns21:WNode */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 19; } /* = unique id SOAP_TYPE_ns21__addNode */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         ns21__addNode() { ns21__addNode::soap_default(NULL); }
	virtual ~ns21__addNode() { }
};
#endif

#ifndef SOAP_TYPE_ns21__addNodeResponse
#define SOAP_TYPE_ns21__addNodeResponse (20)
/* ns21:addNodeResponse */
class SOAP_CMAC ns21__addNodeResponse
{
public:
	int *return_;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* optional element of type xsd:int */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 20; } /* = unique id SOAP_TYPE_ns21__addNodeResponse */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         ns21__addNodeResponse() { ns21__addNodeResponse::soap_default(NULL); }
	virtual ~ns21__addNodeResponse() { }
};
#endif

#ifndef SOAP_TYPE_ns21__addSendinfo
#define SOAP_TYPE_ns21__addSendinfo (21)
/* ns21:addSendinfo */
class SOAP_CMAC ns21__addSendinfo
{
public:
	std::string *arg0;	/* optional element of type xsd:string */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 21; } /* = unique id SOAP_TYPE_ns21__addSendinfo */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         ns21__addSendinfo() { ns21__addSendinfo::soap_default(NULL); }
	virtual ~ns21__addSendinfo() { }
};
#endif

#ifndef SOAP_TYPE_ns21__addSendinfoResponse
#define SOAP_TYPE_ns21__addSendinfoResponse (22)
/* ns21:addSendinfoResponse */
class SOAP_CMAC ns21__addSendinfoResponse
{
public:
	std::string *return_;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* optional element of type xsd:string */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 22; } /* = unique id SOAP_TYPE_ns21__addSendinfoResponse */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         ns21__addSendinfoResponse() { ns21__addSendinfoResponse::soap_default(NULL); }
	virtual ~ns21__addSendinfoResponse() { }
};
#endif

#ifndef SOAP_TYPE___ns21__transferResult
#define SOAP_TYPE___ns21__transferResult (35)
/* Operation wrapper: */
struct __ns21__transferResult
{
public:
	ns21__transferResult *ns21__transferResult_;	/* optional element of type ns21:transferResult */
public:
	int soap_type() const { return 35; } /* = unique id SOAP_TYPE___ns21__transferResult */
};
#endif

#ifndef SOAP_TYPE___ns21__TransferResultSecond
#define SOAP_TYPE___ns21__TransferResultSecond (39)
/* Operation wrapper: */
struct __ns21__TransferResultSecond
{
public:
	ns21__TransferResultSecond *ns21__TransferResultSecond_;	/* optional element of type ns21:TransferResultSecond */
public:
	int soap_type() const { return 39; } /* = unique id SOAP_TYPE___ns21__TransferResultSecond */
};
#endif

#ifndef SOAP_TYPE___ns21__getSendinfo
#define SOAP_TYPE___ns21__getSendinfo (43)
/* Operation wrapper: */
struct __ns21__getSendinfo
{
public:
	ns21__getSendinfo *ns21__getSendinfo_;	/* optional element of type ns21:getSendinfo */
public:
	int soap_type() const { return 43; } /* = unique id SOAP_TYPE___ns21__getSendinfo */
};
#endif

#ifndef SOAP_TYPE___ns21__dispense
#define SOAP_TYPE___ns21__dispense (47)
/* Operation wrapper: */
struct __ns21__dispense
{
public:
	ns21__dispense *ns21__dispense_;	/* optional element of type ns21:dispense */
public:
	int soap_type() const { return 47; } /* = unique id SOAP_TYPE___ns21__dispense */
};
#endif

#ifndef SOAP_TYPE___ns21__addNode
#define SOAP_TYPE___ns21__addNode (51)
/* Operation wrapper: */
struct __ns21__addNode
{
public:
	ns21__addNode *ns21__addNode_;	/* optional element of type ns21:addNode */
public:
	int soap_type() const { return 51; } /* = unique id SOAP_TYPE___ns21__addNode */
};
#endif

#ifndef SOAP_TYPE___ns21__addSendinfo
#define SOAP_TYPE___ns21__addSendinfo (55)
/* Operation wrapper: */
struct __ns21__addSendinfo
{
public:
	ns21__addSendinfo *ns21__addSendinfo_;	/* optional element of type ns21:addSendinfo */
public:
	int soap_type() const { return 55; } /* = unique id SOAP_TYPE___ns21__addSendinfo */
};
#endif

#ifndef SOAP_TYPE_ns1__InterfaceParameter
#define SOAP_TYPE_ns1__InterfaceParameter (56)
/* ns1:InterfaceParameter */
struct ns1__InterfaceParameter
{
public:
	std::string uuid;	/* required element of type xsd:string */
	std::string ftpUserName;	/* required element of type xsd:string */
	std::string ftpIP;	/* required element of type xsd:string */
	std::string ftpPassWord;	/* required element of type xsd:string */
	std::string ftpDirName;	/* required element of type xsd:string */
	std::string torrentP2pPath;	/* required element of type xsd:string */
	std::string packageName;	/* required element of type xsd:string */
	std::string packageSize;	/* required element of type xsd:string */
	std::string taskType;	/* required element of type xsd:string */
	std::string transferTempPath;	/* required element of type xsd:string */
	std::string transferStorePath;	/* required element of type xsd:string */
	std::string transferTorrentPath;	/* required element of type xsd:string */
public:
	int soap_type() const { return 56; } /* = unique id SOAP_TYPE_ns1__InterfaceParameter */
};
#endif

#ifndef SOAP_TYPE_ns1__isPackExistResponse
#define SOAP_TYPE_ns1__isPackExistResponse (59)
/* ns1:isPackExistResponse */
struct ns1__isPackExistResponse
{
public:
	bool ret;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type xsd:boolean */
public:
	int soap_type() const { return 59; } /* = unique id SOAP_TYPE_ns1__isPackExistResponse */
};
#endif

#ifndef SOAP_TYPE_ns1__isPackExist
#define SOAP_TYPE_ns1__isPackExist (60)
/* ns1:isPackExist */
struct ns1__isPackExist
{
public:
	struct ns1__InterfaceParameter param;	/* required element of type ns1:InterfaceParameter */
public:
	int soap_type() const { return 60; } /* = unique id SOAP_TYPE_ns1__isPackExist */
};
#endif

#ifndef SOAP_TYPE_ns1__transferResponse
#define SOAP_TYPE_ns1__transferResponse (62)
/* ns1:transferResponse */
struct ns1__transferResponse
{
public:
	bool ret;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type xsd:boolean */
public:
	int soap_type() const { return 62; } /* = unique id SOAP_TYPE_ns1__transferResponse */
};
#endif

#ifndef SOAP_TYPE_ns1__transfer
#define SOAP_TYPE_ns1__transfer (63)
/* ns1:transfer */
struct ns1__transfer
{
public:
	struct ns1__InterfaceParameter param;	/* required element of type ns1:InterfaceParameter */
public:
	int soap_type() const { return 63; } /* = unique id SOAP_TYPE_ns1__transfer */
};
#endif

#ifndef SOAP_TYPE_ns1__cutPackageResponse
#define SOAP_TYPE_ns1__cutPackageResponse (65)
/* ns1:cutPackageResponse */
struct ns1__cutPackageResponse
{
public:
	bool ret;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type xsd:boolean */
public:
	int soap_type() const { return 65; } /* = unique id SOAP_TYPE_ns1__cutPackageResponse */
};
#endif

#ifndef SOAP_TYPE_ns1__cutPackage
#define SOAP_TYPE_ns1__cutPackage (66)
/* ns1:cutPackage */
struct ns1__cutPackage
{
public:
	struct ns1__InterfaceParameter param;	/* required element of type ns1:InterfaceParameter */
public:
	int soap_type() const { return 66; } /* = unique id SOAP_TYPE_ns1__cutPackage */
};
#endif

#ifndef SOAP_TYPE_ns1__getUsableSpaceResponse
#define SOAP_TYPE_ns1__getUsableSpaceResponse (69)
/* ns1:getUsableSpaceResponse */
struct ns1__getUsableSpaceResponse
{
public:
	std::string ret;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type xsd:string */
public:
	int soap_type() const { return 69; } /* = unique id SOAP_TYPE_ns1__getUsableSpaceResponse */
};
#endif

#ifndef SOAP_TYPE_ns1__getUsableSpace
#define SOAP_TYPE_ns1__getUsableSpace (70)
/* ns1:getUsableSpace */
struct ns1__getUsableSpace
{
public:
	struct ns1__InterfaceParameter param;	/* required element of type ns1:InterfaceParameter */
public:
	int soap_type() const { return 70; } /* = unique id SOAP_TYPE_ns1__getUsableSpace */
};
#endif

#ifndef SOAP_TYPE_ns1__deletePackageResponse
#define SOAP_TYPE_ns1__deletePackageResponse (72)
/* ns1:deletePackageResponse */
struct ns1__deletePackageResponse
{
public:
	bool ret;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type xsd:boolean */
public:
	int soap_type() const { return 72; } /* = unique id SOAP_TYPE_ns1__deletePackageResponse */
};
#endif

#ifndef SOAP_TYPE_ns1__deletePackage
#define SOAP_TYPE_ns1__deletePackage (73)
/* ns1:deletePackage */
struct ns1__deletePackage
{
public:
	struct ns1__InterfaceParameter param;	/* required element of type ns1:InterfaceParameter */
public:
	int soap_type() const { return 73; } /* = unique id SOAP_TYPE_ns1__deletePackage */
};
#endif

#ifndef SOAP_TYPE_ns1__getTempPackageResponse
#define SOAP_TYPE_ns1__getTempPackageResponse (77)
/* ns1:getTempPackageResponse */
struct ns1__getTempPackageResponse
{
public:
	std::vector<std::string >ret;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type xsd:string */
public:
	int soap_type() const { return 77; } /* = unique id SOAP_TYPE_ns1__getTempPackageResponse */
};
#endif

#ifndef SOAP_TYPE_ns1__getTempPackage
#define SOAP_TYPE_ns1__getTempPackage (78)
/* ns1:getTempPackage */
struct ns1__getTempPackage
{
public:
	struct ns1__InterfaceParameter param;	/* required element of type ns1:InterfaceParameter */
public:
	int soap_type() const { return 78; } /* = unique id SOAP_TYPE_ns1__getTempPackage */
};
#endif

#ifndef SOAP_TYPE_ns1__maketoResponse
#define SOAP_TYPE_ns1__maketoResponse (80)
/* ns1:maketoResponse */
struct ns1__maketoResponse
{
public:
	bool ret;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type xsd:boolean */
public:
	int soap_type() const { return 80; } /* = unique id SOAP_TYPE_ns1__maketoResponse */
};
#endif

#ifndef SOAP_TYPE_ns1__maketo
#define SOAP_TYPE_ns1__maketo (81)
/* ns1:maketo */
struct ns1__maketo
{
public:
	struct ns1__InterfaceParameter param;	/* required element of type ns1:InterfaceParameter */
public:
	int soap_type() const { return 81; } /* = unique id SOAP_TYPE_ns1__maketo */
};
#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_SOAP_ENV__Header
#define SOAP_TYPE_SOAP_ENV__Header (82)
/* SOAP Header: */
struct SOAP_ENV__Header
{
public:
	int soap_type() const { return 82; } /* = unique id SOAP_TYPE_SOAP_ENV__Header */
#ifdef WITH_NOEMPTYSTRUCT
private:
	char dummy;	/* dummy member to enable compilation */
#endif
};
#endif

#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_SOAP_ENV__Code
#define SOAP_TYPE_SOAP_ENV__Code (83)
/* SOAP Fault Code: */
struct SOAP_ENV__Code
{
public:
	char *SOAP_ENV__Value;	/* optional element of type xsd:QName */
	struct SOAP_ENV__Code *SOAP_ENV__Subcode;	/* optional element of type SOAP-ENV:Code */
public:
	int soap_type() const { return 83; } /* = unique id SOAP_TYPE_SOAP_ENV__Code */
};
#endif

#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_SOAP_ENV__Detail
#define SOAP_TYPE_SOAP_ENV__Detail (85)
/* SOAP-ENV:Detail */
struct SOAP_ENV__Detail
{
public:
	char *__any;
	int __type;	/* any type of element <fault> (defined below) */
	void *fault;	/* transient */
public:
	int soap_type() const { return 85; } /* = unique id SOAP_TYPE_SOAP_ENV__Detail */
};
#endif

#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_SOAP_ENV__Reason
#define SOAP_TYPE_SOAP_ENV__Reason (88)
/* SOAP-ENV:Reason */
struct SOAP_ENV__Reason
{
public:
	char *SOAP_ENV__Text;	/* optional element of type xsd:string */
public:
	int soap_type() const { return 88; } /* = unique id SOAP_TYPE_SOAP_ENV__Reason */
};
#endif

#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_SOAP_ENV__Fault
#define SOAP_TYPE_SOAP_ENV__Fault (89)
/* SOAP Fault: */
struct SOAP_ENV__Fault
{
public:
	char *faultcode;	/* optional element of type xsd:QName */
	char *faultstring;	/* optional element of type xsd:string */
	char *faultactor;	/* optional element of type xsd:string */
	struct SOAP_ENV__Detail *detail;	/* optional element of type SOAP-ENV:Detail */
	struct SOAP_ENV__Code *SOAP_ENV__Code;	/* optional element of type SOAP-ENV:Code */
	struct SOAP_ENV__Reason *SOAP_ENV__Reason;	/* optional element of type SOAP-ENV:Reason */
	char *SOAP_ENV__Node;	/* optional element of type xsd:string */
	char *SOAP_ENV__Role;	/* optional element of type xsd:string */
	struct SOAP_ENV__Detail *SOAP_ENV__Detail;	/* optional element of type SOAP-ENV:Detail */
public:
	int soap_type() const { return 89; } /* = unique id SOAP_TYPE_SOAP_ENV__Fault */
};
#endif

#endif

/******************************************************************************\
 *                                                                            *
 * Typedefs                                                                   *
 *                                                                            *
\******************************************************************************/

#ifndef SOAP_TYPE__QName
#define SOAP_TYPE__QName (5)
typedef char *_QName;
#endif

#ifndef SOAP_TYPE__XML
#define SOAP_TYPE__XML (6)
typedef char *_XML;
#endif


/******************************************************************************\
 *                                                                            *
 * Externals                                                                  *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Server-Side Operations                                                     *
 *                                                                            *
\******************************************************************************/


SOAP_FMAC5 int SOAP_FMAC6 __ns21__transferResult(struct soap*, ns21__transferResult *ns21__transferResult_, ns21__transferResultResponse *ns21__transferResultResponse_);

SOAP_FMAC5 int SOAP_FMAC6 __ns21__TransferResultSecond(struct soap*, ns21__TransferResultSecond *ns21__TransferResultSecond_, ns21__TransferResultSecondResponse *ns21__TransferResultSecondResponse_);

SOAP_FMAC5 int SOAP_FMAC6 __ns21__getSendinfo(struct soap*, ns21__getSendinfo *ns21__getSendinfo_, ns21__getSendinfoResponse *ns21__getSendinfoResponse_);

SOAP_FMAC5 int SOAP_FMAC6 __ns21__dispense(struct soap*, ns21__dispense *ns21__dispense_, ns21__dispenseResponse *ns21__dispenseResponse_);

SOAP_FMAC5 int SOAP_FMAC6 __ns21__addNode(struct soap*, ns21__addNode *ns21__addNode_, ns21__addNodeResponse *ns21__addNodeResponse_);

SOAP_FMAC5 int SOAP_FMAC6 __ns21__addSendinfo(struct soap*, ns21__addSendinfo *ns21__addSendinfo_, ns21__addSendinfoResponse *ns21__addSendinfoResponse_);

SOAP_FMAC5 int SOAP_FMAC6 ns1__isPackExist(struct soap*, struct ns1__InterfaceParameter param, bool &ret);

SOAP_FMAC5 int SOAP_FMAC6 ns1__transfer(struct soap*, struct ns1__InterfaceParameter param, bool &ret);

SOAP_FMAC5 int SOAP_FMAC6 ns1__cutPackage(struct soap*, struct ns1__InterfaceParameter param, bool &ret);

SOAP_FMAC5 int SOAP_FMAC6 ns1__getUsableSpace(struct soap*, struct ns1__InterfaceParameter param, std::string &ret);

SOAP_FMAC5 int SOAP_FMAC6 ns1__deletePackage(struct soap*, struct ns1__InterfaceParameter param, bool &ret);

SOAP_FMAC5 int SOAP_FMAC6 ns1__getTempPackage(struct soap*, struct ns1__InterfaceParameter param, std::vector<std::string >&ret);

SOAP_FMAC5 int SOAP_FMAC6 ns1__maketo(struct soap*, struct ns1__InterfaceParameter param, bool &ret);

/******************************************************************************\
 *                                                                            *
 * Server-Side Skeletons to Invoke Service Operations                         *
 *                                                                            *
\******************************************************************************/

extern "C" SOAP_FMAC5 int SOAP_FMAC6 soap_serve(struct soap*);

extern "C" SOAP_FMAC5 int SOAP_FMAC6 soap_serve_request(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve___ns21__transferResult(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve___ns21__TransferResultSecond(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve___ns21__getSendinfo(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve___ns21__dispense(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve___ns21__addNode(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve___ns21__addSendinfo(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_ns1__isPackExist(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_ns1__transfer(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_ns1__cutPackage(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_ns1__getUsableSpace(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_ns1__deletePackage(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_ns1__getTempPackage(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_ns1__maketo(struct soap*);

/******************************************************************************\
 *                                                                            *
 * Client-Side Call Stubs                                                     *
 *                                                                            *
\******************************************************************************/


SOAP_FMAC5 int SOAP_FMAC6 soap_call___ns21__transferResult(struct soap *soap, const char *soap_endpoint, const char *soap_action, ns21__transferResult *ns21__transferResult_, ns21__transferResultResponse *ns21__transferResultResponse_);

SOAP_FMAC5 int SOAP_FMAC6 soap_call___ns21__TransferResultSecond(struct soap *soap, const char *soap_endpoint, const char *soap_action, ns21__TransferResultSecond *ns21__TransferResultSecond_, ns21__TransferResultSecondResponse *ns21__TransferResultSecondResponse_);

SOAP_FMAC5 int SOAP_FMAC6 soap_call___ns21__getSendinfo(struct soap *soap, const char *soap_endpoint, const char *soap_action, ns21__getSendinfo *ns21__getSendinfo_, ns21__getSendinfoResponse *ns21__getSendinfoResponse_);

SOAP_FMAC5 int SOAP_FMAC6 soap_call___ns21__dispense(struct soap *soap, const char *soap_endpoint, const char *soap_action, ns21__dispense *ns21__dispense_, ns21__dispenseResponse *ns21__dispenseResponse_);

SOAP_FMAC5 int SOAP_FMAC6 soap_call___ns21__addNode(struct soap *soap, const char *soap_endpoint, const char *soap_action, ns21__addNode *ns21__addNode_, ns21__addNodeResponse *ns21__addNodeResponse_);

SOAP_FMAC5 int SOAP_FMAC6 soap_call___ns21__addSendinfo(struct soap *soap, const char *soap_endpoint, const char *soap_action, ns21__addSendinfo *ns21__addSendinfo_, ns21__addSendinfoResponse *ns21__addSendinfoResponse_);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_ns1__isPackExist(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct ns1__InterfaceParameter param, bool &ret);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_ns1__transfer(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct ns1__InterfaceParameter param, bool &ret);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_ns1__cutPackage(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct ns1__InterfaceParameter param, bool &ret);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_ns1__getUsableSpace(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct ns1__InterfaceParameter param, std::string &ret);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_ns1__deletePackage(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct ns1__InterfaceParameter param, bool &ret);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_ns1__getTempPackage(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct ns1__InterfaceParameter param, std::vector<std::string >&ret);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_ns1__maketo(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct ns1__InterfaceParameter param, bool &ret);

#endif

/* End of soapStub.h */
