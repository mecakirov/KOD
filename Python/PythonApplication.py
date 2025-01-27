import cv2
import os
import re
from pyzbar.pyzbar import decode
import tkinter as tk
from tkinter import filedialog

def clean_filename(name):
    """
    Cleans invalid characters from a filename.
    """
    cleaned_name = re.sub(r'[\\/*?:"<>|]', '_', name)
    return cleaned_name

def read_qr_code_from_frame(frame, video_file_path):
    """
    Reads QR code data from the frame and renames the video file accordingly.
    """
    try:
        # Detect and decode QR codes
        qr_codes = decode(frame)
    except Exception as e:
        print(f"Error decoding QR code: {e}")
        return None

    if not qr_codes:
        return None

    for qr in qr_codes:
        try:
            # Attempt to decode the QR code in 'utf-8', ignore errors if any
            data = qr.data.decode('utf-8', errors='ignore')  # 'ignore' skips invalid characters
        except UnicodeDecodeError as e:
            print(f"Error: {e}")
            data = qr.data.decode('latin1', errors='ignore')
        
        print(f"QR Code Data: {data}")

        # Clean the QR code data and use it as the new filename
        new_video_name = os.path.join(os.path.dirname(video_file_path), clean_filename(data) + os.path.splitext(video_file_path)[1])

        try:
            # Rename the video file with the cleaned QR code data
            os.rename(video_file_path, new_video_name)
            print(f"Video file name changed from '{os.path.basename(video_file_path)}' to '{os.path.basename(new_video_name)}'.")
        except Exception as e:
            print(f"Could not rename the file: {e}")
            return None
        
        return data

def extract_frame_at_timestamp(video_path, timestamp_ms):
    """
    Extracts a single frame from a video at the specified timestamp.

    Args:
        video_path (str): Path to the video file.
        timestamp_ms (int): Timestamp in milliseconds to extract the frame.
    """
    video = cv2.VideoCapture(video_path)
    
    if not video.isOpened():
        print(f"Cannot open video file {video_path}. Please check the file path.")
        return None
    
    video.set(cv2.CAP_PROP_POS_MSEC, timestamp_ms)
    ret, frame = video.read()
    video.release()

    if not ret:
        print(f"Could not read frame at {timestamp_ms} ms.")
        return None
    
    return frame

def process_video_for_qr_code(video_path, start_time, end_time, step):
    """
    Processes a video by extracting frames at user-defined intervals and attempts to read the QR code.

    Args:
        video_path (str): Path to the video file.
    """
    
    for timestamp_ms in range(start_time, end_time, step):
        print(f"Trying to read QR code at {timestamp_ms} ms.")
        
        frame = extract_frame_at_timestamp(video_path, timestamp_ms)
        
        if frame is None:
            continue
        
        qr_code_data = read_qr_code_from_frame(frame, video_path)
        
        if qr_code_data:
            print(f"QR code found at {timestamp_ms} ms. Process complete.")
            break  # Exit loop if QR code is found and processed
    else:
        print(f"No QR code found in the video from {video_path} within the time range.")


def process_videos_in_folder(folder_path, start_time, end_time, step):
    """
    Processes all video files in the specified folder by attempting to read QR codes.

    Args:
        folder_path (str): Path to the folder containing video files.
    """
    video_files = [f for f in os.listdir(folder_path) if f.lower().endswith(('.mp4', '.avi', '.mov', '.mkv'))]

    if not video_files:
        print("No video files found in the folder.")
        return

    for video_file in video_files:
        video_path = os.path.join(folder_path, video_file)
        print(f"Processing video: {video_file}")
        process_video_for_qr_code(video_path, start_time, end_time, step)

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

# Test the function to process all videos in a folder
process_videos_in_folder( 
    folder_path = select_image(),
    # folder_path = input("Lutfen klasor yolunu girin: "),  # Path to the folder containing video files
    start_time = int(input("Start_time: ")),
    end_time = int(input("End_time: ")) + 1,  
    step = int(input("Step: "))
)
