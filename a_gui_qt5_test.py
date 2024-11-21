import sys
import os
from PyQt5.QtWidgets import (
    QApplication, QMainWindow, QWidget, QLabel, QVBoxLayout,
    QHBoxLayout, QTextEdit, QLineEdit, QPushButton, QFileDialog,
    QScrollArea, QFrame, QMenuBar, QMenu, QSizePolicy
)
from PyQt5.QtCore import Qt
from PyQt5.QtGui import QIcon

title_name = "computer-A"   # 視窗標題
window_size = (850, 650)    # width, height
icon_path = os.path.join(os.path.dirname(__file__), "icon.png")     #先抓當前檔案的路徑,再加上icon

class ChatApp(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle(title_name)
        self.resize(window_size[0], window_size[1])

        # icon圖像設定
        if os.path.exists(icon_path):
            self.setWindowIcon(QIcon(icon_path))
        else:
            print("Icon file not found!")
        
        self.init_ui()

    def init_ui(self):
        # 主窗口的Widget
        central_widget = QWidget()
        self.setCentralWidget(central_widget)

        # 主佈局
        main_layout = QVBoxLayout()

        # 用戶名稱區域
        username_label = QLabel("用戶名稱", self)
        username_label.setAlignment(Qt.AlignLeft)
        username_label.setWordWrap(False)  # 添加自動換行
        main_layout.addWidget(username_label)

        # 聊天顯示區域
        self.chat_area = QScrollArea(self)
        self.chat_area.setWidgetResizable(True)
        self.chat_area.setVerticalScrollBarPolicy(Qt.ScrollBarAlwaysOn)

        # 聊天內容
        self.chat_content = QWidget()
        self.chat_content_layout = QVBoxLayout()
        self.chat_content.setLayout(self.chat_content_layout)
        self.chat_area.setWidget(self.chat_content)

        main_layout.addWidget(self.chat_area)

        # 底部輸入和按鈕區域
        input_layout = QHBoxLayout()

        # 輸入框
        self.input_field = QLineEdit(self)
        self.input_field.setPlaceholderText("輸入訊息...")
        input_layout.addWidget(self.input_field)

        # 文件欄位
        self.file_field = QLineEdit(self)
        self.file_field.setPlaceholderText("檔案.txt")
        self.file_field.setReadOnly(True)
        input_layout.addWidget(self.file_field)

        # 瀏覽按鈕
        browse_button = QPushButton("瀏覽", self)
        browse_button.clicked.connect(self.open_file_dialog)
        input_layout.addWidget(browse_button)

        # 傳送按鈕
        send_button = QPushButton("傳送", self)
        send_button.clicked.connect(self.send_message)
        input_layout.addWidget(send_button)

        main_layout.addLayout(input_layout)
        central_widget.setLayout(main_layout)

        # 新增選單欄
        self.create_menu_bar()

        # 設定布局的邊距和間距
        main_layout.setContentsMargins(10, 10, 10, 10)
        main_layout.setSpacing(10)

    def create_menu_bar(self):
        menu_bar = self.menuBar()

        # 新增選單
        view_menu = QMenu("視圖", self)
        menu_bar.addMenu(view_menu)

        # 新增「滑到底部」按鈕
        scroll_to_bottom_action = view_menu.addAction("滑到底部")
        scroll_to_bottom_action.triggered.connect(self.scroll_to_bottom)

    def open_file_dialog(self):
        options = QFileDialog.Options()
        file_path, _ = QFileDialog.getOpenFileName(self, "選擇文件", "", "All Files (*);;Text Files (*.txt)", options=options)
        if file_path:
            self.file_field.setText(file_path)

    def send_message(self):
        # 發送的訊息顯示在右側
        message = self.input_field.text()
        if message.strip():
            self.add_message(message, is_self=True)
            self.input_field.clear()

        # 模擬接收到訊息
        self.receive_message("這是一個回覆訊息！")

    def receive_message(self, message):
        # 接收到的訊息顯示在左側
        self.add_message(message, is_self=False)

    def create_message_label(self, text, is_self=True):
        # 建立訊息標籤
        message_label = QLabel(text)
        message_label.setWordWrap(True)  # 啟用自動換行
        message_label.setMaximumWidth(500)  # 最大寬度設為500
        
        # 使用 sizeHint 取得文字實際所需寬度
        label_width = message_label.fontMetrics().boundingRect(text).width() + 40  # 加上padding
        
        # 如果文字寬度小於500，則使用實際寬度
        if label_width < 500:
            message_label.setMinimumWidth(min(label_width, 500))
        else:
            message_label.setMinimumWidth(100)  # 設定最小寬度避免過窄
        
        # 設定文字換行策略
        message_label.setTextFormat(Qt.AutoText)
        message_label.setSizePolicy(QSizePolicy.Preferred, QSizePolicy.Minimum)
        
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
        message_layout = QHBoxLayout()
        message_layout.setContentsMargins(0, 0, 0, 0)
        
        if is_self:
            message_layout.addStretch()
            message_layout.addWidget(message_label)
        else:
            message_layout.addWidget(message_label)
            message_layout.addStretch()
        
        return message_layout

    def add_message(self, text, is_self=True):
        # 將訊息加入聊天區域
        message_layout = self.create_message_label(text, is_self)
        self.chat_content_layout.addLayout(message_layout)

    def scroll_to_bottom(self):
        """讓聊天視窗滑到底部"""
        self.chat_area.verticalScrollBar().setValue(self.chat_area.verticalScrollBar().maximum())


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = ChatApp()
    window.show()
    sys.exit(app.exec_())
