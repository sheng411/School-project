import sys
import os
from PyQt5 import QtWidgets, QtCore, QtGui
from PyQt5.QtGui import QIcon
from PyQt5.QtCore import QThread, Qt, pyqtSignal
import serial
import serial.tools.list_ports
import json

# v 8.6.2

'''     環境設定     '''
title_name = "computer-A"   # 視窗標題
window_size = (850, 650)    # width, height
icon_path = os.path.join(os.path.dirname(__file__), "icon.png")     #先抓當前檔案的路徑,再加上icon
background_path=os.path.join(os.path.dirname(__file__), "background.jpg")  #背景圖片
background_path_fixed = background_path.replace("\\", "/")  #斜線翻轉
serial_baud=115200
index_name="呱呱呱呱呱" #首頁標題
img_type=[".png", ".jpg", ".jpeg"]  #圖片格式
txt_type=".txt"  #文字格式


class KeyPressFilter(QtCore.QObject):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.parent = parent

    def eventFilter(self, obj, event):
        if event.type() == QtCore.QEvent.KeyPress:
            if event.key() in (QtCore.Qt.Key_Return, QtCore.Qt.Key_Enter):
                if event.modifiers() == QtCore.Qt.ShiftModifier:
                    # Shift + Enter 換行
                    obj.textCursor().insertText('\n')
                else:
                    # Enter 送出
                    self.parent.send_message()
                return True
        return False

class SerialReaderThread(QThread):
    data_received = pyqtSignal(str)  # 發送訊息到主介面

    def __init__(self, serial_port):
        super().__init__()
        #self.port = port
        self.running = True
        self.serial_connection = serial_port

    def run(self):
        try:
            # 打開串列埠
            #self.serial_connection = serial.Serial(self.port, serial_baud)
            print(self.serial_connection)
            while self.running:
                if self.serial_connection.in_waiting > 0:  # 確認有資料可讀
                    try:
                        data = self.serial_connection.readline().decode('utf-8', errors='ignore').strip()
                        if data:  # 避免空行干擾
                            self.data_received.emit(data)
                    except Exception as e:
                        self.data_received.emit(f"Error reading data: {str(e)}")
        except Exception as e:
            self.data_received.emit(f"Error: {str(e)}")
        finally:
            if self.serial_connection:
                self.serial_connection.close()

    def stop(self):
        self.running = False
        if self.serial_connection and self.serial_connection.is_open:
            self.serial_connection.close()

class MainWindow(QtWidgets.QMainWindow):
    def __init__(self):
        super().__init__()

        self.connect_check=False
        self.username=""
        self.setObjectName("MainWindow")

        self.login_msg_ck=""
        self.login_msg_ck2=False

        # 初始化 chat_content_layout
        self.chat_content_layout = QtWidgets.QVBoxLayout()
        self.central_widget = QtWidgets.QWidget()
        self.central_widget.setLayout(self.chat_content_layout)
        self.setCentralWidget(self.central_widget)

        # 初始化標誌變數
        self.is_connected = False
        self.is_listening = False

        self.setWindowTitle(title_name)
        self.resize(window_size[0], window_size[1])
        self.clear_window()
        self.current_file_path = None  # 新增這行來儲存完整路徑

        # 設定背景圖片
        self.setStyleSheet(f"""
            QMainWindow {{
                background-image: url({background_path_fixed});
                background-position: center;
                background-repeat: none;
            }}
        """)

        # icon圖像設定
        if os.path.exists(icon_path):
            self.setWindowIcon(QIcon(icon_path))
        else:
            print("Icon file not found!")
        
        # 初始化序列埠
        self.serial_port = None

        # 建立中央 Widget
        central_widget = QtWidgets.QWidget()
        self.setCentralWidget(central_widget)
        
        # 建立主布局
        main_layout = QtWidgets.QVBoxLayout(central_widget)

        # 建立標題區域
        title_container = QtWidgets.QWidget()
        title_layout = QtWidgets.QVBoxLayout(title_container)

        # 首頁標題
        title_label = QtWidgets.QLabel(index_name)
        title_label.setAlignment(QtCore.Qt.AlignCenter)
        title_label.setStyleSheet("""
            QLabel {
                font-size: 36px;
                font-weight: bold;
                color: #333333;
                margin-bottom: 10px;
                font-family: "Microsoft YaHei", "微軟正黑體";
            }
        """)

        # 提示文字
        hint_label = QtWidgets.QLabel("請選擇模式")
        hint_label.setAlignment(QtCore.Qt.AlignCenter)
        hint_label.setStyleSheet("""
            QLabel {
                font-size: 24px;
                color: #666666;
                font-family: "Microsoft YaHei", "微軟正黑體";
            }
        """)

        # 建立序列埠控制區域
        port_control = QtWidgets.QHBoxLayout()
        
        # 建立序列埠選擇下拉選單
        self.port_combo = QtWidgets.QComboBox()
        self.update_ports()
        port_label = QtWidgets.QLabel("序列埠選擇:")
        port_label.setStyleSheet("""
            QLabel {
                font-size: 20px;  /* 放大字體 */
                font-weight: bold;  /* 粗體 */
                color: #333333;
                font-family: "Microsoft YaHei", "微軟正黑體";  /* 設置字體 */
            }
        """)
        port_control.addWidget(port_label)
        port_control.setAlignment(QtCore.Qt.AlignCenter)
        port_control.addWidget(self.port_combo)
        
        # 建立連接按鈕
        self.connect_button = QtWidgets.QPushButton("連接")
        self.connect_button.setFixedSize(100, 40)
        self.connect_button.setStyleSheet("""
            QPushButton {
                border-radius: 20px;
                background-color: #4CAF50;
                color: white;
                border: none;
                font-family: "Microsoft YaHei", "微軟正黑體";
                font-size: 16px
            }
            QPushButton:hover {
                background-color: #008000;
            }
        """)
        self.connect_button.clicked.connect(self.toggle_connection)
        port_control.addWidget(self.connect_button)

    #刷新序列埠
        self.refresh_button = QtWidgets.QPushButton("刷新")
        self.refresh_button.setFixedSize(100, 40)
        self.refresh_button.setStyleSheet("""
            QPushButton {
                border-radius: 20px;
                background-color: #c0c0c0;
                color: white;
                border: none;
                font-family: "Microsoft YaHei", "微軟正黑體";
                font-size: 16px
            }
            QPushButton:hover {
                background-color: #808080;
            }
        """)
        self.refresh_button.clicked.connect(self.refresh_ports)
        print("已刷新")
        port_control.addWidget(self.refresh_button)


    # 建立名字輸入區
        login_layout = QtWidgets.QHBoxLayout()

    # 提示字
        name_label = QtWidgets.QLabel("輸入名字：")
        name_label.setFixedSize(100,40)  # 設置固定高度
        name_label.setStyleSheet("""
            QLabel {
                font-size: 20px;  /* 放大字體 */
                font-weight: bold;  /* 粗體 */
                color: #333333;
                font-family: "Microsoft YaHei", "微軟正黑體";  /* 設置字體 */
            }
        """)

    #名字輸入框
        self.name_input = QtWidgets.QLineEdit()
        self.name_input.setFixedSize(195, 45)
        self.name_input.setStyleSheet("""
            QLineEdit {
                font-size: 16px;
                font-family: "Microsoft YaHei", "微軟正黑體";
                padding: 5px;
                border-radius: 10px;
                border: 2px solid #ccc;
                padding: 5px;
                font-size: 16px;
            }
        """)

    #登入按鈕
        self.login_button = QtWidgets.QPushButton("登入")
        self.login_button.setFixedSize(95, 40)
        self.login_button.setStyleSheet("""
            QPushButton {
                border-radius: 20px;
                background-color: #00bfff;
                color: white;
                border: none;
                font-family: "Microsoft YaHei", "微軟正黑體";
                font-size: 16px
            }
            QPushButton:hover {
                background-color: #1e90ff;
            }
        """)
        #self.login_button.setEnabled(False)  # 預設按鈕為禁用
        self.login_button.clicked.connect(self.login_check)

        login_layout.addWidget(name_label)
        login_layout.addWidget(self.name_input)
        login_layout.addWidget(self.login_button)
        login_layout.setAlignment(Qt.AlignCenter)

        # 將標題和提示文字加入標題布局
        title_layout.addWidget(title_label)
        title_layout.addWidget(hint_label)
        main_layout.addWidget(title_container)
        main_layout.addLayout(port_control)
        main_layout.addLayout(login_layout)
        main_layout.addStretch()  # 添加彈性空間，使標題置於上方

        self.serial_thread = None

# 首頁
    def home_index_selected(self):
        central_widget = QtWidgets.QWidget(self)
        self.setCentralWidget(central_widget)
        self.clear_window()

        main_layout = QtWidgets.QVBoxLayout(central_widget)

        # 設定背景圖片
        self.setStyleSheet(f"""
            QMainWindow {{
                background-image: url({background_path_fixed});
                background-position: center;
                background-repeat: no-repeat;
                background-attachment: fixed;
                background-size: cover;
            }}
        """)

        # 標題
        title_container = QtWidgets.QWidget()
        title_layout = QtWidgets.QVBoxLayout(title_container)
        
        title_label = QtWidgets.QLabel(index_name)
        title_label.setAlignment(QtCore.Qt.AlignCenter)
        title_label.setStyleSheet("""
            QLabel {
                font-size: 36px;
                font-weight: bold;
                color: #333333;
                margin-bottom: 10px;
                font-family: "Microsoft YaHei", "微軟正黑體";
            }
        """)
        
        hint_label = QtWidgets.QLabel("請選擇模式")
        hint_label.setAlignment(QtCore.Qt.AlignCenter)
        hint_label.setStyleSheet("""
            QLabel {
                font-size: 24px;
                color: #666666;
                font-family: "Microsoft YaHei", "微軟正黑體";
            }
        """)
        
        # 建立序列埠控制區域
        port_control = QtWidgets.QHBoxLayout()
        
        # 建立序列埠選擇下拉選單
        self.port_combo = QtWidgets.QComboBox()
        self.update_ports()
        port_label = QtWidgets.QLabel("序列埠選擇:")
        port_label.setStyleSheet("""
            QLabel {
                font-size: 20px;  /* 放大字體 */
                font-weight: bold;  /* 粗體 */
                color: #333333;
                font-family: "Microsoft YaHei", "微軟正黑體";  /* 設置字體 */
            }
        """)
        port_control.addWidget(port_label)
        port_control.setAlignment(QtCore.Qt.AlignCenter)
        port_control.addWidget(self.port_combo)
        
        # 建立連接按鈕
        if self.connect_check:
            self.connect_button = QtWidgets.QPushButton("斷開")
            self.connect_button.setFixedSize(100, 40)
            self.connect_button.setStyleSheet("""
                QPushButton {
                    border-radius: 20px;
                    background-color: #ff0000;
                    color: white;
                    border: none;
                    font-family: "Microsoft YaHei", "微軟正黑體";
                    font-size: 16px
                }
                QPushButton:hover {
                    background-color: #dc143c;
                }
            """)
        else:
            self.connect_button = QtWidgets.QPushButton("連接")
            self.connect_button.setFixedSize(100, 40)
            self.connect_button.setStyleSheet("""
                QPushButton {
                    border-radius: 20px;
                    background-color: #4CAF50;
                    color: white;
                    border: none;
                    font-family: "Microsoft YaHei", "微軟正黑體";
                    font-size: 16px
                }
                QPushButton:hover {
                    background-color: #008000;
                }
            """)
        self.connect_button.clicked.connect(self.toggle_connection)
        port_control.addWidget(self.connect_button)

    #刷新序列埠
        self.refresh_button = QtWidgets.QPushButton("刷新")
        self.refresh_button.setFixedSize(100, 40)
        self.refresh_button.setStyleSheet("""
            QPushButton {
                border-radius: 20px;
                background-color: #c0c0c0;
                color: white;
                border: none;
                font-family: "Microsoft YaHei", "微軟正黑體";
                font-size: 16px
            }
            QPushButton:hover {
                background-color: #808080;
            }
        """)
        self.refresh_button.clicked.connect(self.refresh_ports)
        print("已刷新")
        port_control.addWidget(self.refresh_button)

    # 建立名字輸入區
        login_layout = QtWidgets.QHBoxLayout()

    # 提示字
        name_label = QtWidgets.QLabel("輸入名字：")
        name_label.setFixedSize(100,40)  # 設置固定高度
        name_label.setStyleSheet("""
            QLabel {
                font-size: 20px;  /* 放大字體 */
                font-weight: bold;  /* 粗體 */
                color: #333333;
                font-family: "Microsoft YaHei", "微軟正黑體";  /* 設置字體 */
            }
        """)

    #名字輸入框
        self.name_input = QtWidgets.QLineEdit()
        self.name_input.setFixedSize(195, 45)
        self.name_input.setStyleSheet("""
            QLineEdit {
                font-size: 16px;
                font-family: "Microsoft YaHei", "微軟正黑體";
                padding: 5px;
                border-radius: 10px;
                border: 2px solid #ccc;
                padding: 5px;
                font-size: 16px;
            }
        """)

    #登入按鈕
        self.login_button = QtWidgets.QPushButton("登入")
        self.login_button.setFixedSize(95, 40)
        self.login_button.setStyleSheet("""
            QPushButton {
                border-radius: 20px;
                background-color: #00bfff;
                color: white;
                border: none;
                font-family: "Microsoft YaHei", "微軟正黑體";
                font-size: 16px
            }
            QPushButton:hover {
                background-color: #1e90ff;
            }
        """)
        #self.login_button.setEnabled(False)  # 預設按鈕為禁用
        self.login_button.clicked.connect(self.login_check)
        
        login_layout.addWidget(name_label)
        login_layout.addWidget(self.name_input)
        login_layout.addWidget(self.login_button)
        login_layout.setAlignment(Qt.AlignCenter)

        title_layout.addWidget(title_label)
        title_layout.addWidget(hint_label)
        main_layout.addWidget(title_container)
        main_layout.addLayout(port_control)
        main_layout.addLayout(login_layout)
        main_layout.addStretch()

# 文字區改版
    def text_input_selected(self):
        self.clear_window()
        self.username = self.name_input.text()
    # 主窗口的Widget
        central_widget = QtWidgets.QLabel()
        central_widget = QtWidgets.QWidget(self)
        self.setCentralWidget(central_widget)

        main_layout = QtWidgets.QVBoxLayout(central_widget)

    # 新增選單欄
        menu_bar = self.menuBar()
        view_menu = QtWidgets.QMenu("功能", self)
        menu_bar.addMenu(view_menu)

        # 新增「滑到底部」按鈕
        scroll_to_bottom_action = view_menu.addAction("滑到底部")
        scroll_to_bottom_action.triggered.connect(self.scroll_to_bottom)

    # 用戶名稱區域
        username_label = QtWidgets.QLabel(f"用戶名稱: {self.username}")
        username_label.setAlignment(Qt.AlignLeft)
        username_label.setStyleSheet("""
            QLabel {
                font-size: 20px;
                font-weight: bold;
                color: #333333;
                font-family: "Microsoft YaHei", "微軟正黑體";
            }
        """)
        main_layout.addWidget(username_label)

    # 聊天顯示區域
        self.chat_area = QtWidgets.QScrollArea(self)
        self.chat_area.setWidgetResizable(True)
        self.chat_area.setVerticalScrollBarPolicy(Qt.ScrollBarAlwaysOn)
        self.chat_area.setStyleSheet("""
            QScrollArea {
                border: 2px solid #ccc;
                border-radius: 10px;
                background: white;
            }
        """)

    # 聊天內容
        self.chat_content = QtWidgets.QWidget()
        self.chat_content_layout = QtWidgets.QVBoxLayout()
        self.chat_content.setLayout(self.chat_content_layout)
        self.chat_area.setWidget(self.chat_content)
        main_layout.addWidget(self.chat_area)

    # 底部輸入和按鈕區域
        input_layout = QtWidgets.QHBoxLayout()

    # 輸入框
        self.input_field = QtWidgets.QTextEdit()
        self.input_field.setFixedHeight(60)  # 設定固定高度
        self.input_field.setPlaceholderText("輸入訊息...")
        self.input_field.setStyleSheet("""
            QTextEdit {
                border: 2px solid #ccc;
                border-radius: 10px;
                padding: 5px;
                font-size: 16px;
            }
        """)
        self.key_filter = KeyPressFilter(self)
        self.input_field.installEventFilter(self.key_filter)

    # 文件欄位
        self.file_label = QtWidgets.QLabel("未選擇檔案")
        self.file_label.setStyleSheet("""
            QLabel {
                border: 2px solid #ccc;
                border-radius: 10px;
                padding: 5px;
                font-size: 16px;
                min-width: 150px;
                font-family: "Microsoft YaHei", "微軟正黑體";
            }
        """)
        self.file_label.setAlignment(Qt.AlignCenter)

    # 瀏覽按鈕
        browse_button = QtWidgets.QPushButton("瀏覽")
        browse_button.setFixedSize(100, 40)
        browse_button.clicked.connect(self.open_file_dialog)
        browse_button.setStyleSheet("""
            QPushButton {
                border-radius: 17px;
                background-color: #4CAF50;
                color: white;
                border: none;
                font-size: 16px;
                font-family: "Microsoft YaHei", "微軟正黑體";
            }
            QPushButton:hover {
                background-color: #008000;
            }
        """)

    # 傳送按鈕
        send_button = QtWidgets.QPushButton("傳送")
        send_button.setFixedSize(100, 40)
        send_button.clicked.connect(self.on_send_clicked)
        send_button.setStyleSheet("""
            QPushButton {
                border-radius: 17px;
                background-color: #4CAF50;
                color: white;
                border: none;
                font-size: 16px;
                font-family: "Microsoft YaHei", "微軟正黑體";
            }
            QPushButton:hover {
                background-color: #008000;
            }
        """)

        input_layout.addWidget(self.input_field)
        input_layout.addWidget(self.file_label)
        input_layout.addWidget(browse_button)
        input_layout.addWidget(send_button)
        main_layout.addLayout(input_layout)
        central_widget.setLayout(main_layout)

    # 設定布局的邊距和間距
        main_layout.setContentsMargins(10, 10, 10, 10)
        main_layout.setSpacing(10)

#選擇檔案
    def open_file_dialog(self):
        options = QtWidgets.QFileDialog.Options()
        file_path, _ = QtWidgets.QFileDialog.getOpenFileName(self, "選擇文件", "", "Text Files (*.txt)", options=options)
        if file_path:
            self.on_file_selected(file_path)

    def on_file_selected(self, file_path):
        self.current_file_path = file_path  # 儲存完整路徑
        file_name = os.path.basename(file_path)
        self.file_label.setText(file_name)

#傳送訊息
    '''
    def send_message(self):
        if not self.is_connected:
            print(f"未連接到序列埠，無法傳送訊息,{self.is_connected}")
            return
        try:
            # 準備 JSON 資料
            message_data = {
                "sender": self.username,
                "timestamp": str(QtCore.QDateTime.currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
            }

            # 處理文字訊息
            if self.input_field.text().strip():
                message_data.update({
                    "file_type": "text",
                    "file_data": self.input_field.text(),
                    "content_type": "message"
                })
                # 轉換成 JSON 並傳送
                json_data = json.dumps(message_data, ensure_ascii=False)
                self.serial_port.write(json_data.encode('UTF-8'))
                self.show_message(self.input_field.text(), is_self=True)
                print(f"送出訊息-> {json_data}")
                self.input_field.clear()

            # 處理檔案
            if hasattr(self, 'file_label') and self.file_label.text() != "未選擇檔案":
                file_path = os.path.abspath(self.current_file_path).replace("\\", "/")
                _, file_extension = os.path.splitext(file_path)
                file_extension = file_extension.lower()

                if file_extension == txt_type:
                    with open(file_path, 'r', encoding='utf-8') as file:
                        file_data = file.read()
                        message_data.update({
                            "file_type": "txt",
                            "file_name": os.path.basename(file_path),
                            "file_data": file_data,
                            "file_size": len(file_data.encode('utf-8')),
                            "content_type": "file"
                        })
                        # 轉換成 JSON 並傳送
                        json_data = json.dumps(message_data, ensure_ascii=False)
                        self.serial_port.write(json_data.encode('UTF-8'))
                        
                        print(f"檔案資訊-> {json_data}")
                        self.show_message(f"已傳送檔案: {os.path.basename(file_path)}", is_self=True)
                        self.file_label.setText("未選擇檔案")

                elif file_extension in img_type:
                    with open(file_path, 'rb') as file:
                        file_data = file.read()
                        #bytecode=list(file_data)
                        #print(bytecode)

                        # 將二進制數據轉換為 base64 字串
                        base64_data = base64.b64encode(file_data).decode('utf-8')
                        message_data.update({
                            "file_type": "img",
                            "file_name": os.path.basename(file_path),
                            "file_data": base64_data,
                            "file_size": len(file_data),
                            "content_type": "file"
                        })
                        # 轉換成 JSON 並傳送
                        json_data = json.dumps(message_data, ensure_ascii=False)
                        self.serial_port.write(json_data.encode('UTF-8'))
                        
                        print(f"檔案資訊-> {json_data}")
                        self.show_message(f"已傳送圖片: {os.path.basename(file_path)}", is_self=True)
                        self.file_label.setText("未選擇檔案")

        except Exception as e:
            print(f"傳送錯誤: {e}")
            QtWidgets.QMessageBox.warning(self, "錯誤", f"傳送失敗: {str(e)}")
    '''

    def select_file(self):
        file_path, _ = QtWidgets.QFileDialog.getOpenFileName(self, "選擇檔案", "", "Text Files (*.txt)")
        if file_path:
            self.selected_file_path = file_path
            self.message_input.setText(f"已選擇檔案: {os.path.basename(file_path)}")
            return True
        return False

    def on_send_clicked(self):
        if hasattr(self, 'current_file_path'):
            self.send_message_file()
        else:
            # 原有的發送訊息邏輯
            self.send_message() 

    def send_message(self):
        if not self.is_connected:
            print(f"未連接到序列埠，無法傳送訊息,{self.is_connected}")
            return
        # 送文字訊息
        message = self.input_field.toPlainText().strip()
        if message:
            try:
                #message+="\n"
                self.serial_port.write(message.encode('UTF-8'))
                self.show_message(message, is_self=True)
                print(f"送出訊息-> {message}")
                self.input_field.clear()
            except Exception as e:
                print(f"Error sending message: {e}")

    def send_message_file(self):
        try:
            if hasattr(self, 'current_file_path'):
                with open(self.current_file_path, 'r', encoding='utf-8') as file:
                    content = file.read()
                    data = {
                        'file_type': 'txt',
                        "file_name": os.path.basename(self.current_file_path),
                        'file_data': content,
                        'content_type': 'file'
                    }
                    json_data = json.dumps(data, ensure_ascii=False)
                    if self.serial_port and self.serial_port.is_open:
                        self.serial_port.write(json_data.encode())
                        print(f"已發送檔案: {self.current_file_path}")
                        delattr(self, 'current_file_path')
                        self.file_label.clear()
            else:
                print("未選擇檔案")
        except Exception as e:
            print(f"發送檔案時發生錯誤: {e}")

# 開始聆聽序列埠
    def start_listening(self):
        #self.serial_port = self.port_selector.currentText()
        if hasattr(self, 'serial_port') and self.serial_port.is_open:
            self.serial_thread = SerialReaderThread(self.serial_port)
            self.serial_thread.data_received.connect(self.check_and_receive_message)
            self.serial_thread.start()
        else:
            print("Serial port not connected")

    def start_listening_login(self):
        try:
            self.stop_listening()
            if hasattr(self, 'serial_port') and self.serial_port.is_open:
                print("開始建立新的監聽線程")
                self.is_listening = True
                self.serial_thread1 = SerialReaderThread(self.serial_port)
                self.serial_thread1.data_received.connect(self.remsg_login)
                self.serial_thread1.start()
            else:
                print("Serial port not connected")
                self.is_listening = False
        except Exception as e:
            print(f"啟動監聽時發生錯誤: {e}")
            self.is_listening = False

    def stop_listening(self):
        try:
            print("準備停止監聽")
            self.is_listening = False
            if hasattr(self, 'serial_thread1'):
                if self.serial_thread1.isRunning():
                    self.serial_thread1.data_received.disconnect()
                    self.serial_thread1.quit()
                    if not self.serial_thread1.wait(200):  # 等待200ms
                        self.serial_thread1.terminate()  # 強制終止
                    print("線程已停止")
                delattr(self, 'serial_thread1')
        except Exception as e:
            print(f"停止監聽時發生錯誤: {e}")

    def remsg_login(self, message):
        self.login_msg_ck=message
        print("self.login_msg_ck ",self.login_msg_ck)

# 登入確認
    def login_check(self):
        try:
            print("self.login_msg_ck-*- ",self.login_msg_ck,"--",self.login_msg_ck2)
            # 檢查是否選擇序列埠
            selected_port = self.port_combo.currentText()
            if not selected_port or not self.is_connected:
                QtWidgets.QMessageBox.critical(
                    self,
                    "Error",
                    "請連接序列埠，在重試一次",
                    QtWidgets.QMessageBox.Ok
                )
                return False
            
            if not self.name_input.text().strip():
                QtWidgets.QMessageBox.critical(
                    self,
                    "Error",
                    "請輸入姓名",
                    QtWidgets.QMessageBox.Ok
                )
                return False
            
            if self.login_msg_ck != "setup OK":
                QtWidgets.QMessageBox.warning(
                    self,
                    "Warning",
                    "開發板尚未設定完成",
                    QtWidgets.QMessageBox.Ok
                )
                return False

            # 如果條件都符合，連接按鈕事件
            self.login_button.clicked.disconnect()  # 先斷開現有連接
            self.text_input_selected()
            #self.login_msg_ck2=True
            self.stop_listening()
            self.start_listening()
            print("登入檢查成功，已啟用聊天功能")
            return True
                
        except Exception as e:
            QtWidgets.QMessageBox.critical(
                self,
                "錯誤",
                f"登入檢查時發生錯誤：{str(e)}",
                QtWidgets.QMessageBox.Ok
            )
            return False

# 接收訊息
    '''
    def receive_message(self, message):
        if not self.is_connected:
            print("未連接到序列埠，無法傳送訊息")
            return
        try:
            # 檢查 message 是否為空
            if not message.strip():
                raise ValueError("接收到空訊息")

            # 解析 JSON 資料
            data = json.loads(message)
            
            if data.get("content_type") == "message":
                self.show_message(data["file_data"], is_self=False)

            # 根據內容類型處理
            elif data.get("content_type") == "file" and data.get("file_type") == "img":
                # 將 base64 字串轉回二進位數據
                img_bytes = base64.b64decode(data["file_data"])
                
                # 建立暫存檔案儲存接收到的圖片
                temp_path = f"received_{data['file_name']}"
                with open(temp_path, 'wb') as f:
                    f.write(img_bytes)
                
                # 顯示圖片
                pixmap = QtGui.QPixmap(temp_path)
                if not pixmap.isNull():
                    img_label = QtWidgets.QLabel()
                    img_label.setPixmap(pixmap.scaled(300, 200, Qt.KeepAspectRatio, Qt.SmoothTransformation))
                    
                    message_layout = QtWidgets.QHBoxLayout()
                    message_layout.addWidget(img_label)
                    message_layout.addStretch()
                    
                    if self.chat_content_layout:
                        self.chat_content_layout.addLayout(message_layout)
                    self.show_message(f"收到圖片: {data['file_name']}", is_self=False)
            else:
                # 處理其他類型的訊息
                self.show_message(data.get("file_data", message), is_self=False)

        except json.JSONDecodeError as e:
            print(f"JSON 解析錯誤: {e}")
            print(f"收到的原始訊息: {message}")  # 偵錯用
            # 當作普通文字處理
            if self.chat_content_layout:
                self.show_message(message, is_self=False)
            
        except Exception as e:
            print(f"接收訊息處理錯誤: {e}")
            self.show_message(f"接收訊息錯誤: {str(e)}", is_self=False)
    '''

    def check_and_receive_message(self, message):
        try:
            # 嘗試解析JSON
            json_data = json.loads(message)
            
            # 如果成功解析且是檔案類型
            if isinstance(json_data, dict) and json_data.get('content_type') == 'file':
                self.receive_message_file(message)

        except json.JSONDecodeError:
            # 非JSON格式，使用一般訊息處理
            self.receive_message(message)
        except Exception as e:
            print(f"訊息處理錯誤: {e}")
            self.receive_message(message)

    def receive_message(self, message):
        if not self.is_connected:
            print("未連接到序列埠，無法傳送訊息")
            return
        # 接收到的訊息顯示在左側
        self.show_message(message, is_self=False)

    def receive_message_file(self, message):
        try:
            # 解析JSON資料
            data = json.loads(message)
            
            # 檢查是否為文件類型
            if data.get('file_type') == 'txt':
                content = data.get('content')
                filename = data.get('filename')
                
                # 儲存接收到的文件
                save_path = f"received_{filename}"
                #with open(save_path, 'w', encoding='utf-8') as f:
                #    f.write(content)
                
                # 顯示接收訊息
                self.show_message(f"[收到文件:] {filename} \n[內容:]\n{content}", is_self=False)
                print(f"文件已儲存至: {save_path}")
                
        except json.JSONDecodeError as e:
            print(f"JSON解析錯誤: {e}")
        except Exception as e:
            print(f"接收文件時發生錯誤: {e}")

    def create_message_label(self, text, is_self=True):
        # 建立訊息標籤
        message_label = QtWidgets.QLabel(text)
        message_label.setWordWrap(False)  # 啟用自動換行
        message_label.setMaximumWidth(700)  # 最大寬度設為700
        
        # 使用 sizeHint 取得文字實際所需寬度
        label_width = message_label.fontMetrics().boundingRect(text).width() + 40  # 加上padding
        
        # 如果文字寬度小於500，則使用實際寬度
        if label_width < 500:
            message_label.setMinimumWidth(min(label_width, 500))
        else:
            message_label.setMinimumWidth(100)  # 設定最小寬度避免過窄
        
        # 設定文字換行策略
        message_label.setTextFormat(Qt.AutoText)
        message_label.setSizePolicy(QtWidgets.QSizePolicy.Preferred, QtWidgets.QSizePolicy.Minimum)
        
        # 設定樣式
        style = """
            QLabel {
                background-color: #%s;
                border-radius: 10px;
                padding: 15px;
                margin: 5px;
                word-wrap: break-word;
                white-space: pre-wrap;
                line-height: 1.2;
                font-size: 14px;
            }
        """ % ("DCF8C6" if is_self else "FFFFFF")
        
        message_label.setStyleSheet(style)
        message_label.adjustSize()
        
        # 建立水平布局
        message_layout = QtWidgets.QHBoxLayout()
        message_layout.setContentsMargins(0, 0, 0, 0)
        
        if is_self:
            message_layout.addStretch()
            message_layout.addWidget(message_label)
        else:
            message_layout.addWidget(message_label)
            message_layout.addStretch()
        
        return message_layout

    def show_message(self, text, is_self=True):
        # 將訊息加入聊天區域
        message_layout = self.create_message_label(text, is_self)
        self.chat_content_layout.addLayout(message_layout)

    #聊天室到底部
    def scroll_to_bottom(self):
        self.chat_area.verticalScrollBar().setValue(self.chat_area.verticalScrollBar().maximum())

# 檔案選單區
    def file_input_selected(self):
        self.clear_window()
        print("選擇檔案輸入模式")
        # 清除現有的中央元件
        old_widget = self.centralWidget()
        if (old_widget):
            old_widget.deleteLater()
        
        # 建立新的中央元件
        central_widget = QtWidgets.QWidget()
        self.setCentralWidget(central_widget)
        
        # 建立主布局
        main_layout = QtWidgets.QVBoxLayout()
        central_widget.setLayout(main_layout)
        
        # 檔案選擇區域
        file_section = QtWidgets.QGroupBox("檔案選擇")
        file_layout = QtWidgets.QHBoxLayout()
        file_section.setLayout(file_layout)
        
        # 檔案路徑顯示
        self.file_path = QtWidgets.QLineEdit()
        self.file_path.setReadOnly(True)
        self.file_path.setPlaceholderText("請選擇圖片...")
        self.file_path.setStyleSheet("""
            QLineEdit {
                padding: 8px;
                border-radius: 5px;
                border: 1px solid #ccc;
                font-size: 14px;
            }
        """)
        
        # 瀏覽按鈕
        browse_button = QtWidgets.QPushButton("瀏覽")
        browse_button.setFixedSize(100, 40)
        browse_button.setStyleSheet("""
            QPushButton {
                border-radius: 20px;
                background-color: #4CAF50;
                color: white;
                border: none;
                font-size: 14px
            }
        """)
        browse_button.clicked.connect(self.browse_image)
        
        # 圖片預覽區域
        self.image_preview = QtWidgets.QLabel()
        self.image_preview.setFixedSize(400, 300)
        self.image_preview.setAlignment(QtCore.Qt.AlignCenter)
        self.image_preview.setStyleSheet("""
            QLabel {
                border: 2px dashed #ccc;
                border-radius: 10px;
                background-color: #f9f9f9;
            }
        """)
        
        # 添加到布局
        file_layout.addWidget(self.file_path)
        file_layout.addWidget(browse_button)
        main_layout.addWidget(file_section)
        main_layout.addWidget(self.image_preview)
        main_layout.addStretch()

# 瀏覽圖片
    def browse_image(self):
        # 設定支援的圖片格式
        image_formats = "圖片檔案 (*.png *.jpg *.jpeg *.bmp *.gif)"
        file_name, _ = QtWidgets.QFileDialog.getOpenFileName(
            self,
            "選擇圖片",
            "",
            image_formats
        )
        if file_name:
            self.file_path.setText(file_name)
            # 載入並顯示圖片預覽
            pixmap = QtGui.QPixmap(file_name)
            # 保持圖片比例並適應預覽區域大小
            scaled_pixmap = pixmap.scaled(
                self.image_preview.size(),
                QtCore.Qt.KeepAspectRatio,
                QtCore.Qt.SmoothTransformation
            )
            self.image_preview.setPixmap(scaled_pixmap)

# 更新可用序列埠列表
    def update_ports(self):
        self.port_combo.clear()
        ports = [port.device for port in serial.tools.list_ports.comports()]
        # 設置下拉選單樣式
        self.port_combo.setStyleSheet("""
            QComboBox {
                border: 2px solid #ccc;
                border-radius: 10px;
                padding: 5px;
                min-width: 150px;
                height: 30px;
                font-size: 16px;
                font-family: "Microsoft YaHei", "微軟正黑體";
                background-color: white;
            }
            QComboBox::drop-down {
                border: none;
                width: 30px;
            }
            QComboBox::down-arrow {
                image: url(down_arrow.png);
                width: 12px;
                height: 12px;
            }
            QComboBox QAbstractItemView {
                border: 2px solid #ccc;
                border-radius: 5px;
                selection-background-color: #4CAF50;
                selection-color: white;
            }
        """)
        
        # 如果沒有可用序列埠，添加提示文字
        if not ports:
            self.port_combo.addItem("未找到可用序列埠")
            print("未找到可用序列埠")
        else:
            self.port_combo.addItems(ports)
            print("可用序列埠列表:", ports)

#刷新串列埠清單
    def refresh_ports(self):
            
            self.port_combo.clear()
            ports = serial.tools.list_ports.comports()
            for port in ports:
                self.port_combo.addItem(port.device)

# 切換序列埠連接狀態
    def toggle_connection(self):
        if self.serial_port is None:  # 未連接狀態
            try:
                selected_port = self.port_combo.currentText()
                self.serial_port = serial.Serial(selected_port,serial_baud)
                #self.serial_port=selected_port
                self.connect_button.setText("斷開")
                self.connect_button.setStyleSheet("""
                    QPushButton {
                        border-radius: 20px;
                        background-color: #ff0000;
                        color: white;
                        border: none;
                        font-family: "Microsoft YaHei", "微軟正黑體";
                        font-size: 16px
                    }
                    QPushButton:hover {
                        background-color: #dc143c;
                    }
                """)
                self.port_combo.setEnabled(False)
                print("我連上的酷東西", selected_port)
                self.connect_check=True
                self.is_connected = True
                print(self.serial_port,serial_baud)
                self.start_listening_login()
                #ser = serial.Serial(serial_port, serial_baud)

            except Exception as e:
                QtWidgets.QMessageBox.critical(self, "錯誤", f"無法連接到序列埠: {str(e)}")
                self.serial_port = None
        else:  # 已連接狀態
            self.serial_port.close()
            self.serial_port = None
            self.connect_button.setText("連接")
            self.connect_button.setStyleSheet("""
                    QPushButton {
                        border-radius: 20px;
                        background-color: #4CAF50;
                        color: white;
                        border: none;
                        font-family: "Microsoft YaHei", "微軟正黑體";
                        font-size: 16px
                    }
                """)
            self.port_combo.setEnabled(True)
            self.connect_check=False
            self.is_connected = False
            print("已斷開序列埠連接")

#清除視窗
    def clear_window(self):
        self.setStyleSheet("")
        self.menuBar().clear()
        # 建立選單列
        menubar = self.menuBar()
        input_menu = menubar.addMenu('頁面選擇')
        
        # 首頁
        home_index_action = QtWidgets.QAction('回首頁', self)
        home_index_action.triggered.connect(self.home_index_selected)

        # 文字輸入
        text_input_action = QtWidgets.QAction('文字輸入', self)
        text_input_action.triggered.connect(self.text_input_selected)
        
        # 檔案輸入
        file_input_action = QtWidgets.QAction('檔案輸入', self)
        file_input_action.triggered.connect(self.file_input_selected)
        
        # 將動作加入選單
        input_menu.addAction(home_index_action)
        input_menu.addSeparator()   # 分隔線
        #input_menu.addAction(text_input_action)
        #input_menu.addAction(file_input_action)

if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())
