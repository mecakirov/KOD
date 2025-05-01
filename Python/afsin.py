import cv2
import pytesseract
import pyttsx3
from gtts import gTTS
import os


# Tesseract OCR'in kurulu olduğu dizini belirtin (Windows için gerekli olabilir)
pytesseract.pytesseract.tesseract_cmd = r"C:\Program Files\Tesseract-OCR\tesseract.exe"

def extract_text_from_image(image_path):
    # Görseli yükle
    image = cv2.imread(image_path)

    # Görseli gri tonlamaya çevir
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

    # Gürültüyü azalt
    gray = cv2.medianBlur(gray, 3)

    # OCR ile metni çıkart
    text = pytesseract.image_to_string(gray, lang="tur")  # Türkçe için "tur" kullanabilirsiniz.
    
    return text

def text_to_speech(text):
    engine = pyttsx3.init()

    # Kullanılabilir sesleri listele
    voices = engine.getProperty('voices')

    # Türkçe sesi seç
    for voice in voices:
        if "turkish" in voice.name.lower() or "turk" in voice.id.lower():
            engine.setProperty('voice', voice.id)
            break

    # Konuşma hızını düşür (varsayılan 200 civarıdır)
    engine.setProperty('rate', 200)  # Daha yavaş okuma için düşürdük

    # Metni seslendir
    engine.say(text)
    engine.runAndWait()


if __name__ == "__main__":
    image_path = r"C:\Users\muham\Desktop\Kod\Python\sample_3.jpeg"  # Buraya kendi görselinizin yolunu yazın
    extracted_text = extract_text_from_image(image_path)
    
    print("Çıkarılan Metin:")
    print(extracted_text)
    
    engine = pyttsx3.init()
    engine.setProperty("rate", 125)  # Konuşma hızını ayarla
    engine.say("Merhaba! Ben Python ile konuşuyorum.")
    engine.runAndWait()

    text = extracted_text
    tts = gTTS(text=text, lang="tr")
    tts.save("output.mp3")  # MP3 olarak kaydedilir
    os.system("start output.mp3") 
    
    # if extracted_text.strip():  # Eğer metin boş değilse seslendir
    #     text_to_speech(extracted_text)
    # else:
    #     print("Görselde metin bulunamadı.")
