/*
  DateTime.cpp - Arduino Date and Time library
  Copyright (c) Michael Margolis.  All right reserved.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
*/

extern "C" {
  // AVR LibC Includes
}

#include "RSAAuth.h"


SoftwareSerial swSer(4, 5, false, 256);
RSAAuth::RSAAuth()
{
   
}

byte RSAAuth:: encrypt(long key_e,long n,long m)
{
  long k=1;
    for (int j=0;j<key_e;j++) {
      k=k*m;
      k=k%n;
    }   
    return (byte)k;
}
byte RSAAuth:: decrypt(long key_d,long n,long en)
{
  long k;
  
    k=1;
    for (int j=0;j<key_d;j++) {
      k=k*en;
      k=k%n;
    }   
    return (byte)k;
}
int RSAAuth:: encrypt(char MSG[],byte en[])
{
  
  byte  pt;
  int i=0,j,len;
  len=strlen(MSG);
  while(i!=len) {
    pt=MSG[i];
    j=i%NoK;        
    en[i]=encrypt(KEYs[j].e,KEYs[j].n,pt);
    i++;
  }
  //en[i]=255;
  return len;
}

void RSAAuth:: decrypt(byte en[],byte m[],int len)
{
  
  char msg[50];
  int i=0,j;
  
  while(i<len) {
    j=i%NoK;
    m[i]=decrypt(KEYs[j].d,KEYs[j].n,en[i]);
    i++;
  }
  //m[i]=0;
  //Serial.println("THE DECRYPTED MESSAGE IS");
  
  
}
byte RSAAuth::GenCrc(byte en[],int n)
{
  long tong=0;
  for(int i=0;i<n;i++)
    tong%=253;
  return (byte)tong;
}
int RSAAuth::Package(byte en[],int n,byte pac[])
{
  int i;
  byte crc=GenCrc(en,n);
  pac[0]=255;
  for(i=0;i<n;i++)
  {
    pac[i+1]=(byte)en[i];
  }
  pac[i+1]=crc;
  pac[i+2]=254;
  return n+3;
}
int RSAAuth::OpenPackage(byte pac[],int m,byte en[])
{
  if(pac[0]!=255||pac[m-1]!=254) return -1;
  byte crc=GenCrc(pac+1,m-3);
  if(crc!=pac[m-2]) return -1;
  for(int i=1;i<m-2;i++)
    en[i-1]=pac[i];
  return m-3;
}
bool RSAAuth::checkArray(byte arr[],int*BEGIN,int*END)
{
  bool dau=false,cuoi=false;
  for(int i=*BEGIN;i!=*END;i++,i%=255)
  {
    
    if(arr[i]==255) dau=true;
    if(arr[i]==254 && dau==true) return true;
  }
  return false;
}
bool RSAAuth::CutString(byte arr[],int*BEGIN,int*END,byte desArr[],int *pDesLen)
{
  
  if(checkArray(arr,BEGIN,END)==true)
  {    
    //Serial.println("Nhan:");
      //Serial.println(*BEGIN);
      //Serial.println(*END);
    while(arr[*BEGIN]!=255) //start
    {
      (*BEGIN)++;
      (*BEGIN)%=255;
    }
          
    *pDesLen=0;
    while(arr[*BEGIN]!=254)//end
    {
      desArr[(*pDesLen)++]=arr[(*BEGIN)++];
      (*BEGIN)%=255;
    }
      
    desArr[(*pDesLen)++]=arr[(*BEGIN)++];    
    (*BEGIN)%=255;
    return true;
  }
  return false;
}
void RSAAuth::Send(char cstr[])
{
  
  
  int len=encrypt(cstr,en);
  len=Package(en,len,pac);  
  int i=0;
  while (i < len) {
    swSer.write(pac[i++]);
    yield();
  }
  
}

bool RSAAuth::Receive(byte buff[],int*BEGIN,int*END,byte m[],int *pDesLen)
{
   while (swSer.available() > 0&&(*END+1)%255!=*BEGIN) {    
      buff[(*END)++]=swSer.read(); 
      (*END)%=255;             
      yield();
  }
  if(CutString(buff,BEGIN,END,desArr,pDesLen))
  {    
    *pDesLen=OpenPackage(desArr,*pDesLen,unpac);
    if(*pDesLen>=0)
    {
      desArr[*pDesLen]=0;
      rsa.decrypt(unpac,m,*pDesLen);
      m[*pDesLen]=0;
      //Serial.println("Nhan:");
      //Serial.println((char*)m);      
      return true;
    }
    else return false;//Serial.println("Nhan error:");
  }
  return true;
}
RSAAuth rsa = RSAAuth() ;
