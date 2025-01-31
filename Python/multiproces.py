import multiprocessing
import time
import os

def worker(n):
    """Her bir çekirdek bu fonksiyonu çalıştırarak verilen sayının karesini hesaplar."""
    print(f"Çekirdek {os.getpid()} - {n} sayısını işliyor...")
    time.sleep(1)  # Simüle edilmiş iş yükü (1 saniye bekletiyoruz)
    return n * n

if __name__ == "__main__":
    numbers = list(range(1, 21))  # 1'den 20'ye kadar olan sayılar
    available_cores = multiprocessing.cpu_count()-4  # Kullanılabilir çekirdek sayısını al

    print(f"Kullanılabilir Çekirdek Sayısı: {available_cores}")

    # İşlem havuzu oluştur
    with multiprocessing.Pool(available_cores) as pool:
        results = pool.map(worker, numbers)  # Tüm sayılar eşzamanlı işlenecek

    print("\nİşlem tamamlandı. Sonuçlar:")
    print(results)
