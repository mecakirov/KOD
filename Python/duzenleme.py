import pandas as pd

# Ham verilerin olduğu Excel dosyasını oku
df = pd.read_excel("reports_27_02_2025V2.xlsx", engine="openpyxl")

# ✅ Tarihleri düzgün formatlamak için fonksiyon
def fix_date(date_value):
    if pd.isna(date_value):  # Eğer boşsa geç
        return None
    try:
        return pd.to_datetime(date_value, errors="coerce", dayfirst=False)
    except Exception:
        return None  # Hatalı verileri None yap

# "Signal At" sütunundaki tarihleri düzelt
df["Signal At"] = df["Signal At"].apply(fix_date)
df = df.dropna(subset=["Signal At"])  # Hatalı tarihleri sil

# ✅ Excel çıktısını oluştur
output_path = "duzenliV2.xlsx"
with pd.ExcelWriter(output_path, engine="openpyxl") as writer:
    for device_id, group in df.groupby("Device ID"):
        # Haftalık bazda checksum toplamlarını hesapla
        weekly_data = group.resample("W-MON", on="Signal At")["Checksum"].sum().reset_index()
        weekly_data.rename(columns={"Signal At": "Week Start"}, inplace=True)  # Tarih sütununu değiştir

        # Excel'e ayrı bir sekme olarak ekle
        weekly_data.to_excel(writer, sheet_name=str(device_id), index=False)

print(f"Veriler düzenlendi ve {output_path} dosyasına kaydedildi!")
