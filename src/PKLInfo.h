/*
*@file:PKLInfo.h
*@brief:pkl info �ṹ
*@author: zhangmiao@oristartech.com
*@date: 2012/06/05
*/
#ifndef __PKLINFO_H__
#define __PKLINFO_H__

#include <string>
#include <iostream>

class PKLInfo
{
public:
    PKLInfo();
    ~PKLInfo();
public:
    static int num;
    std::string id;
    std::string path;
};

#endif
