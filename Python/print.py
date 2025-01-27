import time
import tkinter as tk
from tkinter import filedialog

def select_image():
    # Tkinter root penceresini gizle
    root = tk.Tk()
    root.withdraw()  # Ana pencereyi gizle

    # Kullanıcıdan bir görsel dosyasını seçmesini iste
    
    folder_path = filedialog.askdirectory(
        title="videoların bulunduğu klasörü seçin",
       # filetypes=[("Görsel Dosyalar", "*.png;*.jpg;*.jpeg;*.bmp;*.gif"), ("Tüm Dosyalar", "*.*")]
    )
    
    if folder_path:
        print(f"Seçilen dosya: {folder_path}")
        return folder_path
    else:
        print("Hiçbir klasör seçilmedi.")
        return None

# Test için fonksiyonu çağır
image_path = select_image()
if image_path:
    print(f"Görsel dizini: {image_path}")

print("test");
time.sleep(2);
print("test2");
time.sleep(2);