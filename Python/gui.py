import cv2
import os
from pyzbar.pyzbar import decode, ZBarSymbol
import re
import time
import shutil
import tkinter as tk
from tkinter import filedialog
import threading
import sys
from multiprocessing import Pool, cpu_count, freeze_support


# Son step direk error klasörüne atabilir.
#cpu_count() ayarlı yapılabilebilir.
# sayaçlar textboxlara yazılacak
# kalan süre sayacı eklenecek
# cd python
# pyinstaller --onefile --console --add-data "libiconv.dll;." --add-data "libzbar-64.dll;." asenkron.py
## Gui ##

class RedirectText:
    """Terminal output redirection to a text widget"""
    def __init__(self, text_widget):
        self.output = text_widget # Çıktıyı göstereceğimiz widget

    def write(self, string): 
    
        self.output.insert(tk.END, string) # Çıktıyı ekle
        self.output.see(tk.END)  # Otomatik kaydırma

    def flush(self):
        pass  # Boş bırakılabilir, gereksiz hata almamak için

def process_videos():
    folder = folder_path.get()
    if folder:
        process_videos_in_folder(folder_path=folder)

def log_error(message):
    """Logs errors to a file."""
    with open("error_log.txt", "a", encoding="utf-8") as f:
        f.write(f"{time.strftime('%Y-%m-%d %H:%M:%S')} - {message}\n")

def clean_filename(name):
    """Cleans invalid characters from a filename and removes prefix before the first underscore."""
    name = re.sub(r'[\/*?:"<>|]', '_', name)  # Geçersiz karakterleri temizle
    name = re.sub(r'^[^_]*_', '', name, count=1)  # İlk '_' karakterine kadar olan kısmı sil
    #name = re.search(r'[^.]_*', name).group()  # Dosya uzantısından sonrasını sil
    return name

def read_qr_code_from_frame(frame, video_file_path, changed_folder, repeated_folder):
    """Reads QR code data from a frame and renames the video file accordingly."""
    try:
        qr_codes = decode(frame, symbols=[ZBarSymbol.QRCODE])  
    except Exception as e:
        log_error(f"QR code decoding error: {e}")
        return None
    
    if not qr_codes:
        return None

    for qr in qr_codes:
        data = qr.data.decode('utf-8', errors='ignore')
        new_video_name = os.path.join(changed_folder, clean_filename(data) + os.path.splitext(video_file_path)[1])

        if os.path.exists(new_video_name):  # Dosya zaten varsa, "repeated" klasörüne taşı
            print(f"File already exists: {new_video_name}. Moving to 'repeated' folder.")
            repeated_video_path = os.path.join(repeated_folder, os.path.basename(video_file_path))
            try:
                shutil.move(video_file_path, repeated_video_path)
                print(f"Moved to 'repeated' folder: {repeated_video_path}")
            except Exception as e:
                log_error(f"Error moving file to repeated folder: {e}")
            return None

        try: # Dosya adını değiştir ve taşı
            shutil.move(video_file_path, new_video_name)
           # os.rename(video_file_path, new_video_name)
            print(f"Video renamed: {os.path.basename(video_file_path)} -> {os.path.basename(new_video_name)}")
            return data
        except Exception as e:
            log_error(f"Error renaming file: {e}")
            return None

    return None

def extract_frame_at_timestamp(video_path, timestamp_ms):
    """Extracts a frame at the specified timestamp."""
    video = cv2.VideoCapture(video_path)
    if not video.isOpened():
        log_error(f"Cannot open video file {video_path}.")
        return None

    video.set(cv2.CAP_PROP_POS_MSEC, timestamp_ms)
    ret, frame = video.read()
    video.release()
    return frame if ret else None

def process_video_for_qr_code(video_path, changed_folder, repeated_folder, step_info):
    """Processes a video to detect QR codes at specified time intervals."""
    
    video = cv2.VideoCapture(video_path)
    if not video.isOpened():
        log_error(f"Cannot open video file {video_path}.")
        return False

    fps = video.get(cv2.CAP_PROP_FPS)
    total_frames = video.get(cv2.CAP_PROP_FRAME_COUNT)
    video_duration_ms = (total_frames / fps) * 1000  
    video.release()

    # end_time'ı video süresi ile sınırla
    end_time = min(step_info["end_time"], video_duration_ms)

    # start_time ve step değerlerini kullanarak while döngüsü ile frameleri işle
    timestamp_ms = step_info["start_time"]
    while timestamp_ms < end_time:
        frame = extract_frame_at_timestamp(video_path, timestamp_ms)
        if frame is None:
            break

        qr_code_data = read_qr_code_from_frame(frame, video_path, changed_folder, repeated_folder)
        if qr_code_data:
            return True

        # step değerini ekleyerek bir sonraki frame'e geç
        timestamp_ms += step_info["step"]

    return False

def process_video_step(video_path, step, changed_folder, repeated_folder, error_folder, step_index, total_steps, video_index, total_videos, remaining_videos):
    """Processes a single video with a given step."""
    # Klasörlerdeki video sayılarını hesapla
    changed_count = len(os.listdir(changed_folder))
    repeated_count = len(os.listdir(repeated_folder))
    error_count = len(os.listdir(error_folder))
    #count .mp4 .avi .mov .mkv files in the folder
    remaining_videos = len([f for f in os.listdir(os.path.dirname(video_path)) if f.lower().endswith(('.mp4', '.avi', '.mov', '.mkv'))])
    # remaining_videos = len(os.listdir(os.path.dirname(video_path)))-3
   # remaining_videos = total_videos - (changed_count + repeated_count + error_count)
    
    print(f"Processing video: {os.path.basename(video_path)} step {step_index}/{total_steps} {video_index}/{total_videos}, Changed: {changed_count}, non-detected: {error_count}, Repeated: {repeated_count}, Remaining: {remaining_videos}")
    if process_video_for_qr_code(video_path, changed_folder, repeated_folder, step):
        return video_path
    return None

def process_videos_in_folder(folder_path):
    start_time = time.time()

    changed_folder = os.path.join(folder_path, "changed")
    error_folder = os.path.join(folder_path, "non_detected")
    repeated_folder = os.path.join(folder_path, "repeated")  

    os.makedirs(changed_folder, exist_ok=True)
    os.makedirs(error_folder, exist_ok=True)
    os.makedirs(repeated_folder, exist_ok=True)  

    video_files = [os.path.join(folder_path, f) for f in os.listdir(folder_path) if f.lower().endswith(('.mp4', '.avi', '.mov', '.mkv'))]
    total_videos = len(video_files)
    remaining_videos = total_videos  # Başlangıçta kalan video sayısı toplam video sayısına eşit

    steps = [
        {"start_time": 0 * 1000, "end_time": 5 * 1000, "step": 1000},  
        {"start_time": 6 * 1000, "end_time": 15 * 1000, "step": 1000},
        {"start_time": 0.5 * 1000, "end_time": 5 * 1000, "step": 1000},
        {"start_time": 5.5 * 1000, "end_time": 10 * 1000, "step": 1000},  # mükrerrer frame atlanacak 
        {"start_time": 0.25 * 1000, "end_time": 5 * 1000, "step": 500},
        {"start_time": 5.25 * 1000, "end_time": 10 * 1000, "step": 500}  # mükrerrer frame atlanacak    
    ]

    # Her adım için paralel işleme
    for step_index, step in enumerate(steps, start=1):
        print(f"\n--- Step {step_index}/{len(steps)} ---")
        with Pool(processes=cpu_count()) as pool:
            # process_video_step fonksiyonuna ek argümanları gönder
            results = pool.starmap(
                process_video_step,
                [
                    (
                        video_path,
                        step,
                        changed_folder,
                        repeated_folder,
                        error_folder,
                        step_index,
                        len(steps),
                        video_index + 1,
                        total_videos,
                        remaining_videos
                    )
                    for video_index, video_path in enumerate(video_files)
                ]
            )
        
        # İşlenen videoları listeden çıkar
        processed_videos = [result for result in results if result is not None]
        video_files = [video_path for video_path in video_files if video_path not in processed_videos]

    # Kalan videoları "error" klasörüne taşı
    for video_path in video_files:
        error_video_path = os.path.join(error_folder, os.path.basename(video_path))
        try:
            os.rename(video_path, error_video_path)
            print(f"Video moved to 'error' folder: {error_video_path}")
        except Exception as e:
            log_error(f"Error moving file to error folder: {e}")

    elapsed_time = time.time() - start_time
    print(f"\nProcessing completed in {elapsed_time:.2f} seconds.")
    #Expect press any key
    input("Press any key to exit...")

def start_process():
    """Start button click event"""
    if not folder_path.get():
        print("Please select a folder first!")
        return

    print(f"Selected folder: {folder_path.get()}")
    thread = threading.Thread(target=process_videos, daemon=True)
    thread.start()

def select_folder():
    """Klasör seçme işlemi"""
    path = filedialog.askdirectory(title="Select folder containing videos")
    if path:
        folder_path.set(path)
        return path

if __name__ == "__main__":
    freeze_support()  # Windows'ta multiprocessing için gerekli

    # Ana pencere
    root = tk.Tk()
    root.title("Video QR Scanner")  # Pencere başlığı
    root.geometry("500x400")

    # Program ismi
    labelTitle = tk.Label(root, text="Video QR Scanner", font=("Arial", 16, "bold"))
    labelTitle.pack(pady=10)

    # Klasör seçme butonu
    folder_path = tk.StringVar()
    btnSelect = tk.Button(root, text="Folder Select", command=select_folder)
    btnSelect.pack()

    # Seçilen klasörü gösteren alan
    textFolder = tk.Entry(root, textvariable=folder_path, width=50, state="readonly")
    textFolder.pack(pady=5)

    # Çıktı alanı
    textMultiOut = tk.Text(root, height=10, width=60)
    textMultiOut.pack(pady=5)

    # Çıktıları yönlendirmek için stdout'u değiştir
    sys.stdout = RedirectText(textMultiOut) # Çıktıları yönlendir
   # sys.stderr = RedirectText(textMultiOut) # Hata çıktılarını da yönlendir

    # Start butonu
    btnStart = tk.Button(root, text="Start", command=start_process, bg="green", fg="white")
    btnStart.pack(pady=10)

    root.mainloop()