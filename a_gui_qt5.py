from PyQt5 import QtWidgets, QtCore, QtGui
import sys

title_name = "computer-A"
window_size = (850, 650)  # width, height

class MainWindow(QtWidgets.QMainWindow):
    def __init__(self):
        super().__init__()
        
        self.setObjectName("MainWindow")
        self.setWindowTitle(title_name)
        self.resize(window_size[0], window_size[1])
        
        # 建立中央 Widget
        central_widget = QtWidgets.QWidget(self)
        self.setCentralWidget(central_widget)

        # 建立主布局
        main_layout = QtWidgets.QHBoxLayout(central_widget)  # 改為水平布局
        main_layout.setSpacing(20)  # 設置間距

        # 左側容器（包含傳送區和接收區）
        left_container = QtWidgets.QWidget()
        left_container.setMinimumWidth(int(window_size[0] * 0.6))  # 設置最小寬度
        left_container.setMaximumWidth(int(window_size[0] * 0.75))  # 設置最大寬度為視窗寬度的 2/3
        left_layout = QtWidgets.QVBoxLayout(left_container)
        left_layout.setContentsMargins(0, 0, 0, 0)  # 移除邊距
        
        # 建立傳送區
        self.create_section(left_layout, "傳送區", "請輸入內容...")
        self.create_section(left_layout, "接收區", "等待接收...")
        
        main_layout.addWidget(left_container)
        
        # 右側容器（包含按鈕）
        right_container = QtWidgets.QWidget()
        right_layout = QtWidgets.QVBoxLayout(right_container)
        right_layout.setContentsMargins(0, 140, 0, 0)  # 調整上方邊距以對齊傳送區
        
        # 送出按鈕
        send_button = QtWidgets.QPushButton("送出")
        send_button.setFixedSize(150, 60)  # 增加按鈕尺寸
        send_button.setStyleSheet("""
            QPushButton {
                border-radius: 30px;  /* 增加圓角半徑 */
                background-color: #4CAF50;
                color: white;
                border: none;
                font-size: 16px;  /* 增加字體大小 */
            }
            QPushButton:hover {
                background-color: #45a049;
            }
            QPushButton:pressed {
                background-color: #3d8b40;
            }
        """)
        
        right_layout.addWidget(send_button, 0, QtCore.Qt.AlignTop)  # 將按鈕置頂對齊
        main_layout.addWidget(right_container)

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

if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())
