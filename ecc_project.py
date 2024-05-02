import math
import random

m=163; p1=0xc9; mask=pow(2,m)-1; a=1

def GFM(a, b):
    c = 0;  
    for i in range(m-1, 0, -1):
        c = c ^ ((a>>i)&0x01)*b
        c = ((c << 1) & mask) ^ ((c>>(m-1))&0x01)*p1
    c = c ^ (a&0x01)*b
    return c

def inv(a):
    sum=1
    a2=GFM(a,a)
    for i in range(0, m-1):
        sum=GFM(sum,a2)
        a2=GFM(a2,a2)
    return sum

def point_add(P,Q):
    g1=[0,0]
    if P[0]==Q[0] and (P[0]^P[1]==Q[1] or Q[0]^Q[1]==P[1]): # (x,y)+(x,x+y)=O
         g1 = [0, 0];
    else:
        if P[0]==Q[0] and P[1]==Q[1]:  # Q=P call point double function
           g1=point_double(P)
        else:
            if P[0]==0 and P[1]==0: # O+Q = Q
                g1=Q
            else:
                if Q[0] == 0 and Q[1] == 0: # P+O = P
                   g1=P
                else:
                    s=GFM(Q[1]^P[1],inv(Q[0]^P[0]))
                    x3=GFM(s,s)^s^Q[0]^P[0]^a
                    y3=GFM(s,P[0]^x3)^x3^P[1]
                    g1[0]=x3; g1[1]=y3
    return g1

def point_double(G):
    G1=[0,0]
    if G[0]!=0:
       s=G[0]^(GFM(G[1],inv(G[0])))
       x3=GFM(s,s)^s^a
       y3=GFM(G[0],G[0])^GFM((s^1),x3)
       G1[0] = x3; G1[1] = y3
    return G1

def scalarm(K, G):
    LE = [[0,0],G]
    Q=[0,0]
    for i in range(m-1,0,-1):
        Q = point_double(Q)
        Q = point_add(Q, LE[(K >> i) & 0X01])
    Q = point_add(Q,LE[(K >> i) & 0X01])
    return Q

def random1():
    d=0
    for i in range(0,163,1):    
            a=random.randint(0,1)
            d=(d<<1)|a 
    return d



'''
def scalarm(K, G):
    q=m%wd; qbit=pow(2,q)-1; maskw=pow(2,wd)-1;
    PT=[]; PT.append([0,0]); PT.append(G); 
    Qt=PT[1]
    for i in range(2, maskw+1):
        Qt = point_add(Qt, G)
        PT.append(Qt)
    Q = [0, 0];    
    for i in range(m-wd, q-1, -wd):
        for j in range(0, wd):  Q = point_double(Q)
        Q = point_add(Q, PT[(K >> i) & maskw])
    for i in range(0, q):
        Q = point_double(Q)
    Q = point_add(Q, PT[K&qbit])
    return Q

'''
G=[0X02FE13C0537BBC11ACAA07D793DE4E6D5E5C94EEE8,0X0289070FB05D38FF58321F2E800536D538CCDAA3D9]
'''
for K in range(1, mask+1):
    Q=scalarm(K,G)
    print(" %2dP->" %K, Q)
'''



d=random1()  # set user A private key d
Qa=scalarm(d,G)
#print("User A %2d*G->" %d)
print("(",hex(Qa[0]),",",hex(Qa[1]),")")

k=random1()# set user A private key d
Qb=scalarm(k,G)
#print("User B %2d*G->" %k)
print("(",hex(Qb[0]),",",hex(Qb[1]),")")

#User A receiving User B, Qb
Sa=scalarm(d, Qb)
#print("User A receiving User B %2d*Qb->" %d)
print("(",hex(Sa[0]),",",hex(Sa[1]),")")

#User B receiving User A, Qa
Sb=scalarm(k, Qa)
#print("User B receiving User A %2d*Qa->" %k)
print("(",hex(Sb[0]),",",hex(Sb[1]),")")