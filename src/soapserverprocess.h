#ifndef _H_SOAPSERVERPROCESS_
#define _H_SOAPSERVERPROCESS_

#include "soapH.h"
#include <iostream>
#include <fstream>

void* SoapServerProcess(void *port);
int ForBrowser(soap *csoap);
int ReplaceHttp(const std::string &endpoint);

#endif//_H_SOAPSERVERPROCESS_
