import tkinter as tk
from PIL import Image, ImageTk

imglink=''

def open_photo(link):
    root = tk.Toplevel()
    root.title('Picture show')


    img = Image.open(link)
    tk_img = ImageTk.PhotoImage(img)

    show=tk.Label(root, image=tk_img)
    show.pack()

    img_w, img_h=img.size   #get photo size

    root.geometry(f'{img_w}x{img_h}')
    root.mainloop()
