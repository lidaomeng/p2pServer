#import "stlvector.h"

struct ns1__InterfaceParameter
{
	std::string uuid;
	std::string ftpUserName;
	std::string ftpIP;
	std::string ftpPassWord;
	std::string ftpDirName;
	std::string torrentP2pPath;
	std::string packageName;
	std::string packageSize;
	std::string taskType;
	std::string transferTempPath;
	std::string transferStorePath;
	std::string transferTorrentPath;
};

int ns1__isPackExist(struct ns1__InterfaceParameter param, bool &ret);
int ns1__transfer(struct ns1__InterfaceParameter param, bool &ret);
int ns1__cutPackage(struct ns1__InterfaceParameter param, bool &ret);
int ns1__getUsableSpace(struct ns1__InterfaceParameter param, std::string &ret);
int ns1__deletePackage(struct ns1__InterfaceParameter param, bool &ret);
int ns1__getTempPackage(struct ns1__InterfaceParameter param, std::vector<std::string> &ret);
int ns1__maketo(struct ns1__InterfaceParameter param, bool &ret);
