/*******************************************************************************
  Copyright (C), 2013 TP-LINK TECHNOLOGIES CO., LTD.
  File name:   Ycodec.h
  Description: ��ȡ�豸��Ϣ�õ���unicode ����ת��
  Author:      Ren Jian <renjian@tp-link.net>
  History:
  ---------------------------------
  V1.0, 2015-01-16, renjian      create file.
*******************************************************************************/


#pragma once
#include <iostream>
using namespace std;

class YCodec
{
public:
    YCodec();
    ~YCodec();
    // char to hex code
    int Char2Hex(char ch);
    // hex to char
    char Hex2Char(unsigned int n);
    // num^<span style="background-color: rgb(255, 255, 255); ">index</span>
    long MIndex(int num, int index);
    // string to hex code
    long String2Hex(char* string, int strlen);
    // hex to char*
    int Hex2String(long hex, char* string, int* slen);
    // hex to wchar_t code
    wchar_t Hex2Wchart(long hex);
    // UNICODEʮ�������ַ���ת����Ӣ��
    // hex string to wchar_t*
    int HexStr2WChars(char* hexstr, int hexstrlen, int eachchar, wchar_t* wchs, int* wchslen);
    // wchar_t to hex code
    long Wchart2Hex(wchar_t wch);
    // ��Ӣ��ת��UNICODEʮ�������ַ���
    // wchar_t* to char*(hex string)
    int Wchart2HexStr(wchar_t* wchs, int wchslen, int eachchar, char* hexstr, int* hexstrlen);

};
