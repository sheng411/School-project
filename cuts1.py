'''
input type : string / int / float
output type : list

'''


def cuts_message(msg):
    m=[]
    ms_str=""
    ms_str1=""
    count=0
    count1=0


    for i in msg:
        ms_str+=i
        count+=1
    if (count%16)==0:
        ms_str=ms_str

    if count > 0:
        for i in range(16-(count%16)):
            ms_str=ms_str+"0"
            count+=1
            
    for i in ms_str:
        ms_str1+=i
        count1+=1
        if(count1%(count//16))==0:
            m.append(ms_str1)
            ms_str1=""
    count=0
    ms_str=""  # reset string
    return m


G="1234567890123456789"
print(cuts_message(G))



'''
# test
inp = input("->")
print(cuts_message(inp))
'''
