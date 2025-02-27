import time
import pandas as pd
import openpyxl
from selenium import webdriver
from selenium.webdriver.chrome.service import Service
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from webdriver_manager.chrome import ChromeDriverManager

# WebDriver başlat
service = Service(ChromeDriverManager().install())
driver = webdriver.Chrome(service=service)

# Web sitesine git
driver.get("https://admin.waterhero.io/login")
wait = WebDriverWait(driver, 10)

try:
    # Giriş işlemi
    email_input = wait.until(EC.presence_of_element_located((By.ID, "email")))
    email_input.send_keys("admin@admin.com")
    password_input = driver.find_element(By.ID, "password")
    password_input.send_keys("admin")
    login_button = wait.until(EC.element_to_be_clickable((By.XPATH, "//button[@type='submit']")))
    login_button.click()
    print("Giriş yapıldı!")

    # Reports sayfasına git
    reports_link = wait.until(EC.element_to_be_clickable((By.XPATH, "//span[contains(text(), 'Reports')]")))
    reports_link.click()
    print("Reports sayfasına geçildi!")

    # 50 kayıt gösterme
    records_dropdown = wait.until(EC.element_to_be_clickable((By.ID, "tableRecordsPerPageSelect")))
    records_dropdown.click()
    option_50 = wait.until(EC.element_to_be_clickable((By.XPATH, "//option[@value='50']")))
    option_50.click()
    print("Liste 50'şerli gösterilecek şekilde ayarlandı!")

    time.sleep(2)  # Sayfanın yüklenmesi için bekle

    excel_path = "reportsV2.xlsx"
    try:
        workbook = openpyxl.load_workbook(excel_path)  # Var olan Excel dosyasını aç
        sheet = workbook.active  # İlk sayfayı kullan
    except FileNotFoundError:
        workbook = openpyxl.Workbook()  # Eğer dosya yoksa yeni oluştur
        sheet = workbook.active
        sheet.append([
            "Device ID", "Signal Quality", "Temperature", "Battery Voltage",
            "Battery Voltage 2", "Latitude", "Longitude", "Checksum", "Signal At"
        ])  # Başlık satırı ekle

    page = 704  # Başlanılacak sayfa numarası

    while True:
        driver.get(f"https://admin.waterhero.io/reports?page={page}")  # Sayfa URL'sini değiştirerek ilerle
        time.sleep(1)  # Sayfanın yüklenmesini bekle
        # records_dropdown = wait.until(EC.element_to_be_clickable((By.ID, "tableRecordsPerPageSelect")))
        rows = driver.find_elements(By.XPATH, "//table/tbody/tr")  # Tablodaki tüm satırları al
               
        if not rows:
            break  # Eğer yeni veri yoksa döngüden çık

        for row in rows:
            columns = row.find_elements(By.TAG_NAME, "td")
            row_data = [col.text for col in columns]  # Metinleri al
            if len(row_data) >= 9:  # Eğer 9 sütun varsa
                def get_text(col):
                    """Her hücredeki span içindeki metni çeker, yoksa boş döner."""
                    try:
                        return col.find_element(By.TAG_NAME, "span").text.strip()
                    except:
                        return col.text.strip()

                device_id = get_text(columns[2])
                signal_quality = get_text(columns[3])
                temperature = get_text(columns[4])
                battery_voltage = get_text(columns[5])
                battery_voltage_2 = get_text(columns[6])
                latitude = get_text(columns[7])
                longitude = get_text(columns[8])
                checksum = get_text(columns[9])
                signal_at = get_text(columns[10])

                data = [device_id, signal_quality, temperature, battery_voltage,
                        battery_voltage_2, latitude, longitude, checksum, signal_at]
                #print(data)
                if "Search" not in data:  # "Search" içeren hatalı satırı atla
                    sheet.append(data)  # Yeni veriyi ekle

        workbook.save(excel_path)  # Her sayfa çekildiğinde kaydet
        print(f"Excel dosyası güncellendi! Sayfa: {page}")

        page += 1  # Bir sonraki sayfaya geç

    print("Tüm veriler başarıyla kaydedildi!")

except Exception as e:
    print("Hata oluştu:", e)

finally:
    driver.quit()  # Tarayıcıyı kapat
