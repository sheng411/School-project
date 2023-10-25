from tkinter import *

window=Tk() #start

window.title('AES')        # title
#window.iconbitmap('.ico')  # setting icon (restricted .ico files)


frame_1=Frame(window)
frame_1.pack()

'''     send GUI    '''
send_gui=LabelFrame(frame_1,text="Send")
send_gui.grid(row=0,column=2,padx=50,pady=20)

send_gui_prompts=Label(send_gui,text="Enter text")    #input prompts
send_gui_prompts.grid(row=0,column=0)

send_gui_input=Entry(send_gui)
send_gui_input.grid(row=1,column=0)
send_gui_in_1=Label(send_gui,text=" ")
send_gui_in_1.grid(row=1,column=1)



'''     reception GUI   '''



window.mainloop()   #end