from tkinter import *
from tkinter import ttk,filedialog,messagebox,font
from connect import *
import os

window=Tk() #start

window.title('AES_wifi_client')        # title
#window.iconbitmap('.ico')  # setting icon (restricted .ico files)


#window status
win_status=False

#control font
text_font=font.Font(family="Arial",size=14)
label_font=font.Font(family="Arial",size=12)


#control contexts
window.configure(bg="#8a2be2")      #blueviolet


#NOTE:
'''
update  line 42 and 47 data source
'''

frame_1=Frame(window)
frame_1.pack(expand=True, fill="both",padx=20,pady=20)      #center
#frame_1.configure(bg="#7fffd4")        #light green


'''     send GUI    '''

def send_submit():
    data_text=send_gui_input.get("1.0", END)                #get input content
    print("Send input-->",data_text)
    wifi_return=wifi_connect(data_text)
    print("wifi return->",wifi_return)
    show_text(wifi_return,data_text)

def clear():
    reception_gui_ciphertext.configure(state="normal")      #Editable mode
    reception_gui_ciphertext.delete("1.0", END)             #Delete content
    reception_gui_ciphertext.configure(state="disabled")    #Disable Edit Mode

    reception_gui_plaintext.configure(state="normal")       #Editable mode
    reception_gui_plaintext.delete("1.0", END)              #Delete content
    reception_gui_plaintext.configure(state="disabled")     #Disable Edit Mode


send_gui=LabelFrame(frame_1,text="Send",font=label_font)
send_gui.grid(row=1,column=0)


send_gui_prompts=Label(send_gui,text="Enter text",font=label_font)    #input prompts
send_gui_prompts.grid(row=0,column=0)

#text area
send_gui_input=Text(send_gui,height=8,width=30,font=text_font)      #input area
send_gui_input.grid(row=1,column=0)


#submit area
send_gui_button=Button(send_gui,text="Submit",command=send_submit,height=3,width=16,font=label_font)
send_gui_button.grid(row=1,column=4)



for widget in send_gui.winfo_children():        #spacing
    widget.grid_configure(padx=10,pady=5)


'''     reception GUI   '''


def show_text(ciphert,plaint):
    reception_gui_ciphertext.configure(state="normal")      #Editable mode
    reception_gui_ciphertext.delete("1.0", END)             #Delete content
    reception_gui_ciphertext.insert("1.0", ciphert)       #Infromation to be put into
    reception_gui_ciphertext.configure(state="disabled")    #Disable Edit Mode

    reception_gui_plaintext.configure(state="normal")       #Editable mode
    reception_gui_plaintext.delete("1.0", END)              #Delete content
    reception_gui_plaintext.insert("1.0", plaint)        #Infromation to be put into
    reception_gui_plaintext.configure(state="disabled")     #Disable Edit Mode


reception_gui=LabelFrame(frame_1,text="Reception",font=label_font)
reception_gui.grid(row=2,column=0,padx=20)


reception_gui_prompts=Label(reception_gui,text="ciphertext",font=label_font)    #ciphertext prompts
reception_gui_prompts.grid(row=0,column=0)

reception_gui_prompts=Label(reception_gui,text="plaintext",font=label_font)    #printtext prompts
reception_gui_prompts.grid(row=0,column=2)


#text out area
reception_gui_ciphertext=Text(reception_gui,height=8,width=30,font=text_font)      #output area
reception_gui_ciphertext.grid(row=1,column=0)


reception_gui_text_out=Button(reception_gui,text="Clear",command=clear,height=1,width=10,font=label_font,bg="#ff00ff")
reception_gui_text_out.grid(row=1,column=1)
#reception_gui_text_out=Label(reception_gui,text="    ")
#reception_gui_text_out.grid(row=1,column=1)


reception_gui_plaintext=Text(reception_gui,height=8,width=30,font=text_font)      #output area
reception_gui_plaintext.grid(row=1,column=2)


for widget in reception_gui.winfo_children():        #spacing
    widget.grid_configure(padx=8,pady=5)


'''     other   '''
window.geometry("900x550")  #window size
window.resizable(win_status,win_status)   # disable page size cheng
window.mainloop()   #end