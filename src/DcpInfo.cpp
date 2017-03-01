/*
*@file:DcpInfo.cpp
*@brief:DCP�������ߺ�����
*@author: zhangmiao@oristartech.com
*@date: 2012/06/05
*/
#ifndef __DCPINFO_H__
#include "DcpInfo.h"
#endif

//#define DIR_SEPCHAR '/'
#ifdef WIN32
#define DIR_SEPCHAR '\\'
#undef  _POSIX_
#include <io.h>
#include <sys/stat.h>
//#define atoll atol
#define atoll atof
//#define lstat stat
#define lstat _stat64
#define stat  _stat64
#endif

#ifndef WIN32
//#include "common/C_RunPara.h"
#endif

using namespace Content;
using namespace Content::Dcp;

DcpInfo::DcpInfo()
{
    memset(pklList,0,sizeof(pklList));
    pklN = 0;
    docRet = NULL;
    parser = NULL;
    output = NULL;
    serializer = NULL;
	impl = NULL;
    dcpSize = 0ll;
   
    assetmapIdVt.clear();
 
    assetmapFile.clear();
    
}
DcpInfo::~DcpInfo()
{
    for(int i=0;PKLInfo::num!=0;i++) 
	{ 
		delete pklList[i];
		pklList[i] = NULL;
	}
    //if(parser!=NULL)parser->release();
    //if(docRet!=NULL)docRet->release();
    //if(output!=NULL)output->release();
    //if(serializer!=NULL)serializer->release();
    
    //XMLPlatformUtils::Terminate();
}

//DcpInfo��ʼ����
int DcpInfo::init()
{
//#ifdef WIN32
    /*try
    {
       XMLPlatformUtils::Initialize();
    }
    catch (const XMLException& toCatch)
    {
       XERCES_STD_QUALIFIER cerr << "Error during initialization! :\n"
                              << StrX(toCatch.getMessage()) << XERCES_STD_QUALIFIER endl;
       return 1;
    }*/
//#endif

    XMLCh tempStr[100];
    XMLString::transcode("LS",tempStr,99);
    impl = DOMImplementationRegistry::getDOMImplementation(tempStr);

    parser = ((DOMImplementationLS *)impl)->createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS,0);
    if(parser->getDomConfig()->canSetParameter(XMLUni::fgDOMElementContentWhitespace,true))
       parser->getDomConfig()->setParameter(XMLUni::fgDOMElementContentWhitespace,bool(false));
    parser->getDomConfig()->setParameter(XMLUni::fgDOMErrorHandler, (const void*)(&errorHandler));
   
    output = ((DOMImplementationLS*)impl)->createLSOutput();
    //output->setEncoding(gOutputEncoding);
    output->setEncoding(NULL);

    serializer = ((DOMImplementationLS*)impl)->createLSSerializer();
    
    return 0;
}
int DcpInfo::getAssetmapList( const char *path,char **mapList, int *mapN, int max , string& error )
{
    int ret=0;
    
    ret = searchFile(path,"ASSETMAP",mapList,mapN,max);
    if(ret==-2 || ret==0 || ret==ENOENT);
    else {
        //printf("%s\n",strerror(ret));
		char buffer[BUF_SIZE]="";
		sprintf( buffer,"%s\n",strerror(ret) );
		error = buffer;
        return ret;
    }

	//---����file assetmap��ASSETMAP.xml �� assetmap.xml 
	ret = searchFile(path,"assetmap",mapList,mapN,max);
	if(ret==-2 || ret==0 || ret==ENOENT);
	else {
		//printf("%s\n",strerror(ret));
		char buffer[BUF_SIZE]="";
		sprintf( buffer,"%s\n",strerror(ret) );
		error = buffer;
		return ret;
	}

	ret = searchFile(path,"assetmap.xml",mapList,mapN,max);
	if(ret==-2 || ret==0 || ret==ENOENT);
	else {
		//printf("%s\n",strerror(ret));
		char buffer[BUF_SIZE]="";
		sprintf( buffer,"%s\n",strerror(ret) );
		error = buffer;
		return ret;
	}

	ret = searchFile(path,"ASSETMAP.xml",mapList,mapN,max);
	if(ret==-2 || ret==0 || ret==ENOENT);
	else {
		//printf("%s\n",strerror(ret));
		char buffer[BUF_SIZE]="";
		sprintf( buffer,"%s\n",strerror(ret) );
		error = buffer;
		return ret;
	}

    if(*mapN<=0) 
	{
		error = "can not find ASSETMAP!\n";
		printf("can not find ASSETMAP!\n");
		return -1;
	}
    return 0;
}
int DcpInfo::getKdmList(const char *path,char **kdmList, int *kdmN, int max , string& error)
{
    int ret=0;
  
    ret = searchFile2(path,".xml",kdmList,kdmN,max);
    if(ret==-2 || ret==0 || ret==ENOENT);
	else {
		//printf("%s\n",strerror(ret));
		char buffer[BUF_SIZE]="";
		sprintf( buffer,"%s\n",strerror(ret) );
		error = buffer;
		return ret;
	}
    if(*kdmN<=0) 
	{
		error = "can not find kdm!\n";
		printf("can not find kdm!\n");
		return 1;
	}
    //printf("kdmN=%d\n",kdmN);
    return 0;
}



DOMNode *DcpInfo::getNodeByXpath(DOMNode *root,std::vector<std::string> path)
{
    if(root==NULL) return NULL;
    for(size_t i=0;i<path.size();i++)
    {
        //printf("path:%s\n",(*path)[i].c_str());
        //XMLString::transcode((*path)[i].c_str(),temp,256);
        DOMNodeList *childList = root->getChildNodes();
        size_t j;
        for(j=0;j<childList->getLength();j++)
        {
            //std::cout << StrX(temp) << "--" << StrX(childList->item(j)->getNodeName()) <<std::endl;
            if(childList->item(j)->getNodeType()==DOMNode::ELEMENT_NODE 
               && XMLString::compareString(childList->item(j)->getNodeName(),X(path[i].c_str()))==0)
            {
                root = childList->item(j);
                //printf("%s\n",StrX(root->getNodeName()).localForm());
                break;
            }
        }
        if(j==childList->getLength()) return NULL;
    }

    return root;
}

int DcpInfo::ParseInit(int f_schechk,const std::vector<std::string> &schePath)
{
    bool DoNamespaces          = true;
    bool DoSchema              = false;
    bool SchemaFullChecking    = false;
    bool DoValidate            = false; 
    bool UseCacheGrammar       = false; 

    if(f_schechk!=0)
    {
        DoSchema              = true;
        SchemaFullChecking    = true;
        DoValidate            = true; 
        UseCacheGrammar       = true; 
    }

    DOMConfiguration  *config = parser->getDomConfig();
    config->setParameter(XMLUni::fgDOMValidate,DoValidate);
    config->setParameter(XMLUni::fgXercesSchema,DoSchema);
    config->setParameter(XMLUni::fgDOMNamespaces,DoNamespaces);
    config->setParameter(XMLUni::fgXercesSchemaFullChecking,SchemaFullChecking);
    config->setParameter(XMLUni::fgXercesUseCachedGrammarInParse,UseCacheGrammar);

    for(size_t i = 0; (f_schechk!=0)&&(i<schePath.size()); i++)
    {
		//@author zhangmiao@oristartech.com
		//@date [2013-03-08]
		//@brief �������쳣������׽xml�����������׳��������쳣��
		//@new
		try
		{
			if(0 == parser->loadGrammar(schePath[i].c_str(),Grammar::SchemaGrammarType, true))
			{
				XERCES_STD_QUALIFIER cout << "Loading grammar error:"
					<< schePath[i].c_str()
					<< XERCES_STD_QUALIFIER endl;
				
				//@author zhangmiao@oristartech.com
				//@date [2014-04-04]
				//@brief ��Ҫ����д������־�Ĵ���
				//@new
				string errstr = "Loading grammar error:" + schePath[i] + "!\n";
				WDELOG(errstr);
				//@modify end;

				return -1;
			}
		}
		catch (...)
		{
			printf( "Loading grammar error:Throw Exception!\n" );

			//@author zhangmiao@oristartech.com
			//@date [2014-04-04]
			//@brief ��Ҫ����д������־�Ĵ���
			//@new
			string errstr = "Loading grammar error:Throw Exception!\n";
			WDELOG(errstr);
			//@modify end;

			return -1;
		}
		//@modify end;
    }
    return 0;
}
DOMDocument *DcpInfo::ParseXML(const std::string &xmlFile)
{
    DOMDocument *pDomRet = NULL;

    if(xmlFile.empty()) {printf("papa is NULL!\n");return NULL;}
    
    bool errorOccurred = false;
    try{
        errorHandler.resetErrors();
        pDomRet = parser->parseURI(xmlFile.c_str());
    }
    catch (const XMLException& toCatch)
    {
        XERCES_STD_QUALIFIER cerr << "\nError during parsing: '" << xmlFile << "'\n"
         << "Exception message is:  \n"
         << StrX(toCatch.getMessage()) << "\n" << XERCES_STD_QUALIFIER endl;
         errorOccurred = true;

		 //@author zhangmiao@oristartech.com
		 //@date [2014-04-04]
		 //@brief ��Ҫ����д������־�Ĵ���
		 //@new
		 string errstr = "\nErrors during parsing: '" + xmlFile + "'\n" + "XMLException message is:  \n" + StrX(toCatch.getMessage()).localForm() + "\n";
		 WDELOG(errstr);
		 //@modify end;
    }
    catch (const DOMException& toCatch)
    {
        const unsigned int maxChars = 2047;
        XMLCh errText[maxChars + 1];
        XERCES_STD_QUALIFIER cerr << "\nDOM Error during parsing: '" << xmlFile << "'\n"
        << "DOMException code is:  " << toCatch.code << XERCES_STD_QUALIFIER endl;
        if (DOMImplementation::loadDOMExceptionMsg(toCatch.code, errText, maxChars))
        XERCES_STD_QUALIFIER cerr << "Message is: " << StrX(errText) << XERCES_STD_QUALIFIER endl;
        errorOccurred = true;

		//@author zhangmiao@oristartech.com
		//@date [2014-04-04]
		//@brief ��Ҫ����д������־�Ĵ���
		//@new
		string errstr = "\nDOM during parsing: '" + xmlFile + "'\n" + "Exception message is:  \n" + StrX(errText).localForm() + "\n";
		WDELOG(errstr);
		//@modify end;
     }
    catch (...)
    {
        XERCES_STD_QUALIFIER cerr << "\nUnexpected exception during parsing: '" << xmlFile << "'\n";
        errorOccurred = true;

		//@author zhangmiao@oristartech.com
		//@date [2014-04-04]
		//@brief ��Ҫ����д������־�Ĵ���
		//@new
		string errstr = "\nUnexpected exception during parsing: '" + xmlFile + "'\n";
		WDELOG(errstr);
		//@modify end;
    }
    if(errorHandler.getSawErrors())
    {
        XERCES_STD_QUALIFIER cout << "\nErrors occurred:" << xmlFile << XERCES_STD_QUALIFIER endl;
        errorOccurred = true;

		//@author zhangmiao@oristartech.com
		//@date [2014-04-04]
		//@brief ��Ҫ����д������־�Ĵ���
		//@new
		string errstr = "\nErrors occurred:" + xmlFile + "\n";
		WDELOG(errstr);
		//@modify end;
    }
    if(errorOccurred) return NULL;

    return pDomRet;
}

//----zhangmiao:begin--20120605---
int DcpInfo::ReBuild_Assetmap(  std::string xmlFile, const string &pklUuid, pkl_Info_t& aPkl , string& error )
{
	int ret=0;

	if(ret!=init()) 
	{
		error = "DcpInfo::init() fail.\n";
#ifdef _TEST_
		printf("fail to init()!\n");
#endif
		return -1;
	}

	XMLSize_t nodeNum;
	//bool errorOccurred = false;
	DOMDocument *docAsset = NULL;
	DOMNode *tmpN = NULL;
	std::vector<std::string> path;

	path.clear();
	if( ParseInit(0,path) == -1 ) 
	{
		error = "ReBuild_Assetmap fail to ParseInit()!\n";
		return -1;
	}
	docAsset = ParseXML(xmlFile);
	if(docAsset==NULL) 
	{
		char buffer[BUF_SIZE]="";
		sprintf( buffer,"ReBuild_Assetmap fail to ParseXML()!\n parse(%s) is error!\n",xmlFile.c_str() );
		error = buffer;
		printf("parse(%s) is error!\n",xmlFile.c_str());
		return -1;
	}

	DOMElement *rootAsset = docAsset->getDocumentElement();
	//@author zhangmiao@oristartech.com
	//@date [2013-07-15]
	//@brief �����µ�AssetMap Uuid
	//��Ҫ�����������Ļ�ļ��Ĵ������
	//@new
	//���Ƚ�����AssetMap Uuid
	path.clear();path.push_back("Id");
	tmpN = getNodeByXpath(rootAsset,path);
	if(tmpN==NULL)
	{
		error += "ReBuild_Assetmap(AssetMap Id) is error!\n";
		printf("ReBuild_Assetmap(AssetMap Id) is error!\n");
		return -1;
	}
	string AssetMapUuid = "urn:uuid:";
	uuid_t uuid;
	uuid_generate_random(uuid);
	char tmp[3] = "";
	for(int i=0; i<16; i++)
	{
		sprintf(tmp, "%02x", uuid[i]);
		AssetMapUuid += tmp;
		if(i==3||i==5||i==7||i==9)
			AssetMapUuid += "-";
	}
	tmpN->setTextContent(X(AssetMapUuid.c_str()));
	//@modify end;

	//@author zhangmiao@oristartech.com
	//@date [2014-09-15]
	//@brief �����µ�AssetMap�ļ���<AnnotationText>���ݸ�ʽ��AssetMapUuid _+ԭʼAnnotationText���ݣ��255���ַ�
	//@new
	string new_AssetmapId;
	new_AssetmapId = AssetMapUuid;
	Remove_PrefixOfUuid( new_AssetmapId );

	DOMNode *tmpN_AnnotationText = NULL;
	path.clear();
	path.push_back("AnnotationText");
	tmpN_AnnotationText = getNodeByXpath(rootAsset,path);
	if( tmpN_AnnotationText == NULL )
	{
		error += "ReBuild_Assetmap(AssetMap AnnotationText) is Null!\n";
		printf("ReBuild_Assetmap(AssetMap AnnotationText) is Null!\n");
		//return -1;
	}
	else
	{
		string AnnotationText = StrX(tmpN_AnnotationText->getTextContent()).localForm();
		AnnotationText = new_AssetmapId + "_" + AnnotationText;
		if ( AnnotationText.length() > 255 )
		{
			AnnotationText = AnnotationText.substr( 0 , 255 );
		}

		tmpN_AnnotationText->setTextContent(X(AnnotationText.c_str()));
	}
	//@modify [2014-09-15] end;

	DOMNodeList *nlist = docAsset->getElementsByTagName(X("AssetList"));
	nodeNum = nlist->getLength(); 
	//printf("nodeNum=%d\n",nodeNum);
	if(nodeNum==0)
	{
		error = "Parse Assetmap error:not find AssetList!\n";
#ifdef _TEST_
		printf("Parse Assetmap error:not find AssetList!\n");
#endif
		return -1;
	}

	DOMNode* cloneNode = NULL;
	DOMNode* tmpCloneNode = NULL;
	//copy a empty AssetMap's AssetList 
	DOMNode* nAssetList = nlist->item(0);
	if (nAssetList)
	{
		cloneNode = nAssetList->cloneNode(false);
	}

    nlist = docAsset->getElementsByTagName(X("Asset"));
	nodeNum = nlist->getLength(); 
	size_t i=0;
	//insert pkl file info.
	while( i < nlist->getLength() )
	{
		//DOMNode *nParent = nlist->item(i)->getParentNode();
		//��ȡÿ��<Asset>��uuid
		DOMNode *nAssetChild = nlist->item(i);
		path.clear();path.push_back("Id");
		tmpN = getNodeByXpath(nAssetChild,path);
		if(tmpN==NULL) 
		{
			error = "parseAssetmap(pklId) is error!\n";
			printf("parseAssetmap(pklId) is error!\n");
			return -1;
		}
		std::string pklId = StrX(tmpN->getTextContent()).localForm();
		
		Remove_PrefixOfUuid( pklId );
		
		if (pklId == pklUuid)	//����ҵ�pkl�ļ����ڵĽڵ㣬���Ƹýڵ㣬���������뵽�µ�AssetMap's AssetList�ڵ����
		{
			tmpCloneNode = nAssetChild->cloneNode(true);	//���Ƹýڵ�
			cloneNode->appendChild(tmpCloneNode);	//���뵽�µ�AssetMap's AssetList�ڵ����
			break;
		}
		else
		{
			i++;
		}
	}
	//insert other asset file info.
	vector<asset_pkl_t> vAsset_pkl = aPkl.asset;
	int nSize_Asset_Pkl = vAsset_pkl.size();
	for ( int k=0; k< nSize_Asset_Pkl; k++ )
	{
		asset_pkl_t aAsset_Pkl = vAsset_pkl[k];
		string fileUuid = aAsset_Pkl.uuid;
		i=0;
		while( i < nlist->getLength() )
		{
			//DOMNode *nParent = nlist->item(i)->getParentNode();
			//��ȡÿ��<Asset>��uuid
			DOMNode *nAssetChild = nlist->item(i);
			path.clear();path.push_back("Id");
			tmpN = getNodeByXpath(nAssetChild,path);
			if(tmpN==NULL) 
			{
				error = "parseAssetmap(pklId) is error!\n";
				printf("parseAssetmap(pklId) is error!\n");
				return -1;
			}
			std::string Id = StrX(tmpN->getTextContent()).localForm();

			Remove_PrefixOfUuid( Id );
			
			if ( Id == fileUuid )	//����ҵ�vAsset_pkl��ƥ��Ľڵ㣬���Ƹýڵ㣬���������뵽�µ�AssetMap's AssetList�ڵ����
			{
				tmpCloneNode = nAssetChild->cloneNode(true);	//���Ƹýڵ�
				cloneNode->appendChild(tmpCloneNode);	//���뵽�µ�AssetMap's AssetList�ڵ����
				break;
			}
			else
			{
				i++;
			}
		}
	}

	//@author zhangmiao@oristartech.com
	//@date [2012-12-21]
	//@new
	//�ж��������ɵ�AssetMap��AssetList�ڵ���Ƿ�Ϊ��
	if ( cloneNode == NULL )
	{
		error = "ReBuild_Assetmap error: cloneNode is Null !\n";
		printf("ReBuild_Assetmap error: cloneNode is Null !\n");
		return -1;
	}
	else
	{
		DOMNodeList* nChildNodesList = cloneNode->getChildNodes();
		if ( nChildNodesList == NULL )
		{
			error = "ReBuild_Assetmap error: ChildNodesList Of cloneNode is Null!\n";
			printf("ReBuild_Assetmap error: ChildNodesList Of cloneNode is Null!\n");
			return -1;
		}
	}
	//@modify end;

	nodeNum = cloneNode->getChildNodes()->getLength();
#ifdef _TEST_
	printf("nodeNum=%d\n",(int)nodeNum);
#endif
	if(nodeNum!=0)
	{
		rootAsset->replaceChild( cloneNode, nAssetList ) ;	//���µ�AssetMap��AssetList�ڵ���滻ԭ�е�AssetList
		//д���ļ�����
		XMLFormatTarget *formatTarget;	
#if 1
		formatTarget = new LocalFileFormatTarget(xmlFile.c_str());
		output->setByteStream(formatTarget);
		serializer->getDomConfig()->setParameter(X("format-pretty-print"), true);
		serializer->write(docAsset, output);
		delete formatTarget;
#endif
	}
	
	return 0;
}

int DcpInfo::Parse_Assetmap_New( std::string xmlFile ,AMInfo& aAMInfo , string& error )
{
	if( 0!= init() )
	{
		error += "DcpInfo::init() fail.\n";
		printf("fail to init()!\n");
		return -1;
	}

	assetmap_Info assetmap_info;

	XMLSize_t nodeNum = 0;
	bool errorOccurred = false;
	DOMDocument *docAsset = NULL;
	std::vector<std::string> path;

	path.clear();
	if( ParseInit(0,path) == -1 ) 
	{
		error += "fail to ParseInit()!\n";
		return -1;
	}
	docAsset = ParseXML(xmlFile);
	if(docAsset==NULL) 
	{
		char buffer[BUF_SIZE]="";
		sprintf( buffer,"Parse_Assetmap_New fail to ParseXML()!\n parse(%s) is error!\n",xmlFile.c_str() );
		error += buffer;
		printf("parse(%s) is error!\n",xmlFile.c_str());
		return -1;
	}

	DOMElement *rootAsset = docAsset->getDocumentElement();

	//���Ƚ�����AssetMap Uuid
	path.clear();path.push_back("Id");
	DOMNode *tmpN = getNodeByXpath(rootAsset,path);
	if(tmpN==NULL)
	{
		error += "parseAssetmap(AssetId) is error!\n";
		printf("parseAssetmap(AssetId) is error!\n");
		return -1;
	}
	std::string assetmapId = StrX(tmpN->getTextContent()).localForm();
	
	Remove_PrefixOfUuid( assetmapId );

	//Assetmap�ļ� ����ظ����־Ͳ��ڽ���
	for(size_t i=0;i<assetmapIdVt.size();i++) //remember the assetmapId,not send the same id;
	{
		if(assetmapIdVt[i]==assetmapId)
		{
			char buffer[BUF_SIZE]="";
			sprintf( buffer,"Assetmap is repeat!%s\n",xmlFile.c_str() );
			error += buffer;
			printf("Assetmap is repeat!%s\n",xmlFile.c_str());
			return -1;
		}
	}

	assetmap_info.assetmapUuid = assetmapId;

	//@author zhangmiao@oristartech.com
	//@date [2015-04-17]
	//@brief ��Ҫ���ӶԵ�Ӱ����ʽ�ֶεĴ������
	//@new
	aAMInfo.nFilmPackageFormat = 0;
	std::string sAssetFile_AssetmapNamespaceURI;
	if ( rootAsset )
	{
		sAssetFile_AssetmapNamespaceURI = StrX(rootAsset->getNamespaceURI()).localForm();

		string::size_type pos = sAssetFile_AssetmapNamespaceURI.find("smpte");
		if( pos != string::npos )
		{
			aAMInfo.nFilmPackageFormat = 1;
		}
		else
		{
			aAMInfo.nFilmPackageFormat = 0;
		}
	}
	//@modify end;

	//��AssetList�е�ÿ��Asset�����������
	assetmapFile.clear();//assetmapFile�ڽ���pklʱ���õ�����Parse_Pkl_New()�л���ã������ظ�����
	//�ҳ����е�<Path>�ڵ㣬���н���
	DOMNodeList *pathList = docAsset->getElementsByTagName(X("Path"));
	nodeNum = pathList->getLength(); 
	//printf("nodeNum=%d\n",nodeNum);
	if(nodeNum==0)
	{
		error += "parseAssetmap(path) is error!\n";
		printf("parseAssetmap(path) is error!\n");
		return -1;
	}
	for(size_t i=0;i<nodeNum;i++)
	{
		DOMNode *tmpPath = pathList->item(i);

		DOMNode *tmpN = pathList->item(i);
		if(tmpN==NULL)
		{
			error += "parseAssetmap(path) is error!\n";
			printf("parseAssetmap(path) is error!\n");
			return -1;
		}
		std::string fileName = StrX(tmpN->getTextContent()).localForm();
		//@author zhangmiao@oristartech.com
		//@date [2013-07-15]
		//��Ҫ�����������Ļ�ļ��Ĵ������
		//@new
		std::string PathfileName = StrX(tmpN->getTextContent()).localForm();
#ifdef WIN32
		//�����Ŀ¼'/',�Ὣ����'/'�滻Ϊ'\'
		string::size_type pos = 0;
		pos = fileName.rfind('/');
		if(pos != string::npos)
			fileName.replace(pos,1,"\\");
#endif //WIN32
		//@modify end;
		

		//@author zhangmiao@oristartech.com
		//@date [2012-12-21]
		//@new
		//�ж�<Path>�ĸ��ڵ�<Asset>�Ƿ����
		if ( tmpN->getParentNode() != NULL )
		{
			DOMNode *tmpNODE = tmpN->getParentNode();
			if ( tmpNODE->getParentNode() != NULL )
			{
				if (tmpNODE->getParentNode()->getParentNode() == NULL )
				{
					error += "parseAssetmap( path: getParentNode() ) is error!\n";
					printf("parseAssetmap( path: getParentNode() ) is error!\n");
					return -1;
				}
			}
			else
			{
				error += "parseAssetmap( path: getParentNode() ) is error!\n";
				printf("parseAssetmap( path: getParentNode() ) is error!\n");
				return -1;
			}
		}
		else
		{
			error += "parseAssetmap( path: getParentNode() ) is error!\n";
			printf("parseAssetmap( path: getParentNode() ) is error!\n");
			return -1;
		}
		//@modify end;

		//��<Asset>���ڵ��µ�<Id>ȡ����������ȡÿ���ļ���UUID
		path.clear();path.push_back("Id");
		tmpN = getNodeByXpath(tmpN->getParentNode()->getParentNode()->getParentNode(),path);
		if(tmpN==NULL)
		{
			error += "parseAssetmap(fileId) is error!\n";
			printf("parseAssetmap(fileId) is error!\n");
			return -1;
		}
		std::string fileId = StrX(tmpN->getTextContent()).localForm();
		
		Remove_PrefixOfUuid( fileId );

		std::string sDir = xmlFile.substr(0,xmlFile.rfind(DIR_SEPCHAR)+1);
		file_Info_t fileInfo;
		fileInfo.f_path = sDir + fileName;
		fileInfo.f_id   = fileId;
		//@author zhangmiao@oristartech.com
		//@date [2013-07-15]
		//��Ҫ�����������Ļ�ļ��Ĵ������
		//@new
		fileInfo.f_PathOri = PathfileName;
		//@modify end;
		assetmapFile.push_back(fileInfo);

		//----zhangmiao---parseAssetmap(Length)-------
		//��ÿ���ļ�<Length>��������
		unsigned long long AssetFileLength = 0;
		path.clear();path.push_back("Length");
		if(tmpPath==NULL)
		{
			error += "parseAssetmap(path) is error!\n";
			printf("parseAssetmap(path) is error!\n");
			return -1;
		}
		//const XMLCh * n=  tmpPath->getParentNode()->getNodeName();
		tmpN = getNodeByXpath(tmpPath->getParentNode(),path);
		if(tmpN==NULL)
		{
			error += "parseAssetmap(Length) is error:"+fileName+" not find Length !\n";
			printf("parseAssetmap(Length) is error:%s not find Length !\n" , fileName.c_str() );
			//return -1;
		}
		else
		{
			std::string fileLength = StrX(tmpN->getTextContent()).localForm();
			AssetFileLength =atoll(fileLength.c_str());
		}
        
		asset_assetmap_t asset_assetmap;
		asset_assetmap.uuid = fileId;

		//std::string sDir = xmlFile.substr(0,xmlFile.length()-8);	
		//@author zhangmiao@oristartech.com
		//@date [2013-07-15]
		//��Ҫ�����������Ļ�ļ��Ĵ������
		//@new
		asset_assetmap.Path_NotFullPath = PathfileName;
		//@modify end;

		asset_assetmap.path = sDir + fileName;
		asset_assetmap.Length = AssetFileLength;
		
		assetmap_info.asset.push_back(asset_assetmap);
		//---------------
	}
	if(errorOccurred)
	{
		return -1;
	}
	assetmap_InfoVt.push_back(assetmap_info);

	aAMInfo.aAssetmap_Info = assetmap_info;
	aAMInfo.assetmapName = xmlFile;
	aAMInfo.dcpPath = xmlFile.substr(0,xmlFile.rfind(DIR_SEPCHAR)+1);

	//��pkl������ļ���Ϣ��������������һ����pkl�ļ�����
	for(int i=0;PKLInfo::num!=0;i++) 
	{
		printf("delete pklList[%d]\n",i);
		delete pklList[i];
		pklList[i] = NULL;
	}

	pklN = 0;
	DOMNodeList *nlist = docAsset->getElementsByTagName(X("PackingList"));
	nodeNum = nlist->getLength(); 
	//printf("nodeNum=%d\n",nodeNum);
	if(nodeNum==0)
	{
		error += "Parse Assetmap error:not find PackingList!\n";
		printf("Parse Assetmap error:not find PackingList!\n");
		return -1;
	}
	for(size_t i=0;i<nodeNum;i++)
	{
		//���<PackingList>�ĸ��ڵ�<Asset>�����ж��Ƿ����
		DOMNode *nParent = nlist->item(i)->getParentNode();
		//@author zhangmiao@oristartech.com
		//@date [2012-12-21]
		//@new
		if ( nParent == NULL )
		{
			error += "Parse Assetmap error:not find ParentNode Of PackingList!\n";
			printf("Parse Assetmap error:not find ParentNode Of PackingList!\n");
			return -1;
		}
		//@modify end;
		
		//��ȡpkl�ļ���Path������Ǹ��ļ���
		path.clear();
		path.push_back("ChunkList");path.push_back("Chunk");path.push_back("Path");
		tmpN = getNodeByXpath(nParent,path);
		if(tmpN==NULL) 
		{
			error += "parseAssetmap(chunk,path) is error!\n";
			printf("parseAssetmap(chunk,path) is error!\n");
			return -1;
		}
		std::string pklPath = StrX(tmpN->getTextContent()).localForm();
		std::string pklDir = xmlFile.substr(0,xmlFile.rfind(DIR_SEPCHAR)+1);

		//��ȡPKL�ļ���Uuid
		path.clear();path.push_back("Id");
		tmpN = getNodeByXpath(nParent,path);
		if(tmpN==NULL) 
		{
			error += "parseAssetmap(pklId) is error!\n";
			printf("parseAssetmap(pklId) is error!\n");
			return -1;
		}
		std::string pklId = StrX(tmpN->getTextContent()).localForm();

		Remove_PrefixOfUuid( pklId );

		if(++pklN>PKL_N_MAX) //Ŀǰ���֧��64��
		{
			error += "pklN>PKL_N_MAX is error!\n";
			printf("%d\n",pklN);
			return -1;
		}
		PKLInfo *p = new PKLInfo();
		if(p==NULL) 
		{
			error += "new() PKLInfo is failed!\n";
			printf("new() PKLInfo is failed!\n");
			return -1;
		}
		//��pkl�ļ���uuid��path��¼����
		pklList[PKLInfo::num-1] = p;
		pklList[PKLInfo::num-1]->path = pklDir + pklPath;
		pklList[PKLInfo::num-1]->id = pklId;
	}
 
	assetmapIdVt.push_back(assetmapId);
	return 0;
}

int DcpInfo::Parse_Pkl_New( PKLInfo *pkl , dcp_Info_t& dcp , string& error )
{
	if( 0!= init() )
	{
		error = "DcpInfo::init() fail.\n";
		printf("fail to init()!\n");
		return -1;
	}

	pkl_Info_t aPkl_Info;
	

	std::vector<std::string> path;
	int i,nodeNum = 0;
	//XMLCh *xmlC = NULL;
	bool errorOccurred = false;

	path.clear();
	if(-1==ParseInit(0,path))
	{
		error = "Pkl fail to ParseInit()!\n";
		return -1;
	}
	DOMDocument *docPkl = ParseXML((pkl->path).c_str());
	if(docPkl==NULL)
	{
		char buffer[BUF_SIZE]="";
		sprintf( buffer,"pkl fail to ParseXML()!\n parse(%s) is error!\n",(pkl->path).c_str() );
		error = buffer;
		printf("parse(%s) is error!\n",(pkl->path).c_str());
		return 1;
	}

	aPkl_Info.uuid = pkl->id;

	DOMElement *rootEle = docPkl->getDocumentElement();

	//��ȡ�ڵ�<AnnotationText>��ֵ
	path.clear();path.push_back("AnnotationText");
	DOMNode    *tmpN = getNodeByXpath(rootEle,path);
	if(tmpN!=NULL) 
	{
		std::string annoText = StrX(tmpN->getTextContent()).localForm();
		aPkl_Info.annotationText = annoText;
	}
	else 
	{
		//@author zhangmiao@oristartech.com
		//@date 2013-06-19
		//@brief ���<AnnotationText>��ѡԪ�ز����ڣ�����򲻱���:begin
		//˵��:pkl�ļ���<PackingList>�µ�<AnnotationText>Ԫ��Ϊ��ѡ��ǣ�����ڴ˴��Ȳ������ء�
		//@new		
		//errorOccurred = true;

		char buffer[BUF_SIZE] = "";
		sprintf( buffer , "Warning:Parse PKL Element<AnnotationText> NOT exist in <PackingList>(%s)\n" , (pkl->path).c_str() );
		error += buffer;
		printf( "%s" , buffer );
		//@modify end;
	}

	//��������ڵ�<IssueDate>��<Issuer>��<Creator>
	path.clear();
	path.push_back("IssueDate");path.push_back("Issuer");path.push_back("Creator");
	for(size_t j=0;(errorOccurred==false)&&(j<path.size());j++)
	{
		std::vector<std::string> tmpV(1,path[j]);
		DOMNode    *tmpN = getNodeByXpath(rootEle,tmpV);
		if(tmpN==NULL)
		{
			error = "getNodeByXpath is NULL\n";
			printf("getNodeByXpath is NULL\n");
			errorOccurred = true;

			//�ڵ�<IssueDate>��<Issuer>��<Creator> ����һ��������ʱ�����д�����
			printf( "Error:Parse PKL Element<%s> NOT exist in <PackingList>(%s)" , path[j].c_str() , (pkl->path).c_str());
			break;
		}

		std::string aText = StrX(tmpN->getTextContent()).localForm();
		if (path[j]=="IssueDate")
		{
			aPkl_Info.issueDate = aText;
		}
		else if (path[j]=="Issuer")
		{
			aPkl_Info.issuer = aText;
		}
		else if(path[j]=="Creator")
		{
			aPkl_Info.creator = aText;
		}
	}
	//����ڵ㲻���ڣ�������
	if(errorOccurred)
	{
		return 1;
	}

	dcpSize = 0ull;	//���ڱ���pkl���Ĵ�С

	//�������е�<Asset>�ڵ�
	DOMNodeList *nList = docPkl->getElementsByTagName(X("Asset"));
	nodeNum = nList->getLength(); 
	//printf("Asset num=%d\n",nodeNum);
	if(nodeNum==0)
	{
		error = "error:Asset num = 0\n";
		errorOccurred = true;
	}

	//�������<Asset>�ڵ㣬���浽aPkl_Info.asset�ṹ��
	for(i=0;(errorOccurred==false)&&(i<nodeNum);i++)
	{
		asset_pkl_t aAsset_Pkl;

		std::string idValue   = "";
		std::string pathValue = "";
		std::string typeValue = "";
		std::string hashValue = "";
		//@author zhangmiao@oristartech.com
		//@date [2013-07-15]
		//��Ҫ�����������Ļ�ļ��Ĵ������
		std::string Path_NotFullPath = "";
		//@modify end;

		unsigned long long  nSizeValue =0ull;

		DOMNode *tmpN = nList->item(i);

		//�������<Asset>�ڵ������Ԫ�� <Id>��<Size>��<Type>
		path.clear();
		path.push_back("Id");path.push_back("Size");path.push_back("Type");
		for(size_t j=0;j<path.size();j++)
		{
			XMLCh xmlT[1024] = {0};  
			std::vector<std::string> tmpV(1,path[j]);
			DOMNode    *tmpN = getNodeByXpath(nList->item(i),tmpV);
			if(tmpN==NULL)
			{
				error = "getNodeByXpath(Asset)\n";
				printf("getNodeByXpath(Asset)\n");
				errorOccurred=true;
				break;
			}

			XMLString::copyString(xmlT,tmpN->getTextContent());
			
			if(path[j]=="Id")
			{
				idValue = StrX(xmlT).localForm();
				Remove_PrefixOfUuid(idValue); 
			}
			if(path[j]=="Size")
			{
				nSizeValue = atoll(StrX(xmlT).localForm());
				dcpSize += atoll(StrX(xmlT).localForm());	//����pkl���������ļ���С�ܺ�
			}
			if(path[j]=="Type")typeValue = StrX(xmlT).localForm();
		}
		if(errorOccurred)
		{
			break;
		}
		//��һ��ֱ�Ӵӻ���assetmapFile�б�������ȡidValue��Ӧ��path
		for(size_t j=0;j<assetmapFile.size();j++)
		{
			if(idValue==assetmapFile[j].f_id)
			{
				pathValue = assetmapFile[j].f_path;
				//@author zhangmiao@oristartech.com
				//@date [2013-07-15]
				//��Ҫ�����������Ļ�ļ��Ĵ������
				Path_NotFullPath = assetmapFile[j].f_PathOri;
				//@modify end;
				break;
			}
		}
		if(pathValue.empty())
		{
			errorOccurred = true;
			break;
		}
		
		//cpl the sha1 check;
		//if(0!=typeValue.compare("text/xml;asdcpKind=CPL")) continue;

		//�ٶ�ȡ�� <Hash>��Ӧ��hashValue
		path.clear();path.push_back("Hash");
		tmpN = getNodeByXpath(nList->item(i),path);
		if(tmpN==NULL)
		{
			char buffer[BUF_SIZE]="";
			sprintf( buffer,"getNodeByXpath %s is failed!\n",path[0].c_str() );
			error = buffer;

			printf("getNodeByXpath %s is failed!\n",path[0].c_str());
			errorOccurred = true;
			break;
		}
		hashValue = StrX(tmpN->getTextContent()).localForm();
		//������ݶ�������asset_pkl_t�ṹ��
		aAsset_Pkl.hash = hashValue;
		aAsset_Pkl.type = typeValue;
		aAsset_Pkl.uuid = idValue;
		aAsset_Pkl.originalFileName = pathValue;
		aAsset_Pkl.size = nSizeValue;

		//@author zhangmiao@oristartech.com
		//@date [2013-07-15]
		//��Ҫ�����������Ļ�ļ��Ĵ������
		aAsset_Pkl.Path_NotFullPath = Path_NotFullPath;
		//@modify end;

		aPkl_Info.asset.push_back(aAsset_Pkl);
	}

	if(errorOccurred)
	{
		return 1;
	}

	char strT[32];
	sprintf(strT,"%lld",dcpSize);

	aPkl_Info.allFileLength = dcpSize;

	pkl_Info_Vt.push_back(aPkl_Info);

	dcp.aPkl = aPkl_Info;

	return 0;
}

int DcpInfo::Parse_Cpl_New( const char *cplPath, cpl_Info_t &cpl , string& error )
{
	std::vector<std::string> path;
	DOMNode *tmpN = NULL;
	//���Ƚ���cpl��schema��֤

	path.push_back("./cplSchema/xmldsig-core-schema.xsd");
	path.push_back("./cplSchema/xml.xsd");
	path.push_back("./cplSchema/CPL_schema.xsd");
	path.push_back("./cplSchema/SMPTE-429-7-2006-CPL.xsd");
	path.push_back("./cplSchema/SMPTE-429-10-2008-Main-Stereo-Picture-CPL.xsd");
	path.push_back("./cplSchema/Asdcp-Main-Stereo-Picture-CPL.xsd");

	if( ParseInit(1,path) == -1 )
	{
		error += "Cpl error:parse init is error!\n";
		printf("parse init is error!\n");
		return -1;
	}
	string strXml = cplPath;
	DOMDocument *docCpl = ParseXML(strXml);
	if(docCpl==NULL) 
	{
		char buffer[BUF_SIZE]="";
		sprintf( buffer,"parse(%s) is error!\n",cplPath );
		error += buffer;
		printf("parse(%s) is error!\n",cplPath);
		return -2;
	}

	DOMElement *root = docCpl->getDocumentElement();

	//��cpl�ļ������½ڵ���н��� "Id","AnnotationText","IssueDate","Issuer","Creator","ContentTitleText","ContentKind"
	string nameTmp[7]  = {"Id","AnnotationText","IssueDate",
		"Issuer","Creator","ContentTitleText",
		"ContentKind"};
	string *valueTmp[7] = {&cpl.uuid,       &cpl.annotationText,&cpl.issueDate,
		&cpl.issuer,     &cpl.creator,       &cpl.contentTitleText,
		&cpl.contentKind}; 

	for(int i=0;i<7;i++)
	{
		path.clear();path.push_back(nameTmp[i]);
		tmpN = getNodeByXpath(root,path);
		if(tmpN!=NULL)
			*valueTmp[i] = StrX(tmpN->getTextContent()).localForm();
		else
			*valueTmp[i] = "";
		
	}

	//����cpl uuid��ǰ׺"urn:uuid:"
	size_t pos;
	if((pos=cpl.uuid.find("urn:uuid:"))!=string::npos)
		cpl.uuid.erase(pos,9);
	cpl.path = cplPath;
	//cpl.asset.clear();

	//���ǰ��տ��ܳ��ֵ����½ڵ�����ͣ������������������
	string nodeName[4] = { "MainPicture","MainSound","MainSubtitle","MainStereoscopicPicture" }; //"MainMarkers","###other" "msp-cpl:MainStereoscopicPicture"
	string nodeType[4] = { "MainPicture", "MainSound", "MainSubtitle", "MainStereoscopicPicture" }; //"MainMarkers","###other"
	for(int k=0;k<4;k++)
	{
		//DOMNodeList *lMain = docCpl->getElementsByTagName(X(nodeName[k].c_str()));
		DOMNodeList *lMain = docCpl->getElementsByTagNameNS( X("*") , X(nodeName[k].c_str()) );
		int nMain = lMain->getLength();
		//printf("nMain=%d\n",nMain);
		if(nMain!=0)
		{
			//unsigned long long duration = 0lu;
			//unsigned long long fRate = 0lu;
			//Ȼ���������ӽڵ���н���
			asset_cpl_t assetTmp;
			for(int i=0;i<nMain;i++)
			{
				//��������ȡ����ڵ��ֵ����������ڣ������"�մ�"
				string nameTmp[10] ={"Id","AnnotationText","EditRate",
					"IntrinsicDuration","EntryPoint","Duration",
					"KeyId","FrameRate","ScreenAspectRatio","Language"};
				string *valueTmp[10]={&assetTmp.uuid,&assetTmp.annotationText,&assetTmp.editRate,
					&assetTmp.intrinsicDuration,&assetTmp.entryPoint,&assetTmp.duration,
					&assetTmp.keyId,&assetTmp.frameRate,&assetTmp.screenAspectRatio,&assetTmp.language};
				
				for(int j=0;j<10;j++)
				{
					path.clear();path.push_back(nameTmp[j]);
					tmpN = getNodeByXpath(lMain->item(i),path);
					if(tmpN!=NULL)
						*valueTmp[j] = StrX(tmpN->getTextContent()).localForm();
					else
						*valueTmp[j] = "";
					
				}

				//����uuid��ǰ׺"urn:uuid:"
				size_t pos;
				if((pos=assetTmp.uuid.find("urn:uuid:"))!=string::npos)
					assetTmp.uuid.erase(pos,9);
				if((pos=assetTmp.keyId.find("urn:uuid:"))!=string::npos)
					assetTmp.keyId.erase(pos,9);

				//���ڵ�������
				assetTmp.kind = nodeType[k];
				cpl.asset.push_back(assetTmp);
			}
		}
	}
	return 0;
}

int DcpInfo::GetKdmInfo( const std::string& KdmFileName, KdmInfo& kdmInfo, string& error )
{
	int ret = 0;

	if( 0!= init() )
	{
		error = "DcpInfo::init() fail.\n";
		printf("fail to init()!\n");
		return -1;
	}

	//���Ƚ���Kdm�ļ���schema��֤
	vector<string> pathVt;
	pathVt.clear();

	pathVt.push_back("./kdmSchema/etm.xsd");
	pathVt.push_back("./kdmSchema/kdm.xsd");
	pathVt.push_back("./kdmSchema/xml.xsd");
	pathVt.push_back("./kdmSchema/xmldsig-core-schema.xsd");
	pathVt.push_back("./kdmSchema/xenc-schema.xsd");
	pathVt.push_back("./kdmSchema/KDM_schema.xsd");

	if( -1==ParseInit(1,pathVt) )
	{
		error = "Kdm error:parse init is error!\n";
		if(parser!=NULL)parser->release();
		if(output!=NULL)output->release();
		if(serializer!=NULL)serializer->release();
		//XMLPlatformUtils::Terminate();zhangm
		return -1;
	}

	string sFile = KdmFileName;
	if ( ( ret = ParseKdm_New( KdmFileName , kdmInfo ,error ) ) !=0 )
	{
		char buffer[BUF_SIZE]="";
		sprintf( buffer,"parserKdm(%s) is failed!\n", sFile.c_str() );
		error += buffer;
#ifdef _TEST_
			printf( "parserKdm(%s) is failed!\n", sFile.c_str() );
#endif
		return -2;
	}

	return ret;
}
int DcpInfo::GetKdmInfo(  std::string &path, std::vector<KdmInfo> &KdmList, string& error )
{
	char *kdmList[KDM_N_MAX];
	int kdmN = 0;
	int ret = 0;

	if(ret!=init()) 
	{
		error = "DcpInfo::init() fail.\n";
		printf("fail to init()!\n");
		return -1;
	}

	//����ָ��·���µ�kdm�ļ�
	ret = getKdmList( path.c_str(),kdmList,&kdmN,KDM_N_MAX , error );
	if(ret!=0)
	{
#ifdef _TEST_
		printf("fail to getKdmList:%s\n",strerror(ret));
#endif
		while(--kdmN>=0){if(kdmList[kdmN]!=NULL) free(kdmList[kdmN]);}
		if(parser!=NULL)parser->release();
		if(output!=NULL)output->release();
		if(serializer!=NULL)serializer->release();
		//XMLPlatformUtils::Terminate();	zhangm
		return ret;
	}
#if 0
	if(kdmN==0)
	{
#ifdef _TEST_
		printf("there is no kdm file in the USB!\n");
#endif
		
		//while(--kdmN>=0){if(kdmList[kdmN]!=NULL) free(kdmList[kdmN]);}
		return 1;
	}
#endif

	//Kdm�ļ���schema��֤
	vector<string> pathVt;
	pathVt.clear();


	pathVt.push_back("./kdmSchema/etm.xsd");
	pathVt.push_back("./kdmSchema/kdm.xsd");
	pathVt.push_back("./kdmSchema/xml.xsd");
	pathVt.push_back("./kdmSchema/xmldsig-core-schema.xsd");
	pathVt.push_back("./kdmSchema/xenc-schema.xsd");
	pathVt.push_back("./kdmSchema/KDM_schema.xsd");

	if(-1==ParseInit(1,pathVt))
	{
		error = "Kdm error:parse init is error!\n";
		if(parser!=NULL)parser->release();
		if(output!=NULL)output->release();
		if(serializer!=NULL)serializer->release();
		//XMLPlatformUtils::Terminate();	zhangm
		return -1;
	}

	//�����������ļ��������
	for(int i=0;i<kdmN;i++)
	{
		KdmInfo kdmInfo;
		string sFile = kdmList[i];
		if ( ( ret = ParseKdm_New( sFile , kdmInfo , error ) ) !=0 )
		{
#ifdef _TEST_
			printf( "parserKdm(%s) is failed!\n", sFile.c_str() );
#endif
			continue;
			//return 2;
		}
		KdmList.push_back( kdmInfo );
	}

	//release kdmList
	while(--kdmN>=0)
	{ 
		if( kdmList[kdmN] != NULL )
		{
			free(kdmList[kdmN]);
			kdmList[kdmN] = NULL;
		}
	}

	if(parser!=NULL)parser->release();
	if(output!=NULL)output->release();
	if(serializer!=NULL)serializer->release();
	//XMLPlatformUtils::Terminate();	zhangm
	if (KdmList.size()==0)
	{
		error = "error:KdmList is Null;Kdm no found!";
		ret = -2;
		return ret;
	}
	return 0;
}

int DcpInfo::ParseKdm_New( string sFile  , KdmInfo& kdmInfo, string& error )
{
	DOMDocument *docKdm = ParseXML(sFile.c_str());
	if(docKdm==NULL)
	{
		char buffer[BUF_SIZE]="";
		sprintf( buffer,"parse(%s) is error!\n",sFile.c_str() );
		error = buffer;
		return -1;
	}

	string fileName = sFile.substr( sFile.rfind(DIR_SEPCHAR)+1 );
    string path = sFile.substr( 0 , sFile.rfind(DIR_SEPCHAR)+1 );      //�����Ǵ����ļ�·����ftp url
	string kdmId;
	string cplUuid;
	string playerSn;
	string startTime;
	string endTime;
	
	XMLCh xmlC[1024] = {0};  
	DOMNodeList *nlist = NULL;
	
	//������kdmId
	nlist = docKdm->getElementsByTagName(X("MessageId"));
	if(nlist==NULL) 
		return 1;
	
	//@author zhangmiao@oristartech.com
	//@date 2012-12-21
	//@brief 
	//@new
	XMLSize_t nLength = nlist->getLength();
	if( nLength == 0 ) 
	{
		error = "not find the E of MessageId!\n";
		printf("not find the E of MessageId!\n");
		return 1;
	}
	//@modify end;

	XMLString::subString(xmlC,nlist->item(0)->getTextContent(),9,45);
	kdmId = StrX(xmlC).localForm();

	Remove_PrefixOfUuid(kdmId); 

	//������cplUuid
	nlist = docKdm->getElementsByTagName(X("CompositionPlaylistId"));
	if(nlist==NULL) 
		return 1;
	if(0==nlist->getLength()) 
	{
		error = "not find the E of CompositionPlaylistId!\n";
		printf("not find the E of CompositionPlaylistId!\n");
		return 1;
	}
	memset(xmlC,0,sizeof(XMLCh)*1024);
	XMLString::subString(xmlC,nlist->item(0)->getTextContent(),9,45);
	cplUuid = StrX(xmlC).localForm();

	Remove_PrefixOfUuid(cplUuid); 

	//����kdm����ʼʱ��
	nlist = docKdm->getElementsByTagName(X("ContentKeysNotValidBefore"));
	if(nlist==NULL) 
		return 1;
	if(0==nlist->getLength()) 
	{
		error = "not find the E of ContentKeysNotValidBefore!\n";
		printf("not find the E of ContentKeysNotValidBefore!\n");
		return 1;
	}
	memset(xmlC,0,sizeof(XMLCh)*1024);
	XMLString::copyString(xmlC,nlist->item(0)->getTextContent());
    startTime = StrX(xmlC).localForm();

	//@author zhangmiao@oristartech.com
	//@date 2012-10-23
	//@brief ��ʱ���ַ������д���"2009-07-04T10:00:00+09:00"��ת��Ϊ"2009-07-04 10:00:00"����ʱ����ʱ��
	//@new
	/*string::size_type pos = 0;
	if( (pos = startTime.rfind('+')) != string::npos || (pos = startTime.rfind('-')) != string::npos )
	{
		startTime.erase(pos);
		pos = startTime.find('T');
		if(pos != string::npos)
			startTime.replace(pos,1," ");
	}*/
	//@modify end;

	//@author zhangmiao@oristartech.com
	//@date 2014-10-08
	//@brief �����µ�ʱ��������룬Ĭ��ת��Ϊ����ʱ��+08:00����:"2009-07-04T10:00:00+09:00"��ת��Ϊ"2009-07-04 09:00:00"��
	//@new
	int ret = 0;
	string sDestTimeStr_startTime;
	ret = ConvertTimeZoneInfo( startTime , "+08:00" , sDestTimeStr_startTime );
	startTime = sDestTimeStr_startTime;
	//@modify(ʱ������)end;
	

	//����kdm����ֹʱ��
	nlist = docKdm->getElementsByTagName(X("ContentKeysNotValidAfter"));
	if(nlist==NULL) 
		return 1;
	if(0==nlist->getLength()) 
	{
		error = "not find the E of ContentKeysNotValidAfter!\n";
		printf("not find the E of ContentKeysNotValidAfter!\n");
		return 1;
	}
	memset(xmlC,0,sizeof(XMLCh)*1024);
	XMLString::copyString(xmlC,nlist->item(0)->getTextContent());
	endTime = StrX(xmlC).localForm();

	//@author zhangmiao@oristartech.com
	//@date 2012-10-23
	//@brief ��ʱ���ַ������д�����ʱ����ʱ��
	//@new
	/*pos = 0;
	if( (pos = endTime.rfind('+')) != string::npos || (pos = endTime.rfind('-')) != string::npos )
	{
		endTime.erase(pos);
		pos = endTime.find('T');
		if(pos != string::npos)
			endTime.replace(pos,1," ");
	}*/
	//@modify end;

	//@author zhangmiao@oristartech.com
	//@date 2014-10-08
	//@brief �����µ�ʱ��������룬Ĭ��ת��Ϊ����ʱ��+08:00����:"2009-07-04T10:00:00+09:00"��ת��Ϊ"2009-07-04 09:00:00"��
	//@new
	string sDestTimeStr_endTime;
	ret = ConvertTimeZoneInfo( endTime ,"+08:00", sDestTimeStr_endTime );
	endTime = sDestTimeStr_endTime;
	//@modify(ʱ������)end;

	//�������<DeviceListIdentifier> �豸ID
	nlist = docKdm->getElementsByTagName(X("DeviceListIdentifier"));
	if(nlist==NULL) 
		return 1;
	if(0==nlist->getLength()) 
	{
		error = "not find the E of DeviceListIdentifier!\n";
		printf("not find the E of DeviceListIdentifier!\n");
		//@author zhangmiao@oristartech.com
		//@date 2013-04-02
		//@brief ����ñ��<DeviceListIdentifier>û���ҵ������Ժ��ԡ�
		//modify
		//return 1;
		//@modify end;
	}
	//@author zhangmiao@oristartech.com
	//@date 2013-04-02
	//@brief ����ñ��<DeviceListIdentifier>�ҵ��������������
	//@new
	else
	{
		memset(xmlC,0,sizeof(XMLCh)*1024);
		XMLString::subString(xmlC,nlist->item(0)->getTextContent(),9,45);
		playerSn = StrX(xmlC).localForm();

		Remove_PrefixOfUuid(playerSn); 
	}
	//@modify end;

	// X509SubjectName
	nlist = docKdm->getElementsByTagName(X("X509SubjectName"));
	if(nlist==NULL) 
		return 1;
	if(0==nlist->getLength()) 
	{
		error = "not find the E of X509SubjectName!\n";
		printf("not find the E of X509SubjectName!\n");
		return 1;
	}
	memset(xmlC,0,sizeof(XMLCh)*1024);
	XMLString::copyString(xmlC,nlist->item(0)->getTextContent());
	string deviceTypeNumber = StrX(xmlC).localForm();
	if (deviceTypeNumber.size())
	{
		//����deviceTypeNumber
		size_t pos;
		if( ( pos = deviceTypeNumber.find("CN=") ) != string::npos )
		{
			//�ҵ���,������ '.'
			string deviceTypeNumber1 = deviceTypeNumber.substr(pos+3);
			pos = deviceTypeNumber1.find(",");
			if( pos != string::npos )
			{
				deviceTypeNumber1 = deviceTypeNumber1.substr(0,pos);
			}
			else
			{
				deviceTypeNumber1 = deviceTypeNumber1.substr(0);
			}
			deviceTypeNumber = deviceTypeNumber1;
			//ȥ��ĩβ���ַ� '.'
			pos = deviceTypeNumber.rfind('.');
			if( pos != string::npos )
			{
				if( pos == deviceTypeNumber.size()-1 )
					deviceTypeNumber.erase(pos);
			}
		}
		else
		{
			error = "No 'CN=' the Elem of X509SubjectName!\n";
			printf("No 'CN=' the Elem of X509SubjectName!\n");
			return 1;
		}
	}
	else
	{
		error = "empty the Elem of X509SubjectName!\n";
		printf("empty the Elem of X509SubjectName!\n");
		return 1;
	}

	kdmInfo.filename = fileName;
	kdmInfo.path = path;
	kdmInfo.cplUuid = cplUuid;
	kdmInfo.kdmUuid = kdmId;
	kdmInfo.playerSn = playerSn;
	kdmInfo.startTime = startTime;
    kdmInfo.endTime = endTime;
    kdmInfo.deviceTypeNumber = deviceTypeNumber;
    kdmInfo.status = 0;

	return 0;
}
//---zhangmiao:end---20120605----

//----zhangmiao---begin---20120615----
int DcpInfo::CreateDCPInfo(string srcPath ,string& error )
{   
	char *assetmapList[ASSETMAP_N_MAX];
	int assetmapN = 0;
	int ret = 0;

	if(ret!=init())
	{
		error = "DcpInfo::init() fail.\n";
		return ret;
	}

	bool bIsFileName = false;
	if ( srcPath.rfind(DIR_SEPCHAR) != (srcPath.length()-1) )
	{
		bIsFileName = true;
	}
	
	if(srcPath.rfind(DIR_SEPCHAR)==srcPath.length()-1)
		srcPath.erase(srcPath.length()-1);

	
	if ( bIsFileName )
	{
		if( assetmapN >= ASSETMAP_N_MAX )
		{
#ifdef _TEST_
			printf( "nfile=%d is bigger than max!\n", assetmapN );
#endif
			char buffer[BUF_SIZE]="";
			sprintf( buffer,"nfile=%d is bigger than max!\n", assetmapN );
			error = buffer;
			//if(*nfile>0) while(--(*nfile)>=0) free(filelist[*nfile]);
			ret = -2;
		}
		assetmapList[assetmapN] = (char *)malloc(PATH_MAX);
		sprintf(assetmapList[assetmapN],"%s",srcPath.c_str());
		//printf("%s,%d\n",filelist[*nfile],*nfile);
		assetmapN++;
	}
	else
	{
		ret = getAssetmapList(srcPath.c_str(),assetmapList,&assetmapN,ASSETMAP_N_MAX , error );
	}


	if(ret!=0)
	{
		char buffer[BUF_SIZE]="";
		sprintf( buffer,"fail to getAssetmapList:%s\n", strerror(ret) );
		error = buffer;
		//printf("fail to getAssetmapList:%s\n",strerror(ret));
		for(int i=0;i<assetmapN;i++) free(assetmapList[i]);
		if(parser!=NULL)parser->release();
		//if(docRet!=NULL)docRet->release();
		if(output!=NULL)output->release();
		if(serializer!=NULL)serializer->release();
		//XMLPlatformUtils::Terminate();	zhangm
		return ret;
	}
#if 0
	if(assetmapN==0)
	{
		printf("did not get the ASSETMAP list!\n");
		return -1;
	}
#endif
	//printf("assetmapN=%d\n",assetmapN);
	assetmapIdVt.clear();
	for(int i=0;i<assetmapN;i++)
	{
		AMInfo_t aAMInfo;

		//std::cout << assetmapList[i] << std::endl;
		//����Assetmap�ļ�
		ret = Parse_Assetmap_New( assetmapList[i], aAMInfo , error );
		if(ret==-1)
		{
			printf("parseAssetmap error!:%s\n",assetmapList[i]);
			//printf("parseAssetmap error!\n");
			continue;
		}
		vector<dcp_Info_t> vDcp;

		for(int j=0;j<pklN;j++)
		{
			dcp_Info_t dcp;

			//std::cout << pklList[j]->path << std::endl;
			//����PKL�ļ�
			ret = 0;
			ret = Parse_Pkl_New( pklList[j], dcp , error );
			if( ret == -1 )
			{
				printf( "parsePkl error!:%s\n" , ( pklList[j]->path ).c_str() );

				for(int i=0;PKLInfo::num!=0;i++){delete pklList[i];pklList[i]=NULL;}
				for(int i=0;i<assetmapN;i++) free(assetmapList[i]);
				if(parser!=NULL)parser->release();
				//if(docRet!=NULL)docRet->release();
				if(output!=NULL)output->release();
				if(serializer!=NULL)serializer->release();
				//XMLPlatformUtils::Terminate();	zhangm
				return -1;
			}
			//----zhangmiao:begin---2012-12-11----
			//���PKL�ļ������ڣ�����ԣ�����������һ��pkl�ļ���
			else if( ret == 1 )
			{
				continue;
			}
			//----zhangmiao:end----2012-12-11----

			//@author zhangmiao@oristartech.com
			//@date [2015-04-17]
			//@brief ��Ҫ���ӶԵ�Ӱ����ʽ�ֶεĴ������
			//@new
			dcp.aPkl.nFilmPackageFormat = aAMInfo.nFilmPackageFormat;
			//@modify end;

			//if(r!=0){printf("parsePkl error!:%s\n",pklList[j]);}

			//����cpl�ļ�
			int pklAssetN = dcp.aPkl.asset.size();
			//@author zhangmiao@oristartech.com
			//@date [2013-09-22]
			//@brief ��Ҫ�����˶�SMPTE��ʽ�Ĵ�����룬Ŀǰ֧����Ļ�ļ�
			//@new
			//vector<asset_pkl_t> asset_pkl_Vt = dcp.aPkl.asset;
			vector<asset_pkl_t>& asset_pkl_Vt = dcp.aPkl.asset;
			//@modify end;
			for( int k = 0; k < pklAssetN; k++ )
			{
				cpl_Info_t cpl;

				//@author zhangmiao@oristartech.com
				//@date [2013-09-22]
				//@brief ��Ҫ�����˶�SMPTE��ʽ�Ĵ�����룬Ŀǰ֧����Ļ�ļ�
				//@new
				//asset_pkl_t asset_pkl_t = asset_pkl_Vt[k];
				asset_pkl_t& asset_pkl_t = asset_pkl_Vt[k];
				//@modify end;
				string typeValue = asset_pkl_t.type;
				if ( typeValue!="text/xml;asdcpKind=CPL" )       //if(0!=typeValue.compare("text/xml;asdcpKind=CPL")) continue;
				{
					//@author zhangmiao@oristartech.com
					//@date [2013-09-18]
					//@brief ��Ҫ�����˶�SMPTE��ʽ�Ĵ������
					//@new
					//continue;
					if( typeValue == "text/xml" )
					{
						printf("This PKL is SMPTE Format file!\n");

						int IsCplFile=0;
						ret = VerifyIsCplFile4Pkl_Asset( asset_pkl_t.originalFileName.c_str() , IsCplFile , error );
						if ( ret < 0)
						{
							continue;
						}
						
						if ( IsCplFile ==1 )
						{
							asset_pkl_t.type = "text/xml;asdcpKind=CPL";
						}
						else //����Ļ�ļ�
						{
							//����asdcp����
							asset_pkl_t.type = "text/xml;asdcpKind=Subtitle";
							continue;
						}
						
					}
					else
					{
						continue;
					}
					//@modify end;
				}
				string cplPath = asset_pkl_t.originalFileName;
				if (Parse_Cpl_New( cplPath.c_str() , cpl , error )<0)
				{
					printf("parseCpl(%s) is error!\n",cplPath.c_str());
					continue;
				}

				/*bool bIsInsertCpl=false;
				int nSize_vDcp = vDcp.size();
				for(int i1=0;i1<nSize_vDcp;i1++)
				{
					int nSize_vCpl = vDcp[i1].vCpl.size();
                    for(int j1=0;j1<nSize_vCpl;j1++)
						if(vDcp[i1].vCpl[j1].issueDate==cpl.issueDate &&
							vDcp[i1].vCpl[j1].issuer   == cpl.issuer &&
							vDcp[i1].vCpl[j1].creator  == cpl.creator)
						{
							vDcp[i1].vCpl.push_back(cpl);
							bIsInsertCpl =true;
							
							break;
						}
						if (bIsInsertCpl)
						{
							break;
						}
						
				}
					

				if ( bIsInsertCpl )
				{
					continue;
				}*/


				//char uuid[UUID_LEN] = {0};
				//printf("getPkkuuid:%s\n",cpl.uuid.c_str());
				//if(0!=getPklUuid(cpl.uuid.c_str(),uuid,sfd))        
				//        getUuid(uuid,UUID_LEN);
				//	dcp.dcpUuid = uuid;
				//printf("getPkkuuid over:%s\n",uuid);
				//cout << "uuid="<< dcp.dcpUuid << endl;
				//dcp.dcpPath = "doby" + cpl.annotationText + ".pkl.xml"
				/*dcp.dcpSource = cpl.path.substr(0,cpl.path.rfind(DIR_SEPCHAR)+1);
				dcp.annotationText = cpl.contentTitleText; 
				dcp.issueDate = cpl.issueDate;
				dcp.issuer = cpl.issuer;
				dcp.creator = cpl.creator;*/
				dcp.vCpl.push_back(cpl);
				
			}

			//@author zhangmiao@oristartech.com
			//@date [2013-07-15]
			//���cpl�б�Ϊ�գ����ǲ�����
			if ( dcp.vCpl.size() == 0 ) 
				continue;
			//@modify end;
			
			vDcp.push_back(dcp);

		}
		for(int k=0;PKLInfo::num!=0;k++)
		{ 
			delete pklList[k];
			pklList[k]=NULL;
		}
		
		//vDcp.push_back(dcp);

		//@author zhangmiao@oristartech.com
		//@date [2013-07-15]
		//���vDcp�б�Ϊ�գ����ǲ�����
		if ( vDcp.size() == 0 ) 
			continue;
		//@modify end;

		aAMInfo.vDcp = vDcp;
		vAMInfo_t.push_back(aAMInfo);
	}

	return 0;
}
//----zhangmiao:end--20120615------

//@author zhangmiao@oristartech.com
//@date [2013-09-22]
//@brief ��Ҫ�����˶�SMPTE��ʽ�Ĵ������
//@new
//��֤�Ƿ���cpl�ļ�
//IsCplFile��0 = ����cpl�ļ���1 = ��cpl�ļ���
int DcpInfo::VerifyIsCplFile4Pkl_Asset( const char* Pkl_AssetPath, int& IsCplFile , string& error )
{
	if( 0!= init() )
	{
		error += "DcpInfo::init() fail.\n";
		printf("fail to init()!\n");
		return -1;
	}

	std::vector<std::string> path;
	
	//����cpl��schema��֤

	path.push_back("./cplSchema/xmldsig-core-schema.xsd");
	path.push_back("./cplSchema/xml.xsd");
	path.push_back("./cplSchema/CPL_schema.xsd");
	path.push_back("./cplSchema/SMPTE-429-7-2006-CPL.xsd");
	path.push_back("./cplSchema/SMPTE-429-10-2008-Main-Stereo-Picture-CPL.xsd");
	path.push_back("./cplSchema/Asdcp-Main-Stereo-Picture-CPL.xsd");


	if( ParseInit(1,path) == -1 )
	{
		error += "Cpl error:parse init is error!\n";
		printf("parse init is error!\n");
		return -1;
	}

	string strXml = Pkl_AssetPath;
	DOMDocument *doc = ParseXML(strXml);
	if( doc == NULL ) 
	{
		IsCplFile = false;

		char buffer[BUF_SIZE]="";
		sprintf( buffer,"parse(%s) is error!\n",Pkl_AssetPath );
		error += buffer;
		printf("parse(%s) is error!\n",Pkl_AssetPath);

		return 0;
	}
	else
	{
		IsCplFile = true;
	}
	return 0;
}
//@modify end;

//--------------------------------------------
DOMCountErrorHandler::DOMCountErrorHandler():fSawErrors(false)
{
}

DOMCountErrorHandler::~DOMCountErrorHandler()
{
}
// ---------------------------------------------------------------------------
//  DOMCountHandlers: Overrides of the DOM ErrorHandler interface
// ---------------------------------------------------------------------------
bool DOMCountErrorHandler::handleError(const DOMError& domError)
{
    fSawErrors = true;
#if 1
    if (domError.getSeverity() == DOMError::DOM_SEVERITY_WARNING)
        XERCES_STD_QUALIFIER cerr << "\nWarning at file ";
    else if (domError.getSeverity() == DOMError::DOM_SEVERITY_ERROR)
        XERCES_STD_QUALIFIER cerr << "\nError at file ";
    else
        XERCES_STD_QUALIFIER cerr << "\nFatal Error at file ";

    XERCES_STD_QUALIFIER cerr << StrX(domError.getLocation()->getURI())
         << ", line " << domError.getLocation()->getLineNumber()
         << ", char " << domError.getLocation()->getColumnNumber()
         << "\n  Message: " << StrX(domError.getMessage()) << XERCES_STD_QUALIFIER endl;
#endif
    return true;
}

void DOMCountErrorHandler::resetErrors()
{
    fSawErrors = false;
}

//@author zhangmiao@oristartech.com
//@date [2014-04-04]
//@brief ��Ҫ����д������־�Ĵ���
//@new
#if defined(WRITE_DCP_ELOG) 
#ifndef TMS20_LOG
#include "log/C_LogManage.h"
#endif
#endif
int DcpInfo::WriteErrorLog( const std::string& strError )
{
	int ret = 0;
	//int iLevel=1, iModule=19, iSubModule=0;
	//int errorCode = 0;
#ifdef WRITE_DCP_ELOG
	ret = C_LogManage::GetInstance()->WriteLog( iLevel,  iModule, iSubModule, errorCode, strError);
#endif // WRITE_ELOG
	return ret;
}
//@modify end;
