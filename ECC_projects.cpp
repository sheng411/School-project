/*meowmeow*/

#include <iostream>
#include <stdint.h>
#include <math.h>
#include <string>
#include <cstdio>
#include <cstdlib> //rand()
#include <ctime> //time()
using namespace std;
// m        163             283             571
//   wsize    3               5               9   represiting m bits
//   mask     0x7FFFFFFFF     0X7FFFFFF       0X7FFFFFFFFFFFFFF lead word need to mask;
//   maskbits  35bits         27bits          59bits 
//   f         201            4257            1061              irreducible polynomial 
//   
//   f  x163+x7+x6+x3+1    201
//   f  x283+x12+x7+x5+1   4257
//   f  x571+x10+x5+x2+1   1061     


int       m=163;
#define   wsize 3
uint64_t  mask=0x7FFFFFFFF;
int       maskbits=35;
uint64_t  f=201;
int       q;
int       ts;
int       tablelen;

//--------------------------

/*int       m=283;
#define   wsize 5
uint64_t  mask=0X7FFFFFF;
int       maskbits=27;
uint64_t  f=4257;
int       q;
int       ts;
int       tablelen;*/

/*int       m=571;
#define   wsize 9
uint64_t  mask=0x7FFFFFFFFFFFFFF;
int       maskbits=59;
uint64_t  f=1061;
int       q;
int       ts;
int       tablelen;*/

struct mbits
{
   uint64_t a[wsize];
};

struct points
{
 mbits x;
 mbits y; 
};
uint64_t *M;
mbits    *L;
mbits    *X1; // POWER 2
mbits    *X2; // POWER 4
mbits    *X3; // POWER 8
mbits    *X4; // POWER 16
mbits    *X5; // POWER 32
mbits    *X6; // POWER 64
mbits    *X7; // POWER 128
mbits    *X8; // POWER 256


mbits random(mbits w){
 
 int c;
 srand(time(NULL));
 for(int i=2;i<=0;i--){
  
  c=rand()%2;
  for(int j=0;j<=63;j++){
   w.a[i]=(w.a[i]<<1)|c;
  } 
 }
 
 
 return w;
}





mbits shift(mbits d, int s)
{   if (s>=1)
    {
 d.a[wsize-1]=((d.a[wsize-1]<<s)&mask)|(d.a[wsize-2]>>(64-s));
 for(int i=wsize-2;i>=1;i--)
     d.a[i]=(d.a[i]<<s)|(d.a[i-1]>>(64-s));
 d.a[0]=d.a[0]<<s;
    }
    return d;
}

int getvmbits_shift(mbits *A, int j)
{   uint64_t  temp=0;
    if (j>=1){
    temp=A->a[wsize-1] >> (maskbits-j);
    A->a[wsize-1]=((A->a[wsize-1]<<j)&mask)|(A->a[wsize-2]>>(64-j));
    for(int i=wsize-2;i>=1;i--)
        A->a[i]=(A->a[i]<<j)|(A->a[i-1]>>(64-j));
    A->a[0]=A->a[0]<<j;
    }
 return temp; 
}

int getvmbits(mbits A, int j)
{
 return A.a[wsize-1] >> (maskbits-j);
}

mbits zero(mbits k1){
 for(int i=0;i<3;i++){
     k1.a[i]=0;
     k1.a[i]=0;     
 }
 return k1;
}
points point_zero(points k1){
     k1.x=zero(k1.x);
     k1.y=zero(k1.y);     
 return k1;
}
mbits add(mbits a, mbits b)
{
 mbits c;
 for(int i=0;i<wsize;i++)
    c.a[i]=a.a[i]^b.a[i];
 return c;   
}

mbits addi(mbits a, uint64_t b)
{
 mbits c;
 for(int i=1;i<wsize;i++)  
    c.a[i]=a.a[i];
 c.a[0]=c.a[0]^b;   
 return c;   
}

showword(mbits d)
{
    int h;
    string str1[]={"0","1","2","3","4","5","6","7","8","9","A","B","C","D","E","F"};                   
    string str="";
    for(int i=wsize-1;i>=0;i--)
      {
      for(int j=(63-3);j>=0;j=j-4)
    str=str+str1[(d.a[i]>>j)&0x0f];  
   }
    printf("%s\n",str.c_str());
}

cls(mbits *d)
{
    for(int i=0;i<wsize;i++)
      d->a[i]=0;
}

setone(mbits *d)
{
    d->a[0]=1;    
    for(int i=1;i<wsize;i++)
      d->a[i]=0;  
}

makeMTABLE(uint64_t *M, int s)
{
 for(int i=0;i<pow(2,s);i++)
        M[i]=0;
 for(int i=0;i<pow(2,s);i++)
     for(int j=0;j<s;j++)
      M[i]=M[i]^((i>>j)&0x1)*(f<<j) ;
}

void makeLTABLE(mbits *L, mbits B)
{
    int one=1, p2, bit=1;       
    cls(&L[0]);
    L[1]=B;
    for(int i=2;i<tablelen;i++)
     if (i==(one << bit))
     {
         L[i]=shift(B,bit);
         L[i].a[0]=L[i].a[0]^M[B.a[wsize-1]>>(maskbits-bit)];
         bit++; 
  }
  else
      L[i]=add(L[one<<(bit-1)],L[(i^(one << (bit-1)))]);
}

mbits GFM(mbits A, mbits B)
{
 mbits C;
 cls(&C);
    makeLTABLE(L,B);
    for(int i=m-1;i>=(ts+q);i=i-ts)
      {
       C=add(C,L[getvmbits_shift(&A,ts)]);
     C=addi(shift(C,ts),M[getvmbits(C,ts)]); 
    }
    C=add(C,L[getvmbits_shift(&A,ts)]);
    C=addi(shift(C,q),M[getvmbits(C,q)]);
    for(int j=q-1;j>=1;j--)
     C=add(C,L[(getvmbits_shift(&A,1))<<j]);
    C=add(C,L[getvmbits_shift(&A,1)]);
    return C;
}

void makext(int s, mbits *X)  //餘數 
{
    mbits t1; 
    for(int i=0;i<m;i++)
      cls(&X[i]);
    cls(&t1);
    t1.a[0]=1;   
    X[0].a[0]=1;
    int count=2;
    int num=1;
    for(int i=0;i<m*s;i++)
    { 
     if (i==count)
      {
   count=count+s;
      X[num++]=t1;
      }   
     if (getvmbits_shift(&t1, 1))
      t1.a[0]=t1.a[0]^f;
    }
}

mbits power(mbits A, mbits *X)
{
    mbits temp;
    cls(&temp);
    for(int j=m-1;j>=0;j--)
       if (getvmbits_shift(&A, 1))
           temp=add(temp,X[j]);
    return temp;           
}

mbits Inverse(mbits A)
{   mbits temp2;
    mbits Iv;
    setone(&Iv); // set mbits vector into 1;
    A=power(A,X1);// X IS POWER 2 
    for(int i=0;i<m-1;i++){
      Iv=GFM(Iv,A);
   A=power(A,X1);// X IS POWER 2
    }
    return Iv;
}


bool compare(mbits a, mbits b)
{
 for(int i=0;i<=1; i++)
 {
  if((a.a[i]==b.a[i])&&(a.a[i+1]==b.a[i+1])&&(a.a[i+2]==b.a[i+2]))
   return true;
  else
   return false;
 }
}

points point_double(points G)
{
 mbits x3;
 mbits y3;
 mbits a={0x1,0,0};
    if (!((G.x.a[0]==0) and (G.x.a[1]==0) and (G.x.a[2]==0)))
    {
  mbits s=add(G.x,(GFM(G.y,Inverse(G.x))));
  x3=add(add(GFM(s,s),s),a);
        y3=add(GFM(G.x,G.x),GFM(add(s,a),x3));
        G.x = x3; G.y= y3;
 }
    return G;
}



points point_add(points P, points Q) 
{
 mbits x3;
 mbits y3;
 points g1; 
 mbits a={0x1,0,0};
    if (compare(P.x,Q.x) and (compare(add(P.x,P.y),Q.y) or compare(add(Q.x,Q.y),P.y))) // (x,y)+(x,x+y)=O
    {
     for(int i=0;i<3;i++){
      g1.x.a[i]=0;
      g1.y.a[i]=0;     
  } 
    
 }    
    else
    {
     if (compare(P.x,Q.x) and compare(P.y,Q.y))  //Q=P call point double function
        {
      g1=point_double(P);
  }    
        else
        {
         if (((P.x.a[0]==0) and (P.x.a[1]==0) and (P.x.a[2]==0)) and ((P.y.a[0]==0) and (P.y.a[1]==0) and (P.y.a[2]==0))) // O+Q = Q
            {
             g1=Q; 
   }
            else
            {
             if((((Q.x.a[0]==0) and (Q.x.a[1]==0) and (Q.x.a[2]==0)) and ((Q.y.a[0]==0) and (Q.y.a[1]==0) and (Q.y.a[2]==0)))) // P+O = P
                {
              g1=P; 
    }
                else
                {
                    mbits s=GFM(add(Q.y,P.y),Inverse(add(Q.x,P.x)));
                    x3=add(add(add(add(GFM(s,s),s),Q.x),P.x),a);
                    y3=add(add(GFM(s,add(P.x,x3)),x3),P.y);
                    g1.x=x3; g1.y=y3;
    }
   }
  }
 }
 return g1;
}

/*
points scalarm(unsigned int K, points G)
{   
 int wd=1;
    int q=m%wd;
    int qbit=pow(2,q)-1;
    int maskw=pow(2,wd)-1;
 points LE[maskw+1];
 LE[0]=point_zero(LE[0]);
 LE[1]=G;
 LE[2]=point_double(G);
 for(int i=3;i<maskw+1;i++)
 {
  LE[i]=point_add(G,LE[i-1]);
 }
 points Q;
 Q=point_zero(Q);
    for (int i=m-wd;i<q-1;i=i-wd)
    {
     for (int j=0;j<wd;j++)
     {
      Q = point_double(Q);
         Q = point_add(Q, LE[(K >> i) & 0X1]);
     }
 }
 for (int i=0;i<q;i++)
        Q = point_double(Q);
    Q = point_add(Q, LE[K&qbit]);
    return Q;
}
*/


points scalarm(mbits K, points G)
{   
 int m=64;
 points LE[2];
 LE[0]=point_zero(LE[0]);
 LE[1]=G;
 points Q;
 Q=point_zero(Q);
 for (int i=34;i>=0;i--)
    {
      //cout<<i<<endl;
      Q = point_double(Q);
      //cout<<"bits,"<<(K>>i)<<","<<i<<"-->";
      //showword(Q.x);
      //showword(Q.y);
         Q = point_add(Q, LE[(K.a[2]>>i & 0X1)]);
         /*cout<<"--->"<<(K>>i)<<endl;*/
         //showword(Q.x);
      //showword(Q.y);
 }
 
 for (int i=63;i>=0;i--)
    {
      //cout<<i<<endl;
      Q = point_double(Q);
      //cout<<"bits,"<<(K>>i)<<","<<i<<"-->";
      //showword(Q.x);
      //showword(Q.y);
         Q = point_add(Q, LE[(K.a[1] >> i) & 0X1]);
         /*cout<<"--->"<<(K>>i)<<endl;*/
         //showword(Q.x);
      //showword(Q.y);
 }
 
    for (int i=63;i>=1;i--)
    {
      //cout<<i<<endl;
      Q = point_double(Q);
      //cout<<"bits,"<<(K>>i)<<","<<i<<"-->";
      //showword(Q.x);
      //showword(Q.y);
         Q = point_add(Q, LE[(K.a[0]>>i) & 0X1]);
         /*cout<<"--->"<<(K>>i)<<endl;*/
         //showword(Q.x);
      //showword(Q.y);
 }

 Q = point_add(Q, LE[(K.a[0]& 0X1)]);
 /*cout<<"bits,"<<(K&0x01);
 showword(Q.x);
 showword(Q.y);*/

    return Q;
}


int main(int argc, char** argv) {
 clock_t start; double duration;
 mbits A, B, C, IC, O;
 points Qa,Qb,Sa,Sb; 
 X1=new mbits[m]; 
    makext(2,X1);
    ts=2;
 tablelen=pow(2,ts);
    q=m%ts;
 M=new uint64_t [tablelen];
    L=new mbits [tablelen];      
    makeMTABLE(M, ts);
    points  G;
    cout<<"------------"<<endl;
    G.x={0xDE4E6D5E5C94EEE8,0x7BBC11ACAA07D793,0x02FE13C053};
 G.y={0x0536D538CCDAA3D9,0x5D38FF58321F2E80,0X0289070FB0};
  cout<<"x-->"; showword(G.x);
  cout<<endl;
  cout<<"y-->"; showword(G.y);
 cout<<endl;
 
 mbits k_random;
 cout<<"------random(kG)------"<<endl;
 k_random=random(k_random);
 G=scalarm(k_random,G);
 
 cout<<"x-->"; showword(G.x);
  cout<<endl;
  cout<<"y-->"; showword(G.y);
 cout<<endl;
  /*
 cout<<"後來的G:"<<endl;
  cout<<"x-->";
  showword(G.x);
  cout<<""<<endl; 
  cout<<"y-->";
  showword(G.y);*/
  /*
  points G2;
  points G22;
  G2=point_double(G);
  G22=point_add(G,G2);
  cout<<""<<endl;
  cout<<"x-->";
  showword(G2.x);
  showword(G22.x);
  cout<<""<<endl; 
  cout<<"y-->";
  showword(G2.y);
  showword(G22.y);
  */
  
   return 0;
}














/*
 uint64_t d=0x03; // set user A private key d
 Qa=scalarm(d,G);
 //printf("User A", "%2d*G->\n" ,d);
 //printf("(",hex(Qa[0]),",",hex(Qa[1]),")");
 showword(Qa.x);
 showword(Qa.y);
 cout<<endl;
 
 uint64_t k=0x2; // set user A private key d
 Qb=scalarm(k,G);
 //printf("User B ","%2d","*G->" ,k);
 //printf("(",type(hex(Qb[0])),hex(Qb[0]),",",hex(Qb[1]),")")
 showword(Qb.x);
 showword(Qb.y);
 cout<<endl;
 
 //User A receiving User B, Qb
 Sa=scalarm(d, Qb);
 //print("User A receiving User B %2d*Qb->" %d);
 //print("(",hex(Sa[0]),",",hex(Sa[1]),")");
 showword(Sa.x);
 showword(Sa.y);
 cout<<endl;
 
 //User B receiving User A, Qa
 Sb=scalarm(*/ 
