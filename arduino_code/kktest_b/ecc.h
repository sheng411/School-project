#include <iostream>
/* run this program using the console pauser or add your own getch, system("pause") or input loop */
#include <string>
#include <cstdio>
#include <Arduino.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#define   wsize 3
using namespace std;
  
int       m=163;
uint64_t  mask=0x7FFFFFFFF;
int       maskbits=35;

struct mbits{
   uint64_t a[wsize];
};

struct points{
 mbits x;
 mbits y; 
};
points anT1[163];
points anT2[163];
points anT5[163];
points anT10[163];
points anT20[163];
points anT40[163];
int GFMT(int a, int b)
{
int m=8;  
int p=27;
int mask=pow(2,m)-1; 
int c=0;
for (int i=m-1;i>=1;i--)
    {
    c=c^(((a>>i)&0x01)*b);
    c=((c<<1)&mask)^(((c>>(m-1))&0x01)*p);  
    }
c=c^(a&0x01)*b;    
return c;
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

void showword(mbits d){
    int h;
    String str1[]={"0","1","2","3","4","5","6","7","8","9","A","B","C","D","E","F"};                   
    String str="";
    for(int i=wsize-1;i>=0;i--){
        for(int j=(63-3);j>=0;j=j-4)
      str=str+str1[(d.a[i]>>j)&0x0f];  
   }
   printf("%s\n",str.c_str());  
    //Serial.println(str.c_str());
}

mbits add(mbits a, mbits b)
{
 mbits c;
 for(int i=0;i<wsize;i++)
    c.a[i]=a.a[i]^b.a[i];
 return c;   
}

mbits f = {201,0,0};
mbits GFM(mbits A, mbits B){
   mbits C = {0,0,0};
    for(int i=m-1; i>=1; i--){
       if (getvmbits_shift(&A,1) == 1)
          C=add(C,B);
       if (getvmbits(C,1)==1)   
          C=add(shift(C,1),f); 
       else
          C=shift(C,1);    
    }
    if (getvmbits_shift(&A,1) == 1)
       C=add(C,B);
    return C;
}
void cls(mbits *d){
    for(int i=0;i<wsize;i++)
     	d->a[i]=0;
}

mbits powA_n(mbits k,points *a2T){
	mbits sum;
	uint64_t flag=0;
	cls(&sum);
	for(int i=0;i<3;i++){
		for(int j=0;j<64;j++){
			if((k.a[i]>>j)&0x01 ==1){
				
				sum=add(sum,a2T[flag].x);
			}
			flag=flag+1;		
		}	
	}
	
	return sum	;
}
void makepowt(int p, points *a){
	uint64_t  vvvv=1;
	for(int i=0;i<163;i++){
		//cls(&a[i].x);	
    a[i].x={0,0,0};
	}
	for(int i=0;i<163;i++){
		//cls(&a[i].y);	
	}
	for(int i=0;i<163;i++){
		if(i<64){
			a[i].x.a[0]=a[i].x.a[0]^(vvvv<<i);
		}
		if(i>=64 && i<128){
      if(i==64){
        vvvv=1;
      }
			a[i].x.a[1]=a[i].x.a[1]^(vvvv<<(i-64));
		}
		if(i>=128){
      if(i==128){
        vvvv=1;
      }
			a[i].x.a[2]=a[i].x.a[2]^(vvvv<<(i-128));
		}
  }
	
	for(int i=0;i<163;i++){
		for(int j=0;j<p;j++){
			a[i].x=GFM(a[i].x,a[i].x);
		}
	}
}
mbits sum, a2;
mbits Inverse(mbits A)
{ 
    sum = {0x1,0,0};
    a2 = GFM(A,A);
    for(int i=0;i<m-1;i++){
      sum=GFM(sum,a2);
      a2=GFM(a2,a2);// X IS POWER 2
    }
    return sum;
}



mbits Inv(mbits c){   //改良 inverse
	
	mbits M3;   //cls(&M3);
	mbits mt1;  //cls(&mt1);
	mbits mt;   //cls(&mt);
	mbits sum;	//cls(&sum);
	mbits ff;	  //cls(&ff);

	M3=GFM(powA_n(c,anT2),powA_n(c,anT1)); mt=M3; 
	M3=GFM(powA_n(M3,anT2),M3) ; 	                     
	M3=GFM(powA_n(c,anT5),M3) ;                       
	M3=GFM(powA_n(M3,anT5),M3) ;                       
	M3=GFM(powA_n(M3,anT10),M3);
	M3=GFM(powA_n(M3,anT20),M3);
	
	int wd=40;
	int q1=m%wd;
	
	sum=M3;
	for(int i=0;i<int(((m-1)-q1)/wd);i++) {
		sum=powA_n(sum, anT40);
    sum=GFM(sum, M3); 	
	}
	
	sum=GFM(sum,sum);
  sum=GFM(sum,sum); 
	sum=GFM(mt,sum);

 	return sum;
	
}

mbits one={1,0,0};
points point_double(points G){
    points G1 = {0,0}; mbits s; 
    if (G.x.a[0] != 0 && G.x.a[1] != 0 && G.x.a[2] != 0){
      s=add(G.x,(GFM(G.y,Inverse(G.x))));
      G1.x=add(add(GFM(s,s),s),one);
      G1.y=add(GFM(G.x,G.x),GFM(add(s,one),G1.x));
    }   
    return G1;
}


points point_double2(points G){
    points G1 = {0,0}; mbits s; 
    if (G.x.a[0] != 0 && G.x.a[1] != 0 && G.x.a[2] != 0){
      s=add(G.x,(GFM(G.y,Inv(G.x))));
      G1.x=add(add(GFM(s,s),s),one);
      G1.y=add(GFM(G.x,G.x),GFM(add(s,one),G1.x));
    }   
    return G1;
}





int equ(mbits x, mbits b){ 
    int flag=0;
    if ( x.a[0] == b.a[0] && x.a[1] == b.a[1] && x.a[2] == b.a[2])
      flag=1;
    return flag;
}

points point_add(points P, points Q){
    points g1={0,0};

    mbits s; mbits one={1,0,0}; mbits zero={0,0,0};

    if ( equ(P.x,Q.x) && equ(add(P.x,P.y),Q.y) || equ(add(Q.x,Q.y),P.y)) // (x,y)+(x,x+y)=O
      g1 = {0,0};
    else 
      if (equ(P.x,Q.x) && equ(P.y,Q.x))  // Q=P call point double function
        g1=point_double(P);
      else
        if (equ(P.x,zero) && equ(P.y,zero)) // O+Q = Q
          g1=Q;
          else
            if (equ(Q.x,zero) && equ(Q.y,zero)) //P+O = P
              g1=P;
            else{
              s=GFM(add(Q.y,P.y),Inverse(add(Q.x,P.x)));
              g1.x=add(add(add(add(GFM(s,s),s),Q.x),P.x),one);
              g1.y=add(add(GFM(s,add(P.x,g1.x)),g1.x),P.y);
            }           
    return g1;
}
points point_add2(points P, points Q){
    points g1={0,0};
    mbits s; mbits one={1,0,0}; mbits zero={0,0,0};
    if ( equ(P.x,Q.x) && equ(add(P.x,P.y),Q.y) || equ(add(Q.x,Q.y),P.y)) // (x,y)+(x,x+y)=O
         g1 = {0,0};
    else 
        if (equ(P.x,Q.x) && equ(P.y,Q.x))  // Q=P call point double function
           g1=point_double2(P);
        else
        if (equ(P.x,zero) && equ(P.y,zero)) // O+Q = Q
               g1=Q;
           else
            if (equ(Q.x,zero) && equ(Q.y,zero)) //P+O = P
                g1=P;
            else
            {
             s=GFM(add(Q.y,P.y),Inv(add(Q.x,P.x)));
             g1.x=add(add(add(add(GFM(s,s),s),Q.x),P.x),one);
             g1.y=add(add(GFM(s,add(P.x,g1.x)),g1.x),P.y);
            }           
    return g1;
}
points  PT[2];
points scalarmA(mbits K, points G1){
    PT[0] = {0,0};  PT[1] = G1; 

    points Q = {0,0};    // 162-128  127-64  63-0   a[2] a[1] a[0]

    for (int i=34; i>=0; i--){
      Q = point_add(Q, PT[(K.a[2] >> i) & 0x01]);
      Q = point_double(Q);
    }

    for (int i=63; i>=0; i--){
      Q = point_add(Q, PT[(K.a[1] >> i) & 0x01]);
      Q = point_double(Q);
    }

    for (int i=63; i>=1; i--){
        Q = point_add(Q, PT[(K.a[0] >> i) & 0x01]);
        Q = point_double(Q);
    }  
    Q = point_add(Q, PT[K.a[0]&0x01]);
    
    return Q;
}

points scalarmA2(mbits K, points G1){
   PT[0] = {0,0};  PT[1] = G1; 

    points Q = {0,0};    // 162-128  127-64  63-0   a[2] a[1] a[0]
   
    for (int i=34; i>=0; i--){
      Q = point_add2(Q, PT[(K.a[2] >> i) & 0x01]);
      Q = point_double2(Q);
    }
   
    for (int i=63; i>=0; i--){
      Q = point_add2(Q, PT[(K.a[1] >> i) & 0x01]);
      Q = point_double2(Q);
    }
   
    for (int i=63; i>=1; i--){
      Q = point_add2(Q, PT[(K.a[0] >> i) & 0x01]);
      Q = point_double(Q);
    }  
    Q = point_add2(Q, PT[K.a[0]&0x01]);
   
    return Q;
}