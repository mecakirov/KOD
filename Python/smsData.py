#bu kod sms verilerini düzenleyip yeni bir excel dosyasına kaydeder
#Excel dosyasında her iki satırda bir cihaz verisi bulunmaktadır

import pandas as pd

# Excel dosyasını oku (tüm sekmeler)
file_path = "sms_aquawatch2.xlsx"  # Excel dosyanın adı neyse onu yaz
sheets = pd.read_excel(file_path, sheet_name=None, header=None)

# Yeni veri setini saklamak için liste
data = []

for sheet_name, df in sheets.items():
    for i in range(0, len(df), 2):  # 2 satırda bir veri olduğu için
        try:
            # Cihaz verisi olan satırı parçala
            row = df.iloc[i, 0].split(":")
            device_id = row[1]
            signal_quality = row[2]
            temperature = float(row[9])
            battery_voltage_1 = float(row[10])
            battery_voltage_2 = float(row[11])
            latitude = float(row[19])
            longitude = float(row[20])
            checksum = row[21]

            # Tarih verisini düzenle
            signal_at = pd.to_datetime(df.iloc[i + 1, 0])

            # Yeni formatta listeye ekle
            data.append([device_id, signal_quality, temperature, battery_voltage_1, 
                         battery_voltage_2, latitude, longitude, checksum, signal_at])
        except Exception as e:
            print(f"Hata oluştu: {e}")

# Sonucu DataFrame'e çevir
columns = ["Device ID", "Signal Quality", "Temperature", "Battery Voltage", "Battery Voltage 2", 
           "Latitude", "Longitude", "Checksum", "Signal At"]
new_df = pd.DataFrame(data, columns=columns)

# Yeni Excel dosyasına kaydet
new_df.to_excel("duzenlenmis_veriler2.xlsx", index=False)
print("İşlem tamamlandı! Yeni Excel dosyası kaydedildi.")
