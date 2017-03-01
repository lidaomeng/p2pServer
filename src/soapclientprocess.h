#ifndef _H_SOAPCLIENTPROCESS_
#define _H_SOAPCLIENTPROCESS_

#include "soapH.h"
#include <iostream>

//ip:服务端webservice IP地址,port:服务端webservice端口号,arg0:参数1,arg1:参数2
//error:异常字符串,当函数返回非0时,该字符串用于说明异常原因.
int TrnasferResult(const std::string &ip, unsigned short port, std::string &arg0, bool arg1, std::string &error);

#endif//_H_SOAPCLIENTPROCESS_
