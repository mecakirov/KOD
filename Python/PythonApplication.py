#Error renaming file: [WinError 183] Halen varolan bir dosya oluşturulamaz: 'C:/Users/muham/Desktop/v2 - Kopya\\IMG_0957.MOV' -> 'C:/Users/muham/Desktop/v2 - Kopya\\changed\\13170_270603_e23fe3.MOV'
# var olan dosya kontrolü eklenecek. hata olunca loglanacak.
# mükerrer frame atlanacak 
#asenkron çalışacak
import cv2
import os
from pyzbar.pyzbar import decode, ZBarSymbol
import re
import time
import tkinter as tk
import shutil
import tkinter as tk
from tkinter import filedialog

class RedirectText:
    """Terminal output redirection to a text widget"""
    def __init__(self, text_widget):
        self.output = text_widget # Çıktıyı göstereceğimiz widget

    def write(self, string): 
    
        self.output.insert(tk.END, string) # Çıktıyı ekle
        self.output.see(tk.END)  # Otomatik kaydırma

    def flush(self):
        pass  # Boş bırakılabilir, gereksiz hata almamak için

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

        try:
            os.rename(video_file_path, new_video_name)
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
        # videonun uzunluğunu al
    fps = video.get(cv2.CAP_PROP_FPS)
    total_frames = video.get(cv2.CAP_PROP_FRAME_COUNT)
    video_duration_ms = (total_frames / fps) * 1000  
    video.release()

    end_time = min(step_info["end_time"], int(video_duration_ms))

    for timestamp_ms in range(step_info["start_time"], end_time, step_info["step"]):
        # timsatamp_ms değerindeki frame'i al
        frame = extract_frame_at_timestamp(video_path, timestamp_ms)
        if frame is None:
            break

        qr_code_data = read_qr_code_from_frame(frame, video_path, changed_folder, repeated_folder)
        if qr_code_data:
            return True  

    return False  

def process_videos_in_folder(folder_path):
    start_time = time.time()

    changed_folder = os.path.join(folder_path, "changed")
    error_folder = os.path.join(folder_path, "non_detected")
    repeated_folder = os.path.join(folder_path, "repeated")  

    os.makedirs(changed_folder, exist_ok=True)
    os.makedirs(error_folder, exist_ok=True)
    os.makedirs(repeated_folder, exist_ok=True)  

    video_files = [os.path.join(folder_path, f) for f in os.listdir(folder_path) if f.lower().endswith(('.mp4', '.avi', '.mov', '.mkv'))]
    remaining_videos = video_files
    total_videos = len(video_files)

    steps = [
         {"start_time": 0*1000, "end_time": 5 * 1000, "step": 1000},  
         {"start_time": 6*1000, "end_time": 15 * 1000, "step": 1000},
         {"start_time": 0.5*1000, "end_time": 5 * 1000, "step": 1000} ,
         {"start_time": 5.5*1000, "end_time": 10 * 1000, "step": 1000},  # mükrerrer frame atlanacak 
         {"start_time": 0.25*1000, "end_time": 5 * 1000, "step": 500},
         {"start_time": 5.25*1000, "end_time": 10 * 1000, "step": 500} 
          
    ]

    for step_index, step in enumerate(steps, start=1):
        print(f"\n--- Step {step_index}/{len(steps)} ---")
        processed_videos = []
        for video_index, video_path in enumerate(remaining_videos, start=1):
            print(f"Processing video: {os.path.basename(video_path)} step {step_index}/{len(steps)} {video_index}/{total_videos}")
            if process_video_for_qr_code(video_path, changed_folder, repeated_folder, step):
                processed_videos.append(video_path)

        remaining_videos = [v for v in remaining_videos if v not in processed_videos]

    for video_path in remaining_videos:
        error_video_path = os.path.join(error_folder, os.path.basename(video_path))
        try:
            os.rename(video_path, error_video_path)
            print(f"Video moved to 'error' folder: {error_video_path}")
        except Exception as e:
            log_error(f"Error moving file to error folder: {e}")

    elapsed_time = time.time() - start_time
    print(f"\nProcessing completed in {elapsed_time:.2f} seconds.")

def select_folder():
    """Opens a file dialog for the user to select a folder."""
    root = tk.Tk()
    root.withdraw()
    folder_path = filedialog.askdirectory(title="Select folder containing videos")
    return folder_path if folder_path else None

# Run the script
folder = select_folder()
if folder:
    process_videos_in_folder(folder_path=folder)


# read_qr_code_with_opencv(frame):
#     qr_decoder = cv2.QRCodeDetector()
#     data, points, _ = qr_decoder.detectAndDecode(frame)
#     return data if points is not None else None

# import cv2
# import os
# import re
# from pyzbar.pyzbar import decode
# import tkinter as tk
# from tkinter import filedialog

# QR kod içersindeki sembolleri siler
# def clean_filename(name):
#     """
#     Cleans invalid characters from a filename.
#     """
#     cleaned_name = re.sub(r'[\\/*?:"<>|]', '_', name)
#     return cleaned_name

# def read_qr_code_from_frame(frame, video_file_path, changed_folder, error_folder):
#     """
#     Reads QR code data from the frame and renames the video file accordingly.
#     """
#     try:
#         Detect and decode QR codes
#         qr_codes = decode(frame)
#     except Exception as e:
#         print(f"Error decoding QR code: {e}")
#         return None

#     if not qr_codes:
#         return None

#     for qr in qr_codes:
#         try:
#             Attempt to decode the QR code in 'utf-8', ignore errors if any
#             data = qr.data.decode('utf-8', errors='ignore')  # 'ignore' skips invalid characters
#         except UnicodeDecodeError as e:
#             print(f"Error: {e}")
#             data = qr.data.decode('latin1', errors='ignore')

#         print(f"QR Code Data: {data}")

#         Clean the QR code data and use it as the new filename
#         new_video_name = os.path.join(os.path.dirname(video_file_path), clean_filename(data) + os.path.splitext(video_file_path)[1])

#         try:
#             Rename the video file with the cleaned QR code data
#             os.rename(video_file_path, new_video_name)
#             print(f"Video file name changed from '{os.path.basename(video_file_path)}' to '{os.path.basename(new_video_name)}'.")

#             Move to the "changed" folder
#             changed_video_path = os.path.join(changed_folder, os.path.basename(new_video_name))
#             os.rename(new_video_name, changed_video_path)
#             print(f"File moved to 'changed' folder: {changed_video_path}")
#         except Exception as e:
#             print(f"Could not rename the file: {e}")
#             Move to the "error" folder
#             error_video_path = os.path.join(error_folder, os.path.basename(video_file_path))
#             os.rename(video_file_path, error_video_path)
#             print(f"File moved to 'error' folder: {error_video_path}")
#             return None
        
#         return data

# def extract_frame_at_timestamp(video_path, timestamp_ms):
#     """
#     Extracts a single frame from a video at the specified timestamp.

#     Args:
#         video_path (str): Path to the video file.
#         timestamp_ms (int): Timestamp in milliseconds to extract the frame.
#     """
#     video = cv2.VideoCapture(video_path)
    
#     if not video.isOpened():
#         print(f"Cannot open video file {video_path}. Please check the file path.")
#         return None
    
#     video.set(cv2.CAP_PROP_POS_MSEC, timestamp_ms)
#     ret, frame = video.read()
#     video.release()

#     if not ret:
#         print(f"Could not read frame at {timestamp_ms} ms.")
#         return None
    
#     return frame

# def process_video_for_qr_code(video_path, changed_folder, error_folder):
#     """
#     Processes a video by checking for QR codes at specified intervals in a defined sequence.

#     Args:
#         video_path (str): Path to the video file.
#     """
#     steps = [
#         {"start_time": 0, "end_time": 10 * 1000, "step": 1000},   # 0-10 seconds, 1-second intervals
#         {"start_time": 0, "end_time": 10 * 1000, "step": 500},    # 0-10 seconds, 0.5-second intervals
#         {"start_time": 0, "end_time": 5 * 1000, "step": 40}       # 0-5 seconds, every frame
#     ]

#     for step in steps:
#         start_time = step["start_time"]
#         end_time = step["end_time"]
#         step_interval = step["step"]

#         for timestamp_ms in range(start_time, end_time, step_interval):
#             print(f"Trying to read QR code at {timestamp_ms} ms.")

#             frame = extract_frame_at_timestamp(video_path, timestamp_ms)

#             if frame is None:
#                 continue

#             qr_code_data = read_qr_code_from_frame(frame, video_path, changed_folder, error_folder)

#             if qr_code_data:
#                 print(f"QR code found at {timestamp_ms} ms. Process complete.")
#                 return  # Exit if QR code is found and processed

#     If no QR code is found after all steps, move the video to the error folder
#     error_video_path = os.path.join(error_folder, os.path.basename(video_path))
#     os.rename(video_path, error_video_path)
#     print(f"No QR code found. File moved to 'error' folder: {error_video_path}")


# def process_videos_in_folder(folder_path):
#     """
#     Processes all video files in the specified folder by attempting to read QR codes.

#     Args:
#         folder_path (str): Path to the folder containing video files.
#     """
#     Create folders for changed and error files
#     changed_folder = os.path.join(folder_path, "changed")
#     error_folder = os.path.join(folder_path, "error")
#     os.makedirs(changed_folder, exist_ok=True)
#     os.makedirs(error_folder, exist_ok=True)

#     video_files = [f for f in os.listdir(folder_path) if f.lower().endswith(('.mp4', '.avi', '.mov', '.mkv'))]

#     if not video_files:
#         print("No video files found in the folder.")
#         return

#     for video_file in video_files:
#         video_path = os.path.join(folder_path, video_file)
#         print(f"Processing video: {video_file}")
#         process_video_for_qr_code(video_path, changed_folder, error_folder)


# def select_folder():
#     """
#     Opens a file dialog for the user to select a folder and returns its path.
#     """
#     root = tk.Tk()
#     root.withdraw()  # Hide the root window

#     folder_path = filedialog.askdirectory(title="Videoların bulunduğu klasörü seçin")
#     if folder_path:
#         print(f"Seçilen klasör: {folder_path}")
#         return folder_path
#     else:
#         print("Hiçbir klasör seçilmedi.")
#         return None

# Execute the video processing
# folder = select_folder()
# if folder:
#     process_videos_in_folder(folder_path=folder)

# Test the function to process all videos in a folder
# folder = select_folder()
# if folder:
#     process_videos_in_folder(
#         folder_path=folder,
#         start_time=int(input("Start_time: ")),
#         end_time=int(input("End_time: ")) + 1,  
#         step=int(input("Step: "))
#     )
