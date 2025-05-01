import tkinter as tk
from tkinter import filedialog
from tkinter import ttk
from tkinter import messagebox
import requests
import mimetypes
import os
import shutil
# cd python
# pyinstaller --onefile --console --add-data "libiconv.dll;." --add-data "libzbar-64.dll;." asenkron.py
# Tek bir dosya oluşturmak için aşağıdaki komutu kullanabilirsiniz:
# pyinstaller --onefile --noconsole --add-data "C:/Users/muham/Desktop/Kod/Python;." kurban_yukleme_final.py
# Global değişkenler
global_token = None
#video_paths[current_index] = None
ticket_response = None

class App(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("Video Upload Tool")
        self.geometry("500x400")

        self.notebook = ttk.Notebook(self)
        self.notebook.pack(expand=True, fill="both")

        # Tab1: Login
        self.tab_login = ttk.Frame(self.notebook)
        self.notebook.add(self.tab_login, text="Login")

        # Tab2: Upload
        self.tab_upload = ttk.Frame(self.notebook)
        self.notebook.add(self.tab_upload, text="Upload")

        self.create_login_tab()
        self.create_upload_tab()


def login():
    global global_token

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
                    text_result.delete(1.0, tk.END)

                    text_result.insert(tk.END, "Giriş başarılı\nKlasör seçiniz")
                    text_result.insert(tk.END, "Videoların isim formalı KurbanNo_DosyaNo_QrNo şeklinde olmalıdır.\nÖrnek: 123456_123456_32A34C7.mp4")
                    enable_ticket_fields(True)
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
            continue

        # İsim formatı doğru mu?
        name_without_ext = os.path.splitext(file_name)[0]
        parts = name_without_ext.split("_")
        if len(parts) != 3:
            # Geçersiz dosya ismini "Hatalı" klasörüne taşı
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
        text_result.delete(1.0, tk.END)
        text_result.insert(tk.END, f"{len(video_paths)} video yükleme işlemi için hazır.")
        text_result.see(tk.END)  # Autoscroll

def display_video_info(file_path): 
    file_name = os.path.basename(file_path)
    name_parts = file_name.split(".")
    if len(name_parts) != 2:
        return

    name_without_ext, ext = name_parts
    ext = ext.lower()
    split_parts = name_without_ext.split("_")
    if len(split_parts) != 3:
        return

    qurban_no, document_no, qr_no = split_parts
    mime_type, _ = mimetypes.guess_type(file_path)
    #document_no = 1113132134  # Sabit

    entry_qurban.config(state="normal")
    entry_qurban.delete(0, tk.END)
    entry_qurban.insert(0, qurban_no)
    entry_qurban.config(state="readonly")

    entry_document.config(state="normal")
    entry_document.delete(0, tk.END)
    entry_document.insert(0, document_no)
    entry_document.config(state="readonly")

    entry_extension.config(state="normal")
    entry_extension.delete(0, tk.END)
    entry_extension.insert(0, ext)
    entry_extension.config(state="readonly")

    entry_mime.config(state="normal")
    entry_mime.delete(0, tk.END)
    entry_mime.insert(0, mime_type or "video")
    entry_mime.config(state="readonly")

    root.update()  # Arayüzü güncelle


def generate_ticket_and_upload():
    global ticket_response

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

        display_video_info(video_path)  # GUI'de bilgi göster

        data = {
            "city_name": entry_city.get() or "Unknown", 
            "country_name": entry_country.get() or "Unknown",
            #"document_no": entry_document.get() or "Unknown",
            "document_no":"Unknown",
            "qurban_no": entry_qurban.get() or "Unknown",
            "extension": entry_extension.get() or "Unknown",
            "content_type": entry_mime.get() or "Unknown"
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


def enable_ticket_fields(state=False): #
    state_option = "normal" if state else "disabled"
    for widget in ticket_widgets:
        widget.configure(state=state_option)



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

tk.Button(root, text="Giriş Yap", command=login).grid(row=2, columnspan=2, pady=5)

# Dosya seçme butonu
tk.Button(root, text="Klasör Seç", command=select_folder_and_prepare_videos).grid(row=3, columnspan=2, pady=5)

# Ticket formu (ilk başta pasif)

tk.Label(root, text="Şehir:").grid(row=4, column=0)
entry_city = tk.Entry(root)
entry_city.grid(row=4, column=1)


tk.Label(root, text="Ülke:").grid(row=5, column=0)
entry_country = tk.Entry(root)
entry_country.grid(row=5, column=1)


tk.Label(root, text="Dosya No:").grid(row=6, column=0)
entry_document = tk.Entry(root, state="readonly")
entry_document.grid(row=6, column=1)


tk.Label(root, text="Kurban No:").grid(row=7, column=0)
entry_qurban = tk.Entry(root, state="readonly")
entry_qurban.grid(row=7, column=1)


tk.Label(root, text="Video Uzantısı:").grid(row=8, column=0)
entry_extension = tk.Entry(root, state="readonly")
entry_extension.grid(row=8, column=1)


tk.Label(root, text="İçerik Tipi:").grid(row=9, column=0)
entry_mime = tk.Entry(root, state="readonly")
entry_mime.grid(row=9, column=1)

btn_ticket = tk.Button(root, text="Yükle", command=generate_ticket_and_upload)
btn_ticket.grid(row=10, columnspan=2, pady=5)
# Uyarı mesajı
tk.Label(root, text="Uyarı: Her zaman yedekli çalışın",font=("Arial",8)).grid(row=11, columnspan=2, pady=5)
# Sonuç gösterimi
text_result = tk.Text(root, height=10, width=60)
text_result.grid(row=12, columnspan=2)


# Ticket alanları topluca kontrol için liste
ticket_widgets = [
    entry_city, entry_country, 
    #entry_document, entry_qurban, 
    #entry_extension, entry_mime, btn_ticket,
]
enable_ticket_fields(False)

root.mainloop()
