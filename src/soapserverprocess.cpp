#include "soapserverprocess.h"
#include <pthread.h>

using namespace std;

int ExitSoapServer = 0;

void* SoapServerProcess(void *port)
{
	struct soap cSoap;
	soap_init(&cSoap);
	cSoap.fget = ForBrowser;
	soap_set_mode(&cSoap, SOAP_C_UTFSTRING);
	int result = soap_bind(&cSoap, NULL, *(unsigned short *)port, 10);
	if(result < 0)
	{
		cout<<"bind failed!"<<endl;
		exit(1);
	}

	cSoap.send_timeout = 10;
	cSoap.recv_timeout = 10;
	cSoap.accept_timeout = 5;
	cSoap.max_keep_alive = 10;

	while(!ExitSoapServer)
	{
		int result = soap_accept(&cSoap);
		if(result < 0)
			;//log
		else if(result == 0)
			;//do nothing
		else
		{
			soap_serve(&cSoap);
		}
		soap_destroy(&cSoap);
		soap_end(&cSoap);
	}
	return NULL;
}

int ForBrowser(soap *csoap)
{
	ReplaceHttp(csoap->endpoint);

	ifstream fin("ns1.tmp", ios::binary);
	if(!fin.is_open())
		return 404;

	csoap->http_content = "text/xml";
	soap_response(csoap, SOAP_FILE);

	while(!fin.eof())
	{
		fin.read(csoap->tmpbuf, sizeof(csoap->tmpbuf));
		if(soap_send_raw(csoap, csoap->tmpbuf, fin.gcount()))
			break;
	}
	
	fin.close();
	soap_end_send(csoap);
	return 0;
}

int ReplaceHttp(const std::string &endpoint)
{
	ifstream fin("ns1.wsdl", ios::binary);
	if(!fin.is_open())
		return 0;
	fin.seekg(0, ios::end);
	int fileLen = fin.tellg();
	fin.seekg(0, ios::beg);
	char *buffer = new char[fileLen + 1];
	fin.read(buffer, fileLen);
	buffer[fileLen] = 0;
	string xml = buffer;
	delete []buffer;
	fin.close();

	size_t pos = xml.find("http://localhost:80");
	if(pos == string::npos)
		return 0;
	string xml1 = xml.substr(0, pos);
	if(endpoint.find("?wsdl") != string::npos)
		xml1 += endpoint.substr(0, endpoint.size()-5);
	else
		xml1 += endpoint;
	xml1 += xml.substr(pos+strlen("http://localhost:80"), xml.size());

	ofstream fout("ns1.tmp", ios::binary);
	fout.write(xml1.c_str(), xml1.size());
	fout.close();
	return 0;
}
