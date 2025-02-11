import sys
import time
from PyQt5.QtWidgets import QApplication, QWidget, QVBoxLayout, QTextEdit, QPushButton

class ConsoleOutput:
    def __init__(self, text_widget):
        self.text_widget = text_widget

    def write(self, text):
        self.text_widget.append(text)  # Yeni satır ekleme
        self.text_widget.ensureCursorVisible()  # En altta kalmasını sağla

    def flush(self):
        pass  # Boş bırakılabilir, gerekmiyor

class MyApp(QWidget):
    def __init__(self):
        super().__init__()

        self.initUI()

    def initUI(self):
        layout = QVBoxLayout()

        self.text_edit = QTextEdit()
        self.text_edit.setReadOnly(True)  # Sadece çıktı göstermek için
        layout.addWidget(self.text_edit)

        self.button = QPushButton("Başlat")
        self.button.clicked.connect(self.run_process)
        layout.addWidget(self.button)

        self.setLayout(layout)
        self.setWindowTitle("Terminal Arayüzü")
        self.setGeometry(200, 200, 600, 400)

        # Terminal çıktısını QTextEdit'e yönlendiriyoruz
        sys.stdout = ConsoleOutput(self.text_edit)
        sys.stderr = ConsoleOutput(self.text_edit)

    def run_process(self):
        print("İşlem başladı...")
        for i in range(5):
            print(f"Adım {i+1} tamamlandı.")
            time.sleep(1)  # Bekleme simülasyonu
        print("İşlem bitti!")

if __name__ == "__main__":
    app = QApplication(sys.argv)
    ex = MyApp()
    ex.show()
    sys.exit(app.exec_())
