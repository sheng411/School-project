'''
input type : string / int / float
output type : list

'''

lenght=100

def cuts_message(msg):
    m=[]
    ms_str=""
    count=0

    for i in msg:
        ms_str+=i
        count+=1
        if count == lenght:
            m.append(ms_str)
            count=0
            ms_str=""  # reset string

    if count > 0:
        m.append(ms_str)

    return m

'''
# test
inp = input("->")
print(cuts_message(inp))
'''
