#include <ArduinoJson.h>
#include <iostream>
#include <math.h>
#include <string.h>
using namespace std;

uint64_t d=0;
uint64_t d0=0; uint64_t d1=0; uint64_t d2=0; uint64_t d3=0;
uint64_t s0=0; uint64_t s1=0; uint64_t s2=0; uint64_t s3=0;
uint64_t s4=0; uint64_t s5=0; uint64_t s6=0; uint64_t s7=0;
uint64_t s8=0; uint64_t s9=0; uint64_t s10=0; uint64_t s11=0;
uint64_t s12=0; uint64_t s13=0; uint64_t s14=0; uint64_t s15=0;
uint64_t state[4] = {0,0,0,0};


uint64_t GFM(uint64_t a,uint64_t b) {
	int m=8;
	int c=0;
	int p=27;
	int mask;
	mask=pow(2,m)-1;
	for (int i=m-1;i>0;i=i-1){
		c=c^(((a>>i)&0x01)*b);
		c=((c<<1)&mask)^(((c>>(m-1))&0x01)*p);	
	}
	c=c^((a&0x01)*b);
	return c;
}	


void S_box_invS_box(uint64_t s_box[256],uint64_t invs_box[256]){
  uint64_t At[8] = {0x8f, 0xc7, 0xe3, 0xf1, 0xf8, 0x7c, 0x3e, 0x1f};
  for (int i=0;i<256;i++){
    int A=i;	
    int InA=1;
    for (int j=0;j<254;j++) 
        InA = GFM(InA,A);
    int b = InA;
    int icb=0;
      for (int k=0;k<8;k++)
        icb = icb << 1 | ((b >> k)&0x01);
    int b1=0;int temp=0;
    for (int ii=0;ii<8;ii++){
      temp = At[ii]&icb;
      int sum=0;
      for (int l=0;l<8;l++)
          sum = sum ^ ((temp>>l) & 0x01);
      b1 = b1 | (sum << ii);
    }
    b1 = b1 ^ 0x63;
    s_box[i]=b1;
    invs_box[b1]=i;
  }
}

void show(uint64_t state[]){
  printf("%.2X %.2X %.2X %.2X \n", state[0] >> 24,state[1] >> 24,state[2] >> 24, state[3] >> 24);
  printf("%.2X %.2X %.2X %.2X \n", (state[0] >> 16) & 0xff, (state[1] >> 16) & 0xff, (state[2] >> 16) & 0xff, (state[3] >> 16) & 0xff);
  printf("%.2X %.2X %.2X %.2X \n", (state[0] >> 8)  & 0xff, (state[1] >> 8)  & 0xff, (state[2] >> 8)  & 0xff, (state[3] >> 8)  & 0xff);
  printf("%.2X %.2X %.2X %.2X \n", (state[0] >> 0)  & 0xff, (state[1] >> 0)  & 0xff, (state[2] >> 0)  & 0xff, (state[3] >> 0)  & 0xff);
}

void rcv(uint64_t nr,uint64_t rc[]){
  int alpha = 1;
  for(int i=0;i<nr;i++){
    rc[i] = alpha << 24;
    alpha= GFM(alpha, 2);
	}
}

uint64_t rotword(uint64_t temp){
    temp = temp << 8 | temp >> 24;
    return (temp & 0xffffffff);
}

uint64_t subword(uint64_t temp,uint64_t s_box[256]){	
  temp = s_box[temp >> 24] << 24 | s_box[(temp >> 16)&0xff] << 16 | s_box[(temp >> 8)&0xff] << 8  | s_box[temp&0xff];
  return temp;
}

void keyexpansion(uint64_t key[],uint64_t w[],uint64_t nk,uint64_t nr, uint64_t rc[],uint64_t s_box[256]){
  for (int i=0; i<nk; i++)
    w[i] = (key[0+4*i] << 24) | (key[1+4*i] << 16) | (key[2+4*i] << 8) | key[3+4*i];
  for (int i=nk; i<(4*(nr+1)); i++){
    int temp = w[i-1];
    if (i % nk == 0)
      temp = subword(rotword(temp), s_box) ^ rc[int(i/4)-1];
    else{   
      if (nk > 6 and (i % nk == 4))
        temp = subword(temp,s_box);
    }
    w[i] = w[i-nk] ^ temp;
  }
}

void AddRoundKey(uint64_t state[],uint64_t wlen,uint64_t w[]){
  state[0] = state[0] ^ w[wlen+0];
  state[1] = state[1] ^ w[wlen+1];
  state[2] = state[2] ^ w[wlen+2];
  state[3] = state[3] ^ w[wlen+3];
}

void SubBytes(uint64_t state[],uint64_t s_box[256]){
  state[0] = s_box[(state[0] >> 24)] << 24 | s_box[(state[0] >> 16)&0xff] << 16 | s_box[(state[0] >> 8)&0xff] << 8 | s_box[(state[0] & 0xff)];
  state[1] = s_box[(state[1] >> 24)] << 24 | s_box[(state[1] >> 16)&0xff] << 16 | s_box[(state[1] >> 8)&0xff] << 8 | s_box[(state[1] & 0xff)];
  state[2] = s_box[(state[2] >> 24)] << 24 | s_box[(state[2] >> 16)&0xff] << 16 | s_box[(state[2] >> 8)&0xff] << 8 | s_box[(state[2] & 0xff)];
  state[3] = s_box[(state[3] >> 24)] << 24 | s_box[(state[3] >> 16)&0xff] << 16 | s_box[(state[3] >> 8)&0xff] << 8 | s_box[(state[3] & 0xff)];
}

void invSubBytes(uint64_t state[],uint64_t invs_box[256]){
  state[0] = invs_box[(state[0] >> 24)] << 24 | invs_box[(state[0] >> 16)&0xff] << 16 | invs_box[(state[0] >> 8)&0xff] << 8 | invs_box[(state[0] & 0xff)];
  state[1] = invs_box[(state[1] >> 24)] << 24 | invs_box[(state[1] >> 16)&0xff] << 16 | invs_box[(state[1] >> 8)&0xff] << 8 | invs_box[(state[1] & 0xff)];
  state[2] = invs_box[(state[2] >> 24)] << 24 | invs_box[(state[2] >> 16)&0xff] << 16 | invs_box[(state[2] >> 8)&0xff] << 8 | invs_box[(state[2] & 0xff)];
  state[3] = invs_box[(state[3] >> 24)] << 24 | invs_box[(state[3] >> 16)&0xff] << 16 | invs_box[(state[3] >> 8)&0xff] << 8 | invs_box[(state[3] & 0xff)];
}


void ShiftRows(uint64_t state[]){
  uint64_t t00 = (state[0] >> 24); uint64_t  t10 = (state[0] >> 16) & 0xff; uint64_t  t20 = (state[0] >> 8) & 0xff; uint64_t  t30 = state[0] & 0xff;
  uint64_t t01 = (state[1] >> 24); uint64_t  t11 = (state[1] >> 16) & 0xff; uint64_t  t21 = (state[1] >> 8) & 0xff; uint64_t  t31 = state[1] & 0xff;
  uint64_t t02 = (state[2] >> 24); uint64_t  t12 = (state[2] >> 16) & 0xff; uint64_t  t22 = (state[2] >> 8) & 0xff; uint64_t  t32 = state[2] & 0xff;
  uint64_t t03 = (state[3] >> 24); uint64_t  t13 = (state[3] >> 16) & 0xff; uint64_t  t23 = (state[3] >> 8) & 0xff; uint64_t  t33 = state[3] & 0xff;
  state[0] = t00 << 24 | t11 << 16 | t22 << 8 | t33;
  state[1] = t01 << 24 | t12 << 16 | t23 << 8 | t30;
  state[2] = t02 << 24 | t13 << 16 | t20 << 8 | t31;
  state[3] = t03 << 24 | t10 << 16 | t21 << 8 | t32;
}

void invShiftRows(uint64_t state[]){
  uint64_t t00 = (state[0] >> 24); uint64_t  t10 = (state[0] >> 16) & 0xff; uint64_t  t20 = (state[0] >> 8) & 0xff; uint64_t  t30 = state[0] & 0xff;
  uint64_t t01 = (state[1] >> 24); uint64_t  t11 = (state[1] >> 16) & 0xff; uint64_t  t21 = (state[1] >> 8) & 0xff; uint64_t  t31 = state[1] & 0xff;
  uint64_t t02 = (state[2] >> 24); uint64_t  t12 = (state[2] >> 16) & 0xff; uint64_t  t22 = (state[2] >> 8) & 0xff; uint64_t  t32 = state[2] & 0xff;
  uint64_t t03 = (state[3] >> 24); uint64_t  t13 = (state[3] >> 16) & 0xff; uint64_t  t23 = (state[3] >> 8) & 0xff; uint64_t  t33 = state[3] & 0xff;
  state[0] = t00 << 24 | t13 << 16 | t22 << 8 | t31;
  state[1] = t01 << 24 | t10 << 16 | t23 << 8 | t32;
  state[2] = t02 << 24 | t11 << 16 | t20 << 8 | t33;
  state[3] = t03 << 24 | t12 << 16 | t21 << 8 | t30;
}

void MixColumns(uint64_t state[]){
  for(int i=0; i<4; i++){
    uint64_t d0 = state[i] >> 24;
    uint64_t d1 = (state[i] >> 16) & 0xff;
    uint64_t d2 = (state[i] >> 8) & 0xff;
    uint64_t d3 = (state[i] >> 0) & 0xff;
    uint64_t t0 = GFM(2,d0) ^ GFM(3,d1) ^ d2 ^ d3;
    uint64_t t1 = d0 ^ GFM(2,d1) ^ GFM(3,d2) ^ d3;
    uint64_t t2 = d0 ^ d1 ^ GFM(2,d2) ^ GFM(3,d3);
    uint64_t t3 = GFM(3,d0) ^ d1 ^ d2 ^ GFM(2,d3);
    state [i] = t0 << 24 | t1 << 16 | t2 << 8 | t3;
  }
}

void invMixColumns(uint64_t state[]){
  for(int i=0; i<4; i++){
    uint64_t d0 = state[i] >> 24;
    uint64_t d1 = (state[i] >> 16) & 0xff;
    uint64_t d2 = (state[i] >> 8) & 0xff;
    uint64_t d3 = (state[i] >> 0) & 0xff;
    uint64_t t0 = GFM(0x0e, d0) ^ GFM(0x0b, d1) ^ GFM(0x0d, d2) ^ GFM(0x09, d3);
    uint64_t t1 = GFM(0x09, d0) ^ GFM(0x0e, d1) ^ GFM(0x0b, d2) ^ GFM(0x0d, d3);
    uint64_t t2 = GFM(0x0d, d0) ^ GFM(0x09, d1) ^ GFM(0x0e, d2) ^ GFM(0x0b, d3);
    uint64_t t3 = GFM(0x0b, d0) ^ GFM(0x0d, d1) ^ GFM(0x09, d2) ^ GFM(0x0e, d3);
    state [i] = t0 << 24 | t1 << 16 | t2 << 8 | t3;
  }
}

void GFH4(uint64_t d[],uint64_t a0, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t b0, uint64_t b1, uint64_t b2, uint64_t b3){
  uint64_t t0=b0^b2; uint64_t t1=b1^b3; uint64_t t2=a0^a1; uint64_t t3=GFM(a0,t0^t1);
  uint64_t s0=a0^a2; uint64_t s1=a1^a3; uint64_t s2=GFM(s0,b2^b3); uint64_t s3=s0^s1;
  uint64_t r0=GFM(s0,b0^b1); uint64_t r1=s0^s1;   
  uint64_t f0=t3^GFM(t2,t1); uint64_t f1=t3^GFM(t2,t0);
  uint64_t g0=s2^GFM(s3,b3); uint64_t g1=s2^GFM(s3,b2);
  uint64_t h0=r0^GFM(r1,b1); uint64_t h1=r0^GFM(r1,b0);   
  d[0]=f0^g0;
  d[1]=f1^g1;
  d[2]=f0^h0;
  d[3]=f1^h1;
}

void GFH8(uint64_t d[],uint64_t a0, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4, uint64_t a5, uint64_t a6, uint64_t a7, uint64_t b0, uint64_t b1, uint64_t b2, uint64_t b3, uint64_t b4, uint64_t b5, uint64_t b6, uint64_t b7){
  uint64_t F[4];
	uint64_t G[4];
  uint64_t H[4];
  uint64_t t0=a0^a4; uint64_t t1=a1^a5; uint64_t t2=a2^a6; uint64_t t3=a3^a7;
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

void GFH16(uint64_t d[],uint64_t a0,uint64_t a1,uint64_t a2,uint64_t a3,uint64_t a4,uint64_t a5,uint64_t a6,uint64_t a7,uint64_t a8,uint64_t a9,uint64_t a10,uint64_t a11,uint64_t a12,uint64_t a13,uint64_t a14,uint64_t a15,uint64_t b0,uint64_t b1,uint64_t b2,uint64_t b3,uint64_t b4,uint64_t b5,uint64_t b6,uint64_t b7,uint64_t b8,uint64_t b9,uint64_t b10,uint64_t b11,uint64_t b12,uint64_t b13,uint64_t b14,uint64_t b15){
	uint64_t F[8];
	uint64_t G[8];
  uint64_t H[8];
  uint64_t t0=a0^a8; uint64_t t1=a1^a9; uint64_t t2=a2^a10; uint64_t t3=a3^a11; uint64_t t4=a4^a12; uint64_t t5=a5^a13; uint64_t t6=a6^a14; uint64_t t7=a7^a15;
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

void MixColumns_InvMix(uint64_t state[]){
	uint64_t dt[16];
  for(int i=0; i<1; i++){
    uint64_t d0 = state[i] >> 24;
    uint64_t d1 = (state[i] >> 16) & 0xff;
    uint64_t d2 = (state[i] >> 8) & 0xff;
    uint64_t d3 = (state[i] >> 0) & 0xff;
    uint64_t d4 = state[i+1] >> 24;
    uint64_t d5 = (state[i+1] >> 16) & 0xff;
    uint64_t d6 = (state[i+1] >> 8) & 0xff;
    uint64_t d7 = (state[i+1] >> 0) & 0xff;
    uint64_t d8 = state[i+2] >> 24;
    uint64_t d9 = (state[i+2] >> 16) & 0xff;
    uint64_t d10 = (state[i+2] >> 8) & 0xff;
    uint64_t d11 = (state[i+2] >> 0) & 0xff;
    uint64_t d12 = state[i+3] >> 24;
    uint64_t d13 = (state[i+3] >> 16) & 0xff;
    uint64_t d14 = (state[i+3] >> 8) & 0xff;
    uint64_t d15 = (state[i+3] >> 0) & 0xff;
    GFH16(dt,1,3,4,5,6,7,8,9,0XA,0XB,0XC,0XD,0XE,0x10,2,0x1E,d0,d1,d2,d3,d4,d5,d6,d7,d8,d9,d10,d11,d12,d13,d14,d15);
    state [i]   = dt[0] << 24 |dt[1] << 16 | dt[2] << 8 | dt[3];
    state [i+1] = dt[4]<< 24 |dt[5] << 16 | dt[6] << 8 | dt[7];
    state [i+2] = dt[8] << 24 | dt[9] << 16 |dt[10] << 8 | dt[11];
    state [i+3] = dt[12] << 24 | dt[13] << 16 | dt[14] << 8 | dt[15];
  }
}

void cipher(uint64_t state[],uint64_t Nr,uint64_t s_box[256],uint64_t w[45]){
  AddRoundKey(state, 0,w);
  for (int i=1; i<Nr; i++){
    SubBytes(state,s_box);
    MixColumns_InvMix(state);
    AddRoundKey(state, i*4,w);
  } 
  SubBytes(state,s_box);
  AddRoundKey(state, Nr * 4,w);
}

void invcipher(uint64_t state[],uint64_t Nr,uint64_t invs_box[256],uint64_t w[]){
  AddRoundKey(state, Nr*4,w);
  for (int i=Nr-1; i>0; i--){
    invSubBytes(state,invs_box);
    AddRoundKey(state, i*4,w);
    MixColumns_InvMix(state);
	}
  invSubBytes(state,invs_box);
  AddRoundKey(state, 0,w);
}