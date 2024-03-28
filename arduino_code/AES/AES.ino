#include <WiFi.h>
#include <WebServer.h>
#include <mDNS.h>
#include <ArduinoJson.h>
#include <iostream>
#include <math.h>
using namespace std;

const char *ssid = "AC-ESP32";
const char *password = "987654321";
//int count1=0;
unsigned int d=0;
unsigned int d0=0; unsigned int d1=0; unsigned int d2=0; unsigned int d3=0;
unsigned int s0=0; unsigned int s1=0; unsigned int s2=0; unsigned int s3=0;
unsigned int s4=0; unsigned int s5=0; unsigned int s6=0; unsigned int s7=0;
unsigned int s8=0; unsigned int s9=0; unsigned int s10=0; unsigned int s11=0;
unsigned int s12=0; unsigned int s13=0; unsigned int s14=0; unsigned int s15=0;
unsigned  int m[16] = {0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a, 0x30, 0x8d, 0x31, 0x31, 0x98, 0xa2, 0xe0, 0x37, 0x07, 0x34};
unsigned  int key[16] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
unsigned int state[4] = {0,0,0,0};
int count2=0;
IPAddress local_IP(192,168,4,22);
IPAddress gateway(192,168,4,9);
IPAddress subnet(255,255,255,0);

WebServer server(80);  //Define server object

unsigned int GFM(unsigned int a,unsigned int b) 
{
	int m=8;
	int c=0;
	int p=27;
	int mask;
	mask=pow(2,m)-1;
	for (int i=m-1;i>0;i=i-1)
	{
		c=c^(((a>>i)&0x01)*b);
		c=((c<<1)&mask)^(((c>>(m-1))&0x01)*p);	
	}
	c=c^((a&0x01)*b);
	return c;
}	


void S_box_invS_box(unsigned  int s_box[256],unsigned  int invs_box[256])
{
// Making lookup table for s-box and invs-box--
   unsigned int At[8] = {0x8f, 0xc7, 0xe3, 0xf1, 0xf8, 0x7c, 0x3e, 0x1f};
    for (int i=0;i<256;i++)
    {
        int A=i;	
	    // find inverse value ------
        int InA=1;
        for (int j=0;j<254;j++) 
            InA = GFM(InA,A);
        int b = InA;
        //b = 0x53  01010011  ->  11001010  CA
        int icb=0;
         for (int k=0;k<8;k++)
            icb = icb << 1 | ((b >> k)&0x01);
        // the affine matrix operation
        int b1=0;int temp=0;
        for (int ii=0;ii<8;ii++)
        {
			temp = At[ii]&icb;
            int sum=0;
            for (int l=0;l<8;l++)
                sum = sum ^ ((temp>>l) & 0x01);
            b1 = b1 | (sum << ii);
        }
        b1 = b1 ^ 0x63;
        s_box[i]=b1;
        invs_box[b1]=i;
		//printf("%d->%x\n",i,s_box[i]);
    }
}

void show(unsigned  int state[])
{
    printf("%.2X %.2X %.2X %.2X \n", state[0] >> 24,state[1] >> 24,state[2] >> 24, state[3] >> 24);
    printf("%.2X %.2X %.2X %.2X \n", (state[0] >> 16) & 0xff, (state[1] >> 16) & 0xff, (state[2] >> 16) & 0xff, (state[3] >> 16) & 0xff);
    printf("%.2X %.2X %.2X %.2X \n", (state[0] >> 8)  & 0xff, (state[1] >> 8)  & 0xff, (state[2] >> 8)  & 0xff, (state[3] >> 8)  & 0xff);
    printf("%.2X %.2X %.2X %.2X \n", (state[0] >> 0)  & 0xff, (state[1] >> 0)  & 0xff, (state[2] >> 0)  & 0xff, (state[3] >> 0)  & 0xff);
}

void rcv(unsigned  int nr,unsigned  int rc[])
{
    int alpha = 1;
    for(int i=0;i<nr;i++)
    {
        rc[i] = alpha << 24;
        alpha= GFM(alpha, 2);
	}
}



unsigned  int rotword(unsigned  int temp)
{
    temp = temp << 8 | temp >> 24;
    return (temp & 0xffffffff);
}


unsigned  int subword(unsigned  int temp,unsigned  int s_box[256])
{	
    temp = s_box[temp >> 24] << 24 | s_box[(temp >> 16)&0xff] << 16 | s_box[(temp >> 8)&0xff] << 8  | s_box[temp&0xff];
    return temp;
}

void keyexpansion(unsigned  int key[],unsigned  int w[],unsigned int nk,unsigned int nr, unsigned int rc[],unsigned  int s_box[256])
{
    for (int i=0; i<nk; i++)
        w[i] = (key[0+4*i] << 24) | (key[1+4*i] << 16) | (key[2+4*i] << 8) | key[3+4*i];
    for (int i=nk; i<(4*(nr+1)); i++)
    {
        int temp = w[i-1];
        if (i % nk == 0)
           temp = subword(rotword(temp), s_box) ^ rc[int(i/4)-1];
        else
        {   
			if (nk > 6 and (i % nk == 4))
             	temp = subword(temp,s_box);
    	}
		w[i] = w[i-nk] ^ temp;
    }
}

void AddRoundKey(unsigned  int state[],unsigned int wlen,unsigned int w[])
{
    state[0] = state[0] ^ w[wlen+0];
    state[1] = state[1] ^ w[wlen+1];
    state[2] = state[2] ^ w[wlen+2];
    state[3] = state[3] ^ w[wlen+3];
}

void SubBytes(unsigned  int state[],unsigned int s_box[256])
{
    state[0] = s_box[(state[0] >> 24)] << 24 | s_box[(state[0] >> 16)&0xff] << 16 | s_box[(state[0] >> 8)&0xff] << 8 | s_box[(state[0] & 0xff)];
    state[1] = s_box[(state[1] >> 24)] << 24 | s_box[(state[1] >> 16)&0xff] << 16 | s_box[(state[1] >> 8)&0xff] << 8 | s_box[(state[1] & 0xff)];
    state[2] = s_box[(state[2] >> 24)] << 24 | s_box[(state[2] >> 16)&0xff] << 16 | s_box[(state[2] >> 8)&0xff] << 8 | s_box[(state[2] & 0xff)];
    state[3] = s_box[(state[3] >> 24)] << 24 | s_box[(state[3] >> 16)&0xff] << 16 | s_box[(state[3] >> 8)&0xff] << 8 | s_box[(state[3] & 0xff)];
}

void invSubBytes(unsigned  int state[],unsigned int invs_box[256])
{
    state[0] = invs_box[(state[0] >> 24)] << 24 | invs_box[(state[0] >> 16)&0xff] << 16 | invs_box[(state[0] >> 8)&0xff] << 8 | invs_box[(state[0] & 0xff)];
    state[1] = invs_box[(state[1] >> 24)] << 24 | invs_box[(state[1] >> 16)&0xff] << 16 | invs_box[(state[1] >> 8)&0xff] << 8 | invs_box[(state[1] & 0xff)];
    state[2] = invs_box[(state[2] >> 24)] << 24 | invs_box[(state[2] >> 16)&0xff] << 16 | invs_box[(state[2] >> 8)&0xff] << 8 | invs_box[(state[2] & 0xff)];
    state[3] = invs_box[(state[3] >> 24)] << 24 | invs_box[(state[3] >> 16)&0xff] << 16 | invs_box[(state[3] >> 8)&0xff] << 8 | invs_box[(state[3] & 0xff)];
}


void ShiftRows(unsigned  int state[])
{
    unsigned int t00 = (state[0] >> 24); unsigned int  t10 = (state[0] >> 16) & 0xff; unsigned int  t20 = (state[0] >> 8) & 0xff; unsigned int  t30 = state[0] & 0xff;
    unsigned int t01 = (state[1] >> 24); unsigned int  t11 = (state[1] >> 16) & 0xff; unsigned int  t21 = (state[1] >> 8) & 0xff; unsigned int  t31 = state[1] & 0xff;
    unsigned int t02 = (state[2] >> 24); unsigned int  t12 = (state[2] >> 16) & 0xff; unsigned int  t22 = (state[2] >> 8) & 0xff; unsigned int  t32 = state[2] & 0xff;
    unsigned int t03 = (state[3] >> 24); unsigned int  t13 = (state[3] >> 16) & 0xff; unsigned int  t23 = (state[3] >> 8) & 0xff; unsigned int  t33 = state[3] & 0xff;
    state[0] = t00 << 24 | t11 << 16 | t22 << 8 | t33;
    state[1] = t01 << 24 | t12 << 16 | t23 << 8 | t30;
    state[2] = t02 << 24 | t13 << 16 | t20 << 8 | t31;
    state[3] = t03 << 24 | t10 << 16 | t21 << 8 | t32;
}

void invShiftRows(unsigned  int state[])
{
    unsigned int t00 = (state[0] >> 24); unsigned int  t10 = (state[0] >> 16) & 0xff; unsigned int  t20 = (state[0] >> 8) & 0xff; unsigned int  t30 = state[0] & 0xff;
    unsigned int t01 = (state[1] >> 24); unsigned int  t11 = (state[1] >> 16) & 0xff; unsigned int  t21 = (state[1] >> 8) & 0xff; unsigned int  t31 = state[1] & 0xff;
    unsigned int t02 = (state[2] >> 24); unsigned int  t12 = (state[2] >> 16) & 0xff; unsigned int  t22 = (state[2] >> 8) & 0xff; unsigned int  t32 = state[2] & 0xff;
    unsigned int t03 = (state[3] >> 24); unsigned int  t13 = (state[3] >> 16) & 0xff; unsigned int  t23 = (state[3] >> 8) & 0xff; unsigned int  t33 = state[3] & 0xff;
    state[0] = t00 << 24 | t13 << 16 | t22 << 8 | t31;
    state[1] = t01 << 24 | t10 << 16 | t23 << 8 | t32;
    state[2] = t02 << 24 | t11 << 16 | t20 << 8 | t33;
    state[3] = t03 << 24 | t12 << 16 | t21 << 8 | t30;
}

void MixColumns(unsigned  int state[])
{
    for(int i=0; i<4; i++)
    {
        unsigned int d0 = state[i] >> 24;
        unsigned int d1 = (state[i] >> 16) & 0xff;
        unsigned int d2 = (state[i] >> 8) & 0xff;
        unsigned int d3 = (state[i] >> 0) & 0xff;
        unsigned int t0 = GFM(2,d0) ^ GFM(3,d1) ^ d2 ^ d3;
        unsigned int t1 = d0 ^ GFM(2,d1) ^ GFM(3,d2) ^ d3;
        unsigned int t2 = d0 ^ d1 ^ GFM(2,d2) ^ GFM(3,d3);
        unsigned int t3 = GFM(3,d0) ^ d1 ^ d2 ^ GFM(2,d3);
        state [i] = t0 << 24 | t1 << 16 | t2 << 8 | t3;
    }
}

void invMixColumns(unsigned  int state[])
{
    for(int i=0; i<4; i++)
    {
      unsigned  int d0 = state[i] >> 24;
      unsigned  int d1 = (state[i] >> 16) & 0xff;
      unsigned  int d2 = (state[i] >> 8) & 0xff;
      unsigned  int d3 = (state[i] >> 0) & 0xff;
      unsigned  int t0 = GFM(0x0e, d0) ^ GFM(0x0b, d1) ^ GFM(0x0d, d2) ^ GFM(0x09, d3);
      unsigned  int t1 = GFM(0x09, d0) ^ GFM(0x0e, d1) ^ GFM(0x0b, d2) ^ GFM(0x0d, d3);
      unsigned  int t2 = GFM(0x0d, d0) ^ GFM(0x09, d1) ^ GFM(0x0e, d2) ^ GFM(0x0b, d3);
      unsigned  int t3 = GFM(0x0b, d0) ^ GFM(0x0d, d1) ^ GFM(0x09, d2) ^ GFM(0x0e, d3);
	    state [i] = t0 << 24 | t1 << 16 | t2 << 8 | t3;
    }
}



void GFH4(unsigned int d[],unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3, unsigned int b0, unsigned int b1, unsigned int b2, unsigned int b3)
{
    //d[4]={0};
    unsigned int t0=b0^b2; unsigned int t1=b1^b3; unsigned int t2=a0^a1; unsigned int t3=GFM(a0,t0^t1);
    unsigned int s0=a0^a2; unsigned int s1=a1^a3; unsigned int s2=GFM(s0,b2^b3); unsigned int s3=s0^s1;
    unsigned int r0=GFM(s0,b0^b1); unsigned int r1=s0^s1;   
    unsigned int f0=t3^GFM(t2,t1); unsigned int f1=t3^GFM(t2,t0);
    unsigned int g0=s2^GFM(s3,b3); unsigned int g1=s2^GFM(s3,b2);
    unsigned int h0=r0^GFM(r1,b1); unsigned int h1=r0^GFM(r1,b0);   
    d[0]=f0^g0;
    d[1]=f1^g1;
    d[2]=f0^h0;
    d[3]=f1^h1;
}

void GFH8(unsigned int d[],unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3, unsigned int a4, unsigned int a5, unsigned int a6, unsigned int a7, unsigned int b0, unsigned int b1, unsigned int b2, unsigned int b3, unsigned int b4, unsigned int b5, unsigned int b6, unsigned int b7)
{
    unsigned int F[4];
	unsigned int G[4];
    unsigned int H[4];
	//d[8]={0};
    unsigned int t0=a0^a4; unsigned int t1=a1^a5; unsigned int t2=a2^a6; unsigned int t3=a3^a7;
    GFH4(F,a0,a1,a2,a3,b0^b4,b1^b5,b2^b6,b3^b7);
    GFH4(G,t0,t1,t2,t3,b4,b5,b6,b7);
    GFH4(H,t0,t1,t2,t3,b0,b1,b2,b3);
    d[0]=F[0]^G[0];
    d[1]=F[1]^G[1];
    d[2]=F[2]^G[2];
    d[3]=F[3]^G[3];
    d[4]=F[0]^H[0];
    d[5]=F[1]^H[1];
    d[6]=F[2]^H[2];
    d[7]=F[3]^H[3];
}

void GFH16(unsigned int d[],unsigned int a0,unsigned int a1,unsigned int a2,unsigned int a3,unsigned int a4,unsigned int a5,unsigned int a6,unsigned int a7,unsigned int a8,unsigned int a9,unsigned int a10,unsigned int a11,unsigned int a12,unsigned int a13,unsigned int a14,unsigned int a15,unsigned int b0,unsigned int b1,unsigned int b2,unsigned int b3,unsigned int b4,unsigned int b5,unsigned int b6,unsigned int b7,unsigned int b8,unsigned int b9,unsigned int b10,unsigned int b11,unsigned int b12,unsigned int b13,unsigned int b14,unsigned int b15)
{
	unsigned int F[8];
	unsigned int G[8];
    unsigned int H[8];
    //d[16]={0};
    unsigned int t0=a0^a8; unsigned int t1=a1^a9; unsigned int t2=a2^a10; unsigned int t3=a3^a11; unsigned int t4=a4^a12; unsigned int t5=a5^a13; unsigned int t6=a6^a14; unsigned int t7=a7^a15;
    GFH8(F,a0,a1,a2,a3,a4,a5,a6,a7,b0^b8,b1^b9,b2^b10,b3^b11,b4^b12,b5^b13,b6^b14,b7^b15);
    GFH8(G,t0,t1,t2,t3,t4,t5,t6,t7,b8,b9,b10,b11,b12,b13,b14,b15);
    GFH8(H,t0,t1,t2,t3,t4,t5,t6,t7,b0,b1,b2,b3,b4,b5,b6,b7);
    d[0]=F[0]^G[0];
    d[1]=F[1]^G[1];
    d[2]=F[2]^G[2];
    d[3]=F[3]^G[3];
    d[4]=F[4]^G[4];
    d[5]=F[5]^G[5];
    d[6]=F[6]^G[6];
    d[7]=F[7]^G[7];
    d[8]=F[0]^H[0];
    d[9]=F[1]^H[1];
    d[10]=F[2]^H[2];
    d[11]=F[3]^H[3];
    d[12]=F[4]^H[4];
    d[13]=F[5]^H[5];
    d[14]=F[6]^H[6];
    d[15]=F[7]^H[7];
}

void MixColumns_InvMix(unsigned int state[])
{
	unsigned int dt[16];
    for(int i=0; i<1; i++)
    {
	    unsigned int d0 = state[i] >> 24;
        unsigned int d1 = (state[i] >> 16) & 0xff;
        unsigned int d2 = (state[i] >> 8) & 0xff;
        unsigned int d3 = (state[i] >> 0) & 0xff;
        unsigned int d4 = state[i+1] >> 24;
        unsigned int d5 = (state[i+1] >> 16) & 0xff;
        unsigned int d6 = (state[i+1] >> 8) & 0xff;
        unsigned int d7 = (state[i+1] >> 0) & 0xff;
        unsigned int d8 = state[i+2] >> 24;
        unsigned int d9 = (state[i+2] >> 16) & 0xff;
        unsigned int d10 = (state[i+2] >> 8) & 0xff;
        unsigned int d11 = (state[i+2] >> 0) & 0xff;
        unsigned int d12 = state[i+3] >> 24;
        unsigned int d13 = (state[i+3] >> 16) & 0xff;
        unsigned int d14 = (state[i+3] >> 8) & 0xff;
        unsigned int d15 = (state[i+3] >> 0) & 0xff;
        GFH16(dt,1,3,4,5,6,7,8,9,0XA,0XB,0XC,0XD,0XE,0x10,2,0x1E,d0,d1,d2,d3,d4,d5,d6,d7,d8,d9,d10,d11,d12,d13,d14,d15);
        state [i]   = dt[0] << 24 |dt[1] << 16 | dt[2] << 8 | dt[3];
        state [i+1] = dt[4]<< 24 |dt[5] << 16 | dt[6] << 8 | dt[7];
        state [i+2] = dt[8] << 24 | dt[9] << 16 |dt[10] << 8 | dt[11];
        state [i+3] = dt[12] << 24 | dt[13] << 16 | dt[14] << 8 | dt[15];
    }
}

void cipher(unsigned int state[],unsigned int Nr,unsigned  int s_box[256],unsigned int w[45])
{
    AddRoundKey(state, 0,w);
    for (int i=1; i<Nr; i++)
    {
        SubBytes(state,s_box);
        //ShiftRows(state); //111
        //MixColumns(state);//111
        MixColumns_InvMix(state);
        AddRoundKey(state, i*4,w);
    } 
    SubBytes(state,s_box);
    //ShiftRows(state); //111
    AddRoundKey(state, Nr * 4,w);
}
void invcipher(unsigned int state[],unsigned int Nr,unsigned int invs_box[256],unsigned int w[])
{
    AddRoundKey(state, Nr*4,w);
    for (int i=Nr-1; i>0; i--)
    {
        //invShiftRows(state); //111
        invSubBytes(state,invs_box);
        AddRoundKey(state, i*4,w);
        //invMixColumns(state); //111
        MixColumns_InvMix(state);
	}
    //invShiftRows(state); //111
    invSubBytes(state,invs_box);
    AddRoundKey(state, 0,w);
}

void handleRoot() {

  String webPage;  
  // Allocate JsonBuffer
  // Use arduinojson.org/assistant to compute the capacity.
  StaticJsonBuffer<400> jsonBuffer;
  // Create the root object
  JsonObject& root = jsonBuffer.createObject();
  if (count2==1){
    root["d"] = 1;
  }
  if (count2==2){
    root["d"] = 2;
  }
  if (count2==3){
    root["d"] = 3;
  }
  Serial.println("ok");


  //state[0]=0xffffffff;
  s0 = state[0]  >> 24 ;
  Serial.println(s0);
  Serial.println(state[0]);
  s1 = (state[0]  >> 16)&0xff ;
  s2 = (state[0]  >> 8)&0xff ;
  s3 = (state[0]  >> 0)&0xff  ;
  s4 = (state[1]  >> 24)&0xff  ;
  s5 = (state[1]  >> 16)&0xff  ;
  s6 = (state[1]  >> 8)&0xff  ;
  s7 = (state[1]  >> 0)&0xff  ;
  s8 = (state[2]  >> 24)&0xff  ;
  s9 = (state[2]  >> 16)&0xff  ;
  s10 = (state[2]  >> 8)&0xff  ;
  s11 = (state[2]  >> 0)&0xff  ;
  s12 = (state[3]  >> 24)&0xff  ;
  s13 = (state[3]  >> 16)&0xff  ;
  s14 = (state[3]  >> 8)&0xff  ;
  s15 = (state[3]  >> 0)&0xff  ;



  root["d0"] = s0; //Put Sensor value
  root["d1"] = s1; //Reads Flash Button Status
  root["d2"] = s2; //Reads Flash Button Status
  root["d3"] = s3; //Reads Flash Button Status
  root["d4"] = s4; //Put Sensor value
  root["d5"] = s5; //Reads Flash Button Status
  root["d6"] = s6; //Reads Flash Button Status
  root["d7"] = s7;
  root["d8"] = s8; //Put Sensor value
  root["d9"] = s9; //Reads Flash Button Status
  root["d10"] = s10; //Reads Flash Button Status
  root["d11"] = s11;
  root["d12"] = s12; //Put Sensor value
  root["d13"] = s13; //Reads Flash Button Status
  root["d14"] = s14; //Reads Flash Button Status
  root["d15"] = s15;



  // String thestr = String(13, HEX);
  Serial.println(d0);
  Serial.println(d1);
  Serial.println(d2);
  Serial.println(d3);

  webPage = "";

  root.printTo(webPage);  //Store JSON in String variable
  server.send(200, "application/json", webPage);
}

void rightd() {
  digitalWrite(LED_BUILTIN, HIGH);
} 

void leftd() {
  digitalWrite(LED_BUILTIN, LOW);
}



void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  Serial.println();

  Serial.print("Setting soft-AP configuration ... ");
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");

  Serial.print("Setting soft-AP ... ");
  Serial.println(WiFi.softAP(ssid,password) ? "Ready" : "Failed!");
  //WiFi.softAP(ssid);
  //WiFi.softAP(ssid, password, channel, hidden, max_connection)
  
  // Serial.print("Soft-AP IP address = ");
  // Serial.println(WiFi.softAPIP());
  server.on("/", handleRoot);
  //server.on("/leftd", HTTP_POST, leftd);
  //server.on("/rightd", HTTP_POST, rightd);
  server.begin();
  Serial.println("HTTP server started");
	
  /*
	unsigned  int d0 = m[0]  << 24 | m[1]  << 16 | m[2]  << 8 | m[3];
	unsigned  int d1 = m[4]  << 24 | m[5]  << 16 | m[6]  << 8 | m[7];
	unsigned  int d2 = m[8]  << 24 | m[9]  << 16 | m[10] << 8 | m[11];
	unsigned  int d3 = m[12] << 24 | m[13] << 16 | m[14] << 8 | m[15];
  */

}




void loop() {
  //count1++;
  unsigned  int m[16] = {0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a, 0x30, 0x8d, 0x31, 0x31, 0x98, 0xa2, 0xe0, 0x37, 0x07, 0x34};
	unsigned  int key[16] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
	unsigned  int s_box[256]={0}; unsigned  int invs_box[256]={0};unsigned  int nk=4;unsigned  int nr=10; unsigned int rc[nr]={0}; unsigned  int w[ (nr+1) * 4] ={0};
	S_box_invS_box(s_box,invs_box);
  rcv(nr, rc);
	keyexpansion(key, w, nk, nr,rc,s_box);
	printf("--Input message  -----------------------\n");
  d0 = m[0]  << 24 | m[1]  << 16 | m[2]  << 8 | m[3];
  d1 = m[4]  << 24 | m[5]  << 16 | m[6]  << 8 | m[7];
  d2 = m[8]  << 24 | m[9]  << 16 | m[10] << 8 | m[11];
  d3 = m[12] << 24 | m[13] << 16 | m[14] << 8 | m[15];
  state[0] = d0;
  state[1] = d1;
  state[2] = d2;
  state[3] = d3;
  show(state);
  count2=1;
  server.handleClient();
  delay(10000);
  cipher(state, nr,s_box,w);
  Serial.println("--Encryption ---------------------------\n");
  count2=2;
  server.handleClient();
  WiFi.softAPIP();
  show(state);
  delay(10000);
  Serial.println("--Decryption----------------------------\n");
  count2=3;
  invcipher(state, nr,invs_box,w);
  show(state);
  server.handleClient();
  delay(10000);
	/*show(state);*/
  /*delay(10000);*/
	/*Serial.println("--Encryption ---------------------------\n");
	cipher(state, nr,s_box,w);
	show(state);
    delay(10000);
	Serial.println("--Decryption----------------------------\n");
	invcipher(state, nr,invs_box,w);
	show(state);
  delay(10000);*/
  handleRoot();
  
}