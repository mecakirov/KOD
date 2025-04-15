#Bu kod sms verilerini düzenleyip yeni bir excel dosyasına kaydeder

import pandas as pd

# Türkçe ay isimleri
aylar = {
    1: "Ocak", 2: "Şubat", 3: "Mart", 4: "Nisan", 5: "Mayıs", 6: "Haziran",
    7: "Temmuz", 8: "Ağustos", 9: "Eylül", 10: "Ekim", 11: "Kasım", 12: "Aralık"
}

# 1) Excel dosyasını oku
df = pd.read_excel("repotrs_SMS_2.xlsx")  

# 2) "Signal At" sütunundaki boş verileri tespit et
df = df.dropna(subset=["Signal At"])  # NaN içeren satırları sil

# 3) Tarih formatına çevirirken hatalı olanları NaT yap
df["Signal At"] = pd.to_datetime(df["Signal At"], format="%b %d, %Y %H:%M:%S", errors='coerce')

# 4) Tarih dönüşümü başarısız olanları (NaT olanları) temizle
df = df.dropna(subset=["Signal At"])  

# 5) "Ay-Yıl" sütunu oluştur
df["Ay-Yıl"] = df["Signal At"].apply(lambda x: f"{aylar.get(x.month, 'Bilinmeyen Ay')} {x.year}")

# 6) Aylık bazda "Checksum" toplamını al
sonuc = (
    df.groupby(["Device ID", "Ay-Yıl"], as_index=False)["Checksum"]
      .sum()
      .rename(columns={"Checksum": "Toplam Checksum"})
)

# 7) Yeni Excel dosyasına yaz
sonuc.to_excel("AylikChecksumRaporu_SMS_2.xlsx", index=False)

print("Aylık Checksum raporu oluşturuldu: 'AylikChecksumRaporu.xlsx'")


