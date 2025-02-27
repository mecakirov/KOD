import pandas as pd
import matplotlib.pyplot as plt
import os

# Excel dosyasını oku
df = pd.read_excel("reports_27_02_2025.xlsx")

# Tarih sütununu datetime formatına çevir
df["Signal At"] = pd.to_datetime(df["Signal At"])

# Device ID'lere göre gruplama yap
output_dir = "device_reports"
os.makedirs(output_dir, exist_ok=True)

for device_id, group in df.groupby("Device ID"):
    # Haftalık bazda checksum toplamlarını hesapla
    weekly_data = group.resample("M", on="Signal At")["Checksum"].sum()
    
    # Grafiği çiz
    plt.figure(figsize=(10, 5))
    plt.plot(weekly_data.index, weekly_data.values, marker="x", linestyle="-", label=f"Device {device_id}")
    plt.xlabel("Hafta")
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
