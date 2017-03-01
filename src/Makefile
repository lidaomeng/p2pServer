rely1 =   -I/usr/local/xerces-c/include -I/usr/local/sqlite/include
relyLib = -lpthread  -luuid  -lxerces-c -lsqlite3

COMMONPATH = ./

						 
CC=g++
CFLAGS=-Wall -Wno-switch -O3 -D_TEST_ -D_FILE_OFFSET_BITS=64 $(rely1)  -L/usr/local/lib/  -L/usr/local/xerces-c/lib -L/usr/local/sqlite/lib
TARGET = p2pServer

# debug option, default is no debug
DEBUG=no
ifeq "$(MAKECMDGOALS)" "debug"
DEBUG=yes
endif
ifeq "$(DEBUG)" "yes"
debug : $(TARGET)
endif
ifeq "$(DEBUG)" "yes"
FLAGS= -g 
else
FLAGS=  
endif

FLAGS= -g 

debug = -g

source = DcpInfo.cpp PKLInfo.cpp fun.cpp HashInfo.cpp\
	sha1.cpp  base64.cpp FtpClient.cpp content.cpp \
	soapC.cpp soapServer.cpp stdsoap2.cpp soapserverprocess.cpp soapserverfunction.cpp soapClient.cpp \
	Popen.cpp config.cpp LogManage.cpp rmDir.cpp

SATEPATH = .
CONTENT_PATH = .
SOURCE  := $(patsubst %.cpp, $(CONTENT_PATH)/%.cpp, $(source) )

source2 = C_SatXmlParser.cpp C_Satellite.cpp crc32.cpp 
SOURCE += $(patsubst %.cpp, $(SATEPATH)/%.cpp, $(source2) )		

SOURCE += $(COMMONPATH)C_TcpSocket.cpp 


ALL: $(TARGET)

$(TARGET):
	$(CC) $(FLAGS) $(CFLAGS) -o $(TARGET) main.cpp  $(SOURCE)   $(relyLib) 


clean:
	rm *.o -f
	rm -f $(TARGET)
