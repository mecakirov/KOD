import cv2
import numpy as np
from pyzbar.pyzbar import decode

def process_image(image_path):
    # Görüntüyü yükle
    img = cv2.imread(image_path)

    # Görüntüyü gri tona çevir
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

    # QR kodunu algıla ve çöz
    qr_codes = decode(gray)

    if qr_codes:
        for qr in qr_codes:
            qr_data = qr.data.decode('utf-8')  # QR kod içeriği
            print(f"Bulunan QR Kod: {qr_data}")
            return qr_data
    else:
        print("QR kod bulunamadı.")
        return None

# Eğik veya düşük ışıklı görüntüler için perspektif düzeltme

def adjust_perspective(image_path):
    img = cv2.imread(image_path)
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

    # Kenarları algıla
    edges = cv2.Canny(gray, 50, 150)
    contours, _ = cv2.findContours(edges, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    # En büyük dikdörtgeni bul
    for contour in contours:
        approx = cv2.approxPolyDP(contour, 0.02 * cv2.arcLength(contour, True), True)
        if len(approx) == 4:  # Dört köşe algılanmışsa
            pts = np.array([pt[0] for pt in approx], dtype='float32')
            break

    # Perspektif düzeltme işlemi
    width, height = 600, 400  # Hedef boyut
    target_pts = np.array([[0, 0], [width, 0], [width, height], [0, height]], dtype='float32')
    matrix = cv2.getPerspectiveTransform(pts, target_pts)
    corrected_img = cv2.warpPerspective(img, matrix, (width, height))

    return corrected_img

if __name__ == "__main__":
    image_path = "test_image.png"  # Test görselinin yolu

    # Görüntüyü işleme
    print("Görüntü işleniyor...")
    corrected_image = adjust_perspective(image_path)

    # Düzeltme sonrası QR kodu çöz
    cv2.imwrite("corrected_image.png", corrected_image)  # Düzeltme sonrası görseli kaydet
    qr_data = process_image("corrected_image.png")

    if qr_data:
        print(f"Elde Edilen Belge No: {qr_data}")
    else:
        print("QR kod okunamadı.")
