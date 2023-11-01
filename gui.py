from tkinter import *
from tkinter import ttk,filedialog,messagebox
from photo import *
import os

window=Tk() #start

window.title('AES')        # title
#window.iconbitmap('.ico')  # setting icon (restricted .ico files)


frame_1=Frame(window)
frame_1.pack()


kb_limit=100
total_kb=130
file_path=''


'''     send GUI    '''

def send_submit():
    data_text=send_gui_input.get()
    print("Send input-->",data_text)

def file_show():
    global file_path
    file_path=filedialog.askopenfilename()
    print(file_path)
    file_size()

def file_size():        #get sile size
    global fs_kb
    fs_kb=0
    file_size=os.path.getsize(file_path) #get file path
    fs_kb=file_size/1024    #conversion kb
    print(f"File size: {file_size} bytes ({fs_kb:2f} KB)")
    if fs_kb>kb_limit:
        send_gui_file_1=Label(send_gui,text="*File Size Out of Range*",fg="red",font=('Arial'))
        send_gui_file_1.grid(row=2,column=2)
        send_gui_button=Button(send_gui,text="Submit",command=send_submit,height=3,width=16,state=DISABLED)
        send_gui_button.grid(row=1,column=4)
        send_gui_file_2=Label(send_gui,text=f"{fs_kb:.2f} KB used({fs_kb/total_kb*100:.2f}%)",fg="red")
        send_gui_file_2.grid(row=2,column=4)
    else:
        send_gui_file_1=Label(send_gui,text="                                   ",fg="red",font=('Arial'))
        send_gui_file_1.grid(row=2,column=2)
        send_gui_button=Button(send_gui,text="Submit",command=send_submit,height=3,width=16,state=NORMAL)
        send_gui_button.grid(row=1,column=4)
        send_gui_file_2=Label(send_gui,text=f"{fs_kb:.2f} KB used({fs_kb/total_kb*100:.2f}%)")
        send_gui_file_2.grid(row=2,column=4)



send_gui=LabelFrame(frame_1,text="Send")
send_gui.grid(row=0,column=0)

send_gui_prompts=Label(send_gui,text="Enter text")    #input prompts
send_gui_prompts.grid(row=0,column=0)

#text area
send_gui_input=Text(send_gui,height=8,width=30)      #input area
send_gui_input.grid(row=1,column=0)


#file area
send_gui_file_1=Label(send_gui,text=f"The file mush not exceed {total_kb}KB")
send_gui_file_1.grid(row=0,column=2)

send_gui_button=Button(send_gui,text="File",command=file_show,height=2,width=16)
send_gui_button.grid(row=1,column=2)

send_gui_file_1=Label(send_gui,text="")
send_gui_file_1.grid(row=2,column=2)

send_gui_txt_2=Label(send_gui,text="    ")
send_gui_txt_2.grid(row=1,column=3)

#submit area
send_gui_button=Button(send_gui,text="Submit",command=send_submit,height=3,width=16)
send_gui_button.grid(row=1,column=4)



for widget in send_gui.winfo_children():        #spacing
    widget.grid_configure(padx=5,pady=5)


'''     reception GUI   '''

def photo_show():
    if file_path!='':
        open_photo(f'{file_path}')
    else:
        print("Oh NO")
        messagebox.showwarning('Warning','No path!!!')

reception_gui=LabelFrame(frame_1,text="Reception")
reception_gui.grid(row=1,column=0)

reception_gui_prompts=Label(reception_gui,text="Private text")    #input prompts
reception_gui_prompts.grid(row=0,column=0)

#text out area
reception_gui_input=Text(reception_gui,height=8,width=30)      #output area
reception_gui_input.grid(row=1,column=0)

reception_gui_text_out=Label(reception_gui,text="    ")
reception_gui_text_out.grid(row=1,column=1)

#photo and video area
reception_gui_button=Button(reception_gui,text="Show",command=photo_show,height=2,width=10)
reception_gui_button.grid(row=1,column=2)


for widget in reception_gui.winfo_children():        #spacing
    widget.grid_configure(padx=5,pady=5)


'''     other   '''

window.geometry("650x450")  #window size
window.resizable(True,True)   # disable page size cheng
window.mainloop()   #end