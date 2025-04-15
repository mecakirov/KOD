import pandas as pd
import matplotlib.pyplot as plt
import os

# Excel dosyasını oku (sadece gerekli sütunları al, okuma hızını artır)
df = pd.read_excel("reports_27_02_2025.xlsx", usecols=["Device ID", "Checksum", "Signal At"], engine="openpyxl")


# ✅ Farklı formatlardaki tarihleri düzelt
def fix_date(date_value):
    if pd.isna(date_value):  # Eğer boşsa geç
        return None
    try:
        # Eğer zaten datetime ise, direkt döndür
        if isinstance(date_value, pd.Timestamp):
            return date_value
        # Eğer metin formatında tarih varsa çevir
        return pd.to_datetime(date_value, errors="coerce", dayfirst=False)
    except Exception:
        return None  # Hatalı verileri None yap

# Tüm tarihleri `datetime` formatına çevir
df["Signal At"] = df["Signal At"].apply(fix_date)

# Hatalı tarihleri kontrol et
df = df.dropna(subset=["Signal At"])  # Boş kalanları sil

# Device ID'lere göre gruplama yap
output_dir = "device_reports"
os.makedirs(output_dir, exist_ok=True)

for device_id, group in df.groupby("Device ID"):
    # Aylık bazda checksum toplamlarını hesapla
    data = group.resample("W", on="Signal At")["Checksum"].sum()
    #verileri haftalık bazda düzenlemek için "M" yerine "W
    # Grafiği çiz
    plt.figure(figsize=(10, 5))
    plt.plot(data.index, data.values, marker="x", linestyle="-", label=f"Device {device_id}")
    plt.xlabel("Ay")
    plt.ylabel("Su Kullanımı")
    plt.title(f"Device {device_id} Haftalık Su Kullanımı")
    plt.legend()
    plt.grid()

    # Dosya olarak kaydet
    file_path = os.path.join(output_dir, f"device_{device_id}.png")
    plt.savefig(file_path)
    plt.close()
    
    print(f"Grafik kaydedildi: {file_path}")

print("Tüm cihazlar için grafikler oluşturuldu!")

