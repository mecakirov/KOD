import tkinter as tk
from tkinter import filedialog
import requests
import mimetypes
import os
import shutil
import threading
# cd python
# pyinstaller --onefile --console --add-data "libiconv.dll;." --add-data "libzbar-64.dll;." asenkron.py
# Tek bir dosya oluşturmak için aşağıdaki komutu kullanabilirsiniz:
# pyinstaller --onefile --noconsole --add-data "C:/Users/muham/Desktop/Kod/Python;." kurban_yukleme_final.py
# Global değişkenler
global_token = None
#video_paths[current_index] = None
ticket_response = None
video_extension = ""
video_mime_type = ""
#login_success = False

def login():
    global global_token
   # global login_success
    phone = entry_phone.get()
    password = entry_password.get()

    if not phone.isdigit() or len(phone) != 10:
        text_result.delete(1.0, tk.END)
        text_result.insert(tk.END, "Geçersiz telefon numarası!\nBaşında 0 ve +90 olmadan 10 haneli olmalı.")
        text_result.see(tk.END)
        return
    
    url = "https://www.iddef.org/api/v2/user/login"
   # url = "https://staging.iddef.org/api/v2/user/login"
    data = {
        "password": password,
        "phone_number": phone,
        "country_phone_code": "90",
        "one_time_password": None
    }

    try:
        response = requests.post(url, json=data, timeout=10)

        # Başarılı yanıt
        if response.status_code == 200:
            try:
                json_data = response.json()
                token = json_data.get("user", {}).get("token")
                if token:
                    global_token = token
                   # login_success = True
                    button_select_folder.config(state="normal")
                    text_result.delete(1.0, tk.END)
                    text_result.tag_configure('bold', font=('Arial', 10, 'bold'))
                    text_result.insert(tk.END, "Giriş başarılı\n\nVideoların bulunduğu klasörü seçiniz",'bold') 
                    text_result.insert(tk.END, "\nVideoların adında sadece kurban numarası yazmalıdır\nÖrnek: 123456.mp4")

                else:
                    text_result.delete(1.0, tk.END)
                    text_result.insert(tk.END, "Giriş başarısız: Token alınamadı.")
            except Exception as e:
                text_result.delete(1.0, tk.END)
                text_result.insert(tk.END, f"Yanıt çözümleme hatası:\n{str(e)}")

        # Yetkisiz / Hatalı bilgi / Diğer durumlar
        else:
            try:
                error_msg = response.json().get("message", None)
                if not error_msg:
                    error_msg = f"Hata kodu: {response.status_code}"
                text_result.delete(1.0, tk.END)
                text_result.insert(tk.END, f"Giriş başarısız:\n{error_msg}")
                text_result.see(tk.END)  # Autoscroll
            except ValueError:
                # JSON değilse
                text_result.delete(1.0, tk.END)
                text_result.insert(tk.END, f"Giriş başarısız (geçersiz yanıt):\n{response.text}")
                text_result.see(tk.END)  # Autoscroll

    except requests.exceptions.Timeout:
        text_result.delete(1.0, tk.END)
        text_result.insert(tk.END, "Bağlantı zaman aşımına uğradı. İnternetinizi kontrol edin.")
        text_result.see(tk.END)  # Autoscroll
    except requests.exceptions.ConnectionError:
        text_result.delete(1.0, tk.END)
        text_result.insert(tk.END, "Sunucuya bağlanılamıyor. Ağ bağlantınızı veya sunucuyu kontrol edin.")
        text_result.see(tk.END)  # Autoscroll
    except requests.exceptions.RequestException as e:
        text_result.delete(1.0, tk.END)
        text_result.insert(tk.END, f"Beklenmeyen bir hata oluştu:\n{str(e)}")
        text_result.see(tk.END)  # Autoscroll

def select_folder_and_prepare_videos_threaded():
    threading.Thread(target=select_folder_and_prepare_videos).start()


def select_folder_and_prepare_videos():
    global video_paths, current_index, folder_path
    video_paths = []  # Her seferinde sıfırlanmalı
    current_index = 0

    folder_path = filedialog.askdirectory()
    if not folder_path:
        return

    # "Hatalı" klasörü yoksa oluştur
    Hatalı = os.path.join(folder_path, "Hatalı")
    os.makedirs(Hatalı, exist_ok=True)

    # Tüm dosyaları tara
    for file_name in os.listdir(folder_path):
        file_path = os.path.join(folder_path, file_name)

        # Dosya mı kontrol et
        if not os.path.isfile(file_path):
            continue

        # Geçerli uzantı mı?
        ext = os.path.splitext(file_name)[1].lower()
        if ext not in [".mp4", ".avi", ".mov", ".mkv"]:
            shutil.move(file_path, os.path.join(Hatalı, file_name))
            continue


        # İsim formatı doğru mu?
        name_without_ext = os.path.splitext(file_name)[0]
        if not name_without_ext.isdigit():
            # Dosya adı yalnızca rakamlardan oluşmuyorsa "Hatalı" klasörüne taşı
            shutil.move(file_path, os.path.join(Hatalı, file_name))
            continue

        # Geçerliyse listeye ekle
        video_paths.append(file_path)



    # Sayaç yazdır
    if len(video_paths) == 0:
        text_result.delete(1.0, tk.END)
        text_result.insert(tk.END, "Video bulunamadı.")
        text_result.see(tk.END)  # Autoscroll
    else:
        button_upload.config(state="normal")
        text_result.delete(1.0, tk.END)
        text_result.insert(tk.END, f"{len(video_paths)} video yükleme işlemi için hazır.")
        text_result.see(tk.END)  # Autoscroll

def display_video_info(file_path): 
    global video_extension, video_mime_type
    file_name = os.path.basename(file_path)
    name_parts = file_name.split(".")
    if len(name_parts) != 2:
        return

    qurban_no, ext = name_parts
    ext = ext.lower()
    #Bu bilgilere artık gerek yok. Belge numarası 0 olarak gönderiliyor.
    # split_parts = name_without_ext.split("_")
   # if len(split_parts) != 3:
   #     return
    #qurban_no, document_no, qr_no = split_parts
    mime_type, _ = mimetypes.guess_type(file_path)
   # document_no = "00000"  # Sabit Belge Numarası

    entry_qurban.config(state="normal")
    entry_qurban.delete(0, tk.END)
    entry_qurban.insert(0, qurban_no)
    entry_qurban.config(state="readonly")

    # entry_document.config(state="normal")
    # entry_document.delete(0, tk.END)
    # entry_document.insert(0, document_no)
    # entry_document.config(state="readonly")

    video_extension = ext
    # entry_extension.config(state="normal")
    # entry_extension.delete(0, tk.END)
    # entry_extension.insert(0, ext)
    # entry_extension.config(state="readonly")

    video_mime_type = mime_type or "video"
    # entry_mime.config(state="normal")
    # entry_mime.delete(0, tk.END)
    # entry_mime.insert(0, mime_type or "video")
    # entry_mime.config(state="readonly")

    root.update()  # Arayüzü güncelle

def generate_ticket_and_upload_threaded():
    threading.Thread(target=generate_ticket_and_upload).start()


def generate_ticket_and_upload():
    global ticket_response
    
    button_select_folder.config(state="disabled")
    button_login.config(state="disabled")
    button_upload.config(state="disabled")

   # s3_url = "https://iddefstaging.s3.eu-central-1.amazonaws.com/"
    s3_url = "https://s3.eu-central-1.amazonaws.com/static.iddef.org/"
    
    if not global_token:
        text_result.delete(1.0, tk.END)
        text_result.insert(tk.END, "Önce giriş yapmalısınız!")
        text_result.see(tk.END)  # Autoscroll
        return

    total_videos = len(video_paths)
    for idx, video_path in enumerate(video_paths):
        current_video_name = os.path.basename(video_path)

        display_video_info(video_path)  # GUI'de bilgi göster ve global bilgi değişkenlerini güncelle

        data = {
            "city_name":"Unknown", 
            "country_name":"Unknown",
            "document_no": "00000",  # Sabit Belge Numarası
            "qurban_no": entry_qurban.get() or "Unknown",
            "extension": video_extension or "Unknown",
            "content_type": video_mime_type or "Unknown"
        }

        # 1. Ticket Oluştur
        try:
            response = requests.post(
               # "https://staging.iddef.org/api/v3/qurban/video/generate-ticket",
                "https://www.iddef.org/api/v3/qurban/video/generate-ticket",
                headers={"Authorization": f"Bearer {global_token}"},
                json=data,
                timeout=10
            )
            if response.status_code == 200:
                ticket_response = response.json()
                text_result.insert(tk.END, f"\n{idx + 1}/{total_videos}: {current_video_name} için yükleme işlemi başlatıldı.")
                text_result.see(tk.END)  # Autoscroll
                root.update()  # Arayüzü güncelle
            else:
                error_msg = response.json().get("message", "Ticket oluşturulamadı.")
                text_result.insert(tk.END, f"\n{idx + 1}/{total_videos}: {current_video_name} - {error_msg}")
                text_result.see(tk.END)  # Autoscroll
                root.update()
                continue
        except Exception as e:
            text_result.insert(tk.END, f"\n{idx + 1}/{total_videos}: {current_video_name} - Bağlantı hatası: {str(e)}")
            continue

        # 2. S3’e yükle
        conditions = ticket_response["conditions"]
        policy_base64 = ticket_response["policy_base64"]
        signature = ticket_response["signature"]

        post_data = {
            "key": conditions["key"],
            "bucket": conditions["bucket"],# canlıya özel bu kısım
            "acl": conditions["acl"],
            "Content-Type": conditions["Content-Type"],
            "success_action_redirect": conditions["success_action_redirect"],
            "x-amz-server-side-encryption": conditions["x-amz-server-side-encryption"],
            "x-amz-credential": conditions["x-amz-credential"],
            "x-amz-algorithm": conditions["x-amz-algorithm"],
            "x-amz-date": conditions["x-amz-date"],
            "policy": policy_base64,
            "x-amz-signature": signature
        }

        try:
            with open(video_path, "rb") as file:
                files = {"file": file}
                response = requests.post(s3_url, data=post_data, files=files, allow_redirects=False)
                text_result.insert(tk.END, response.text)  # Yanıtı göster")
                text_result.see(tk.END)  # Autoscroll
                root.update()  # Arayüzü güncelle
                files["file"].close()  # Dosya kapatma

                if response.status_code in [200, 204, 303]:
                    success_url = ticket_response["conditions"]["success_action_redirect"]
                    try:
                        response = requests.get(success_url)
                       # print(f"{video_path} için yükleme bildirimi gönderildi → {response.status_code}")
                    except Exception as e:
                        print(f"{video_path} için bildirim hatası:", str(e))

                    Gonderilen = os.path.join(folder_path, "Gonderilen")
                    os.makedirs(Gonderilen, exist_ok=True)
                    shutil.move(video_path, os.path.join(Gonderilen, current_video_name))
                    text_result.insert(tk.END, f"\n{idx + 1}/{total_videos}: {current_video_name} yüklendi ve taşındı.")
                    text_result.see(tk.END)  # Autoscroll
                else:
                    text_result.insert(tk.END, f"\n{idx + 1}/{total_videos}: {current_video_name} - Yükleme başarısız!")
                    text_result.see(tk.END)  # Autoscroll
        except Exception as e:
            text_result.insert(tk.END, f"\n{idx + 1}/{total_videos}: {current_video_name} - Yükleme Hatası: {str(e)}")
            text_result.see(tk.END)  # Autoscroll
    
    button_login.config(state="normal")
    button_select_folder.config(state="normal")
    button_upload.config(state="normal")




# Arayüz
root = tk.Tk()
root.title("Kurban Video Yükleme Programı")
#root.geometry("500x500")

# Giriş bölümü
tk.Label(root, text="Telefon Numarası:").grid(row=0, column=0)
entry_phone = tk.Entry(root)
#default phone number
#entry_phone.insert(0, "5387115053")
entry_phone.grid(row=0, column=1)

tk.Label(root, text="Şifre:").grid(row=1, column=0)
entry_password = tk.Entry(root, show="*")
#default password
#entry_password.insert(0, "123456")
entry_password.grid(row=1, column=1)

button_login = tk.Button(root, text="Giriş Yap", command=login,state="normal")
button_login.grid(row=2, columnspan=2, pady=5)


button_select_folder = tk.Button(
    root,
    text="Klasör Seç",
    command=select_folder_and_prepare_videos_threaded,
    state="disabled"  # Başlangıçta pasif
)
button_select_folder.grid(row=3, columnspan=2, pady=5)


# Ticket formu (ilk başta pasif)

#tk.Label(root, text="Şehir:").grid(row=4, column=0)
#entry_city = tk.Entry(root)
#entry_city.grid(row=4, column=1)


#tk.Label(root, text="Ülke:").grid(row=5, column=0)
#entry_country = tk.Entry(root)
#entry_country.grid(row=5, column=1)


# tk.Label(root, text="Dosya No:").grid(row=6, column=0)
# entry_document = tk.Entry(root, state="readonly")
# entry_document.grid(row=6, column=1)


tk.Label(root, text="Kurban No:").grid(row=7, column=0)
entry_qurban = tk.Entry(root, state="readonly")
entry_qurban.grid(row=7, column=1)


# tk.Label(root, text="Video Uzantısı:").grid(row=8, column=0)
# entry_extension = tk.Entry(root, state="readonly")
# entry_extension.grid(row=8, column=1)

# tk.Label(root, text="İçerik Tipi:").grid(row=9, column=0)
# entry_mime = tk.Entry(root, state="readonly")
# entry_mime.grid(row=9, column=1)

button_upload = tk.Button(root, text="Yükle", command=generate_ticket_and_upload_threaded,
                       state="disabled")
button_upload.grid(row=10, columnspan=2, pady=5)
# Uyarı mesajı
tk.Label(root, text="Uyarı: Her zaman yedekli çalışın",font=("Arial",8)).grid(row=11, columnspan=2, pady=5)
# Sonuç gösterimi
text_result = tk.Text(root, height=10, width=60)
text_result.grid(row=12, columnspan=2)


root.mainloop()
