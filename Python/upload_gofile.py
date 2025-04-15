import os
import shutil
import tkinter as tk
from tkinter import filedialog, ttk
import threading
import requests
import time
import sys

class TextRedirector(object):
    def __init__(self, widget):
        self.widget = widget

    def write(self, text):
        self.widget.insert('end', text)
        self.widget.see('end')
        self.widget.update_idletasks()

    def flush(self):
        pass

# GoFile.io API Configuration
GOFILE_API_URL = "https://api.gofile.io"
ACCOUNT_TOKEN = "g82duFCCTrW11aqxxX74e1etbfj821JQ"
FOLDER_ID = "5WRPtI"  # Hedef klasör ID'si
FOLDER_NAME = "kurban"  # Hedef klasör adı

def get_server():
    try:
        response = requests.get(f"{GOFILE_API_URL}/getServer")
        if response.status_code == 200:
            return response.json()["data"]["server"]
        return "store1"
    except Exception as e:
        print(f"Sunucu alınırken hata: {e}")
        return "store1"

def upload_with_progress(file_path, upload_url, video_name):
    try:
        file_size = os.path.getsize(file_path)
        
        # Progress güncelleme fonksiyonu
        def update_progress(uploaded):
            percent = (uploaded / file_size) * 100
            progress_text = f"{video_name} %{percent:.1f}\n"
            progress_label.config(text=progress_text)
            current_progress.set(percent)
            root.update_idletasks()
        
        # Dosyayı binary modda aç
        with open(file_path, 'rb') as file:
            files = {'file': (video_name, file)}
            params = {
                'token': ACCOUNT_TOKEN,
                'folderId': FOLDER_ID,
                'folderName': FOLDER_NAME
            }
            
            # Yükleme işlemi
            response = requests.post(upload_url, files=files, params=params)
            
            if response.status_code == 200:
                result = response.json()
                if result['status'] == 'ok':
                    update_progress(file_size)  # %100 göster
                    return True, result['data']['downloadPage']
                return False, result['status']
            return False, f"HTTP {response.status_code}"
    except Exception as e:
        return False, str(e)

def upload_videos():
    selected_folder = folder_path.get()
    if not selected_folder:
        print("Lütfen önce bir klasör seçin!")
        return
    
    upload_button.config(state=tk.DISABLED)
    progress_frame.pack(pady=5)
    print("Yükleme işlemine başlandı...")
    
    changed_folder = selected_folder
    sent_folder = os.path.join(selected_folder, "sent")
    
    try:
        os.makedirs(sent_folder, exist_ok=True)
        
        videos = [f for f in os.listdir(changed_folder) 
                 if f.lower().endswith(('.mp4', '.avi', '.mov', '.mkv'))]
        
        total_files = len(videos)
        
        if not videos:
            print("Yüklenecek dosya bulunamadı!")
            upload_button.config(state=tk.NORMAL)
            return
        
        server = get_server()
        upload_url = f"https://{server}.gofile.io/uploadFile"
        
        for idx, video in enumerate(videos, 1):
            current_progress.set(0)
            progress_label.config(text=f"{video} %0.0")
            root.update_idletasks()
            
            video_path = os.path.join(changed_folder, video)
            temp_path = os.path.join(changed_folder, f"temp_{video}")
            
            try:
                if os.path.exists(video_path):
                    os.rename(video_path, temp_path)
                
                success, result = upload_with_progress(temp_path, upload_url, video)
                
                if success:
                    print(f"\n[{idx}/{total_files}] Yükleme başarılı: {video}")
                    print(f"Dosya URL: {result}")
                    shutil.move(temp_path, os.path.join(sent_folder, video))
                else:
                    print(f"\n[{idx}/{total_files}] Yükleme hatası: {result}")
                    if os.path.exists(temp_path):
                        os.rename(temp_path, video_path)
            except Exception as e:
                print(f"{video} yüklenirken hata: {str(e)}")
                if os.path.exists(temp_path):
                    os.rename(temp_path, video_path)
        
        print("\nTüm yüklemeler tamamlandı.")
    except Exception as e:
        print(f"Klasör işlemlerinde hata: {str(e)}")
    finally:
        upload_button.config(state=tk.NORMAL)
        progress_frame.pack_forget()

def select_folder():
    folder_selected = filedialog.askdirectory()
    if folder_selected:
        folder_path.set(folder_selected)
        print(f"Seçilen klasör: {folder_selected}")
        
        try:
            videos = [f for f in os.listdir(folder_selected) 
                     if f.lower().endswith(('.mp4', '.avi', '.mov', '.mkv'))]
            count_label.config(text=f"{len(videos)} video bulundu")
        except Exception as e:
            count_label.config(text="Klasör okunamadı")
            print(f"Klasör okuma hatası: {e}")

def start_upload():
    if not folder_path.get():
        print("Lütfen önce bir klasör seçin!")
        return
    thread = threading.Thread(target=upload_videos, daemon=True)
    thread.start()

# GUI Setup
root = tk.Tk()
root.title("GoFile Yükleyici")
root.geometry("700x500")

folder_path = tk.StringVar()
current_progress = tk.DoubleVar()

# UI Elements
tk.Label(root, text="GoFile.io Dosya Yükleyici", font=("Arial", 16)).pack(pady=10)

tk.Button(root, text="Klasör Seç", command=select_folder).pack(pady=5)
tk.Entry(root, textvariable=folder_path, width=50, state='readonly').pack(pady=5)

count_label = tk.Label(root, text="0 video bulundu", font=("Arial", 10))
count_label.pack()

output_text = tk.Text(root, height=15, width=80)
output_text.pack(pady=10)

sys.stdout = TextRedirector(output_text)

# Progress Frame
progress_frame = tk.Frame(root)
progress_label = tk.Label(progress_frame, text="", font=("Arial", 10, "bold"))
progress_label.pack()
progress_bar = ttk.Progressbar(progress_frame, orient="horizontal", length=300, mode="determinate", variable=current_progress)
progress_bar.pack(pady=5)

upload_button = tk.Button(root, text="Yüklemeyi Başlat", command=start_upload, 
                         bg="green", fg="white", state=tk.NORMAL)
upload_button.pack(pady=10)

root.mainloop()