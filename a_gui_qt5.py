from PyQt5 import QtWidgets, QtCore, QtGui
import sys

# v 2.0

title_name = "computer-A"   # 視窗標題
window_size = (850, 650)    # width, height

class MainWindow(QtWidgets.QMainWindow):
    def __init__(self):
        super().__init__()
        
        self.setObjectName("MainWindow")
        self.setWindowTitle(title_name)
        self.resize(window_size[0], window_size[1])
        
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
        
        # 將標題和提示文字加入標題布局
        title_layout.addWidget(title_label)
        title_layout.addWidget(hint_label)
        
        # 將標題容器加入主布局
        main_layout.addWidget(title_container)
        main_layout.addStretch()  # 添加彈性空間，使標題置於上方
        
        # 建立選單列
        menubar = self.menuBar()
        input_menu = menubar.addMenu('輸入模式')
        
        # 建立文字輸入動作
        text_input_action = QtWidgets.QAction('文字輸入', self)
        text_input_action.triggered.connect(self.text_input_selected)
        
        # 建立檔案輸入動作
        file_input_action = QtWidgets.QAction('檔案輸入', self)
        file_input_action.triggered.connect(self.file_input_selected)
        
        # 將動作加入選單
        input_menu.addAction(text_input_action)
        input_menu.addAction(file_input_action)
        

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
        print(f"送出訊息-> {message}")
        if message:
            self.receive_area.setText(message)
            self.send_area.clear()

    # 檔案選單區
    def file_input_selected(self):
        print("選擇檔案輸入模式")
        # 清除現有的中央元件
        old_widget = self.centralWidget()
        if old_widget:
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


if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())
