import sys
import os
from PyQt5 import QtWidgets, QtCore, QtGui
from PyQt5.QtGui import QIcon
from PyQt5.QtWidgets import (QLabel, QPushButton,QLineEdit)
from PyQt5.QtCore import QDateTime, Qt
import serial
import serial.tools.list_ports

# v 5.0

'''     環境設定     '''
title_name = "computer-A"   # 視窗標題
window_size = (900, 700)    # width, height
icon_path = os.path.join(os.path.dirname(__file__), "icon.png")     #先抓當前檔案的路徑,再加上icon
serial_baud=115200
#ser = serial.Serial(serial_port, serial_baud)

class MainWindow(QtWidgets.QMainWindow):
    def __init__(self):
        super().__init__()
        
        self.setObjectName("MainWindow")
        self.setWindowTitle(title_name)
        self.resize(window_size[0], window_size[1])

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
        title_label = QtWidgets.QLabel("首頁")
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
        """)
        self.refresh_button.clicked.connect(self.refresh_ports)
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
        """)
        self.login_button.setEnabled(False)  # 預設按鈕為禁用
        self.login_button.clicked.connect(self.text_input_selected)
        
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
        input_menu.addAction(text_input_action)
        input_menu.addAction(file_input_action)
        '''
        # 啟動接收訊息的計時器
        print("1")
        if check_serial==True:
            global ser
            print("check_serial",check_serial)
            #self.update_received_message()
            ser = serial.Serial(serial_port, serial_baud)
            print(ser)
        '''

    # 版面配置
    def create_section(self, parent_layout, label_text, placeholder):
        section_layout = QtWidgets.QVBoxLayout()
        # 標籤設置
        label = QtWidgets.QLabel(label_text)
        label.setAlignment(QtCore.Qt.AlignCenter)
        label.setStyleSheet("""
            QLabel {
                font-size: 24px;  /* 放大字體 */
                font-weight: bold;  /* 粗體 */
                color: #333333;  /* 深灰色文字 */
                padding: 5px;
                font-family: "Microsoft YaHei", "微軟正黑體";  /* 設置字體 */
            }
        """)
        
        # 文字編輯框
        text_edit = QtWidgets.QTextEdit()
        text_edit.setPlaceholderText(placeholder)
        text_edit.setStyleSheet("""
            QTextEdit {
                border-radius: 10px;
                border: 2px solid #ccc;
                padding: 5px;
                font-size: 16px;  /* 編輯框內文字大小 */
            }
        """)
        
        section_layout.addWidget(label)
        section_layout.addWidget(text_edit)
        parent_layout.addLayout(section_layout)

    # 首頁
    def home_index_selected(self):
        central_widget = QtWidgets.QWidget(self)
        self.setCentralWidget(central_widget)

        main_layout = QtWidgets.QVBoxLayout(central_widget)

        # 標題
        title_container = QtWidgets.QWidget()
        title_layout = QtWidgets.QVBoxLayout(title_container)
        
        title_label = QtWidgets.QLabel("首頁")
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
        """)
        self.refresh_button.clicked.connect(self.refresh_ports)
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
        """)
        self.login_button.setEnabled(False)  # 預設按鈕為禁用
        self.login_button.clicked.connect(self.text_input_selected)
        
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

    # 文字選單區
    def text_input_selected(self):
        central_widget = QtWidgets.QWidget(self)
        self.setCentralWidget(central_widget)

        main_layout = QtWidgets.QHBoxLayout(central_widget)
        main_layout.setSpacing(20)

        # 左側容器
        left_container = QtWidgets.QWidget()
        left_container.setMinimumWidth(int(window_size[0] * 0.6))
        left_container.setMaximumWidth(int(window_size[0] * 0.75))
        left_layout = QtWidgets.QVBoxLayout(left_container)
        left_layout.setContentsMargins(0, 0, 0, 0)
        
        # 建立並保存傳送和接收區域的引用
        self.send_area = self.create_section_with_return(left_layout, "傳送區", "請輸入內容...")
        self.receive_area = self.create_section_with_return(left_layout, "接收區", "等待接收...")
        
        main_layout.addWidget(left_container)
        
        # 右側容器
        right_container = QtWidgets.QWidget()
        right_layout = QtWidgets.QVBoxLayout(right_container)
        right_layout.setContentsMargins(0, 120, 0, 0)  # 調整上方邊距以對齊傳送區
        
        # 送出按鈕
        send_button = QtWidgets.QPushButton("送出")
        send_button.setFixedSize(150, 60)
        send_button.setStyleSheet("""
            QPushButton {
                border-radius: 30px;
                background-color: #4CAF50;
                color: white;
                border: none;
                font-size: 16px;
                font-family: "Microsoft YaHei", "微軟正黑體";  /* 新增字體設定 */
            }
            QPushButton:hover {
                background-color: #45a049;
            }
        """)
        send_button.clicked.connect(self.send_message)
        
        right_layout.addWidget(send_button, 0, QtCore.Qt.AlignTop)  # 使用 AlignTop 確保按鈕在頂部
        main_layout.addWidget(right_container)

    # 建立區塊並返回文字編輯框的引用
    def create_section_with_return(self, parent_layout, label_text, placeholder):
        section_layout = QtWidgets.QVBoxLayout()
        
        label = QtWidgets.QLabel(label_text)
        label.setAlignment(QtCore.Qt.AlignCenter)
        label.setStyleSheet("""
            QLabel {
                font-size: 24px;
                font-weight: bold;
                color: #333333;
                font-family: "Microsoft YaHei", "微軟正黑體";
            }
        """)
        
        text_edit = QtWidgets.QTextEdit()
        text_edit.setPlaceholderText(placeholder)
        text_edit.setStyleSheet("""
            QTextEdit {
                border-radius: 10px;
                border: 2px solid #ccc;
                padding: 5px;
                font-size: 16px;
            }
        """)
        
        section_layout.addWidget(label)
        section_layout.addWidget(text_edit)
        parent_layout.addLayout(section_layout)
        
        return text_edit

# 送訊息
    
    def send_message(self):
        message = self.send_area.toPlainText()
        if message:
            try:
                #message+="\n"
                self.serial_port.write(message.encode('UTF-8'))
                print(f"送出訊息-> {message}")
                #self.receive_area.setText(message)
                self.send_area.clear()
            except Exception as e:
                print(f"Error sending message: {e}")
    '''

    def send_message(self):
        """發送訊息"""
        print(self.ser)
        if not self.ser or not self.is_connected:
            QtWidgets.QMessageBox.warning(
                self,
                "連接錯誤",
                "請先連接序列埠"
            )
            return
            
        message = self.send_area.toPlainText()
        if not message:
            return
            
        try:
            message += "\n"
            self.ser.write(message.encode('UTF-8'))
            print(f"送出訊息-> {message}")
            self.send_area.clear()
        except Exception as e:
            print(f"發送錯誤: {e}")
            self.is_connected = False
            QtWidgets.QMessageBox.warning(
                self,
                "發送錯誤",
                f"無法發送訊息: {e}"
            )
    '''
# 接收訊息
    def serial_listen_data(self):
        try:
            if not self.is_connected or not self.serial_port.is_open:
                return None
                
            if self.serial_port.in_waiting:
                data = self.serial_port.readline().decode('UTF-8', errors='ignore').rstrip()
                print(f"Received raw data: {data}")  # 加入更多除錯訊息
                return data
            return None
        except serial.SerialException as e:
            print(f"Serial port error: {e}")
            self.is_connected = False  # 發生錯誤時更新狀態
            return None
    
    def update_received_message(self):
        if not self.is_connected:
            return
            
        try:
            received_data = self.serial_listen_data()
            if received_data:
                self.receive_area.append(received_data + '\n')  # 改用append而不是setText
                print(f"接收訊息-> {received_data}")
            
            # 確保Timer只在連接狀態下繼續
            if self.is_connected:
                QtCore.QTimer.singleShot(500, self.update_received_message)
                
        except Exception as e:
            print(f"Error receiving message: {e}")

    # 檔案選單區
    def file_input_selected(self):
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

    def refresh_ports(self):
            """刷新串列埠清單"""
            self.port_combo.clear()
            ports = serial.tools.list_ports.comports()
            for port in ports:
                self.port_combo.addItem(port.device)

    # 切換序列埠連接狀態
    def toggle_connection(self):
        global ser
        if self.serial_port is None:  # 未連接狀態
            try:
                selected_port = self.port_combo.currentText()
                self.serial_port = serial.Serial(selected_port,serial_baud)
                self.connect_button.setText("斷開")
                #self.serial_port=selected_port
                self.connect_button.setStyleSheet("""
                    QPushButton {
                        border-radius: 20px;
                        background-color: #ff0000;
                        color: white;
                        border: none;
                        font-family: "Microsoft YaHei", "微軟正黑體";
                        font-size: 16px
                    }
                """)
                self.port_combo.setEnabled(False)
                print("我連上的酷東西", selected_port)
                print(self.serial_port,serial_baud)
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
            print("已斷開序列埠連接")

if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())
