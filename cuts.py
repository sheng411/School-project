def cuts_message(msg):
    m=[]
    ms_str=""
    count=0

    for i in msg:
        ms_str+=i
        count+=1
        if count == 16:
            m.append(ms_str)
            count=0
            ms_str=""  # reset string

    if count > 0:
        m.append(ms_str)

    print(m)

'''
# test
inp = input("->")
cuts_message(inp)
'''
