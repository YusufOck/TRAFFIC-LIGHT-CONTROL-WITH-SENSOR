#include <IRremote.h> 
/* 
  IRremote kütüphanesi v4.x sürümü ile IR alıcıyı kontrol etmek
  için gerekli fonksiyonları içerir. 
  (IrReceiver.begin(), IrReceiver.decode() vb.)
*/

// ==========================
// 1) PIN ve IR KOMUT TANIMLARI
// ==========================

// IR alıcının bağlı olduğu pin (Arduino Uno’da A3 = dijital 17).
const int RECV_PIN = 17;

// “Trafik ışığı” senaryosunda kullandığımız dört yön ve
// her yönün kırmızı/sarı/yeşil LED pinleri:

// Kuzey yönü LED pinleri
const int PIN_NORTH_RED    = 2;  // Kuzey kırmızı LED
const int PIN_NORTH_YELLOW = 3;  // Kuzey sarı LED
const int PIN_NORTH_GREEN  = 4;  // Kuzey yeşil LED

// Güney yönü LED pinleri
const int PIN_SOUTH_RED    = 5;  // Güney kırmızı LED
const int PIN_SOUTH_YELLOW = 6;  // Güney sarı LED
const int PIN_SOUTH_GREEN  = 7;  // Güney yeşil LED

// Doğu (East) yönü LED pinleri
const int PIN_EAST_RED     = 8;   // Doğu kırmızı LED
const int PIN_EAST_YELLOW  = 9;   // Doğu sarı LED
const int PIN_EAST_GREEN   = 10;  // Doğu yeşil LED

// Batı (West) yönü LED pinleri
const int PIN_WEST_RED     = 11;  // Batı kırmızı LED
const int PIN_WEST_YELLOW  = 12;  // Batı sarı LED
const int PIN_WEST_GREEN   = 13;  // Batı yeşil LED

// Örnek NEC protokolünde her tuşa basıldığında gelen 32-bit kodlar:
// Bunlar, kumanda üzerinde “2, 4, 5, 6, 8” tuşlarına karşılık
// gelen kodlar olarak tanımlandı.
#define BUTTON_2 0xEE115000
#define BUTTON_4 0xED125000
#define BUTTON_5 0xEC135000
#define BUTTON_6 0xEB145000
#define BUTTON_8 0xEA155000


// ==========================
// 2) MODE ve STATE TANIMLARI
// ==========================

// “Mode” kavramı: Kod iki farklı durumda çalışıyor:
// - MODE_AUTO: Trafik ışıkları otomatik döngü içinde çalışır.
// - MODE_OVERRIDE: IR kumandadan gelen buton komutuna göre
//   belirli bir ışık anında değişir ve otomatik döngüyü durdurur.

enum Mode { MODE_AUTO, MODE_OVERRIDE };
Mode currentMode = MODE_AUTO;  
/* Başlangıçta otomatik modda (MODE_AUTO) çalışacak. */

// “AutoState” kavramı: Otomatik modda ışıkların geçiş
// sırasını belirleyen aşamalar (state’ler).
// Örneğin, önce Kuzey-Yeşil, sonra Kuzey-Sarı, sonra Güney-Yeşil,
// vb. sırasıyla değişiyor.

enum AutoState {
  STATE_N_GREEN,   // 0: Kuzey yönü yeşil
  STATE_N_YELLOW,  // 1: Kuzey yönü sarı
  STATE_S_GREEN,   // 2: Güney yönü yeşil
  STATE_S_YELLOW,  // 3: Güney yönü sarı
  STATE_E_GREEN,   // 4: Doğu yönü yeşil
  STATE_E_YELLOW,  // 5: Doğu yönü sarı
  STATE_W_GREEN,   // 6: Batı yönü yeşil
  STATE_W_YELLOW   // 7: Batı yönü sarı
};
AutoState currentState = STATE_N_GREEN;  
/* Başlangıç durumu: STATE_N_GREEN ⇒ “Kuzey-Yeşil” */

// Zamanlayıcı değişkeni: Hangi aşamada (state) ne kadar
// beklediğimizi kontrol etmek için kullanılıyor.
unsigned long stateStartTime = 0;

// Otomatik döngüde her bir yeşil ışığın ne kadar süre yanacağı (ms).
const unsigned long GREEN_TIME  = 6000UL; // 6000 ms = 6 saniye

// Otomatik döngüde her bir sarı ışığın ne kadar süre yanacağı (ms).
const unsigned long YELLOW_TIME = 3000UL; // 3000 ms = 3 saniye


// ==========================
// 3) YARDIMCI FONKSİYONLAR
// ==========================

// 3.1) setAllRed(): Tüm yönlerin ışıklarını “kırmızı” durumuna çevirir.
//      Yani önce her yönün kırmızısını HIGH, sarı ve yeşillerini LOW yapar.
void setAllRed() {
  // Kuzey yönü: sadece kırmızı yansın
  digitalWrite(PIN_NORTH_RED,    HIGH);
  digitalWrite(PIN_NORTH_YELLOW, LOW);
  digitalWrite(PIN_NORTH_GREEN,  LOW);

  // Güney yönü: sadece kırmızı yansın
  digitalWrite(PIN_SOUTH_RED,    HIGH);
  digitalWrite(PIN_SOUTH_YELLOW, LOW);
  digitalWrite(PIN_SOUTH_GREEN,  LOW);

  // Doğu yönü: sadece kırmızı yansın
  digitalWrite(PIN_EAST_RED,     HIGH);
  digitalWrite(PIN_EAST_YELLOW,  LOW);
  digitalWrite(PIN_EAST_GREEN,   LOW);

  // Batı yönü: sadece kırmızı yansın
  digitalWrite(PIN_WEST_RED,     HIGH);
  digitalWrite(PIN_WEST_YELLOW,  LOW);
  digitalWrite(PIN_WEST_GREEN,   LOW);
}


// 3.2) applyAutoState(): currentState değişkenine göre
// hangi yönün hangi renk ışığının yanması gerektiğini ayarlar.
// Örneğin STATE_N_GREEN ise Kuzey-Yeşil, diğer yönler zaten kırmızı.

void applyAutoState() {
  // Her durumda önce tüm ışıkları kırmızıya çeviriyoruz.
  setAllRed();

  // Hangi aşamadaysak (currentState), sadece ilgili
  // yönün yeşil veya sarı LED’ini yaktırıyoruz.
  switch (currentState) {
    case STATE_N_GREEN:
      // Kuzey kırmızı → LOW, Kuzey yeşil → HIGH
      digitalWrite(PIN_NORTH_RED,   LOW);
      digitalWrite(PIN_NORTH_GREEN, HIGH);
      break;

    case STATE_N_YELLOW:
      // Kuzey kırmızı → LOW, Kuzey sarı → HIGH
      digitalWrite(PIN_NORTH_RED,    LOW);
      digitalWrite(PIN_NORTH_YELLOW, HIGH);
      break;

    case STATE_S_GREEN:
      // Güney kırmızı → LOW, Güney yeşil → HIGH
      digitalWrite(PIN_SOUTH_RED,    LOW);
      digitalWrite(PIN_SOUTH_GREEN,  HIGH);
      break;

    case STATE_S_YELLOW:
      // Güney kırmızı → LOW, Güney sarı → HIGH
      digitalWrite(PIN_SOUTH_RED,    LOW);
      digitalWrite(PIN_SOUTH_YELLOW, HIGH);
      break;

    case STATE_E_GREEN:
      // Doğu kırmızı → LOW, Doğu yeşil → HIGH
      digitalWrite(PIN_EAST_RED,    LOW);
      digitalWrite(PIN_EAST_GREEN,  HIGH);
      break;

    case STATE_E_YELLOW:
      // Doğu kırmızı → LOW, Doğu sarı → HIGH
      digitalWrite(PIN_EAST_RED,     LOW);
      digitalWrite(PIN_EAST_YELLOW, HIGH);
      break;

    case STATE_W_GREEN:
      // Batı kırmızı → LOW, Batı yeşil → HIGH
      digitalWrite(PIN_WEST_RED,    LOW);
      digitalWrite(PIN_WEST_GREEN,  HIGH);
      break;

    case STATE_W_YELLOW:
      // Batı kırmızı → LOW, Batı sarı → HIGH
      digitalWrite(PIN_WEST_RED,     LOW);
      digitalWrite(PIN_WEST_YELLOW, HIGH);
      break;
  }
}


// 3.3) updateAutoState(): Otomatik moddaysak (MODE_AUTO),
// currentState’de ne kadar zaman geçtiğini (millis()-stateStartTime)
// kontrol eder. Eğer geçme süresi geçildiyse, bir sonraki aşamaya
// (state) geçer ve applyAutoState() ile LED’leri günceller.

void updateAutoState() {
  unsigned long now     = millis();            // Şu anki zaman
  unsigned long elapsed = now - stateStartTime; // Geçen süre

  switch (currentState) {
    case STATE_N_GREEN:
      // 6 saniye tamamlandıysa Kuzey-Sarı aşamasına geç
      if (elapsed >= GREEN_TIME) {
        currentState   = STATE_N_YELLOW;
        stateStartTime = now;   // Zamanlayıcıyı yeniden başlat
        applyAutoState();       // Işıkları güncelle
      }
      break;

    case STATE_N_YELLOW:
      // 4 saniye tamamlandıysa Güney-Yeşil aşamasına geç
      if (elapsed >= YELLOW_TIME) {
        currentState   = STATE_S_GREEN;
        stateStartTime = now;
        applyAutoState();
      }
      break;

    case STATE_S_GREEN:
      // 6 saniye tamamlandıysa Güney-Sarı aşamasına geç
      if (elapsed >= GREEN_TIME) {
        currentState   = STATE_S_YELLOW;
        stateStartTime = now;
        applyAutoState();
      }
      break;

    case STATE_S_YELLOW:
      // 4 saniye tamamlandıysa Doğu-Yeşil aşamasına geç
      if (elapsed >= YELLOW_TIME) {
        currentState   = STATE_E_GREEN;
        stateStartTime = now;
        applyAutoState();
      }
      break;

    case STATE_E_GREEN:
      // 6 saniye tamamlandıysa Doğu-Sarı aşamasına geç
      if (elapsed >= GREEN_TIME) {
        currentState   = STATE_E_YELLOW;
        stateStartTime = now;
        applyAutoState();
      }
      break;

    case STATE_E_YELLOW:
      // 4 saniye tamamlandıysa Batı-Yeşil aşamasına geç
      if (elapsed >= YELLOW_TIME) {
        currentState   = STATE_W_GREEN;
        stateStartTime = now;
        applyAutoState();
      }
      break;

    case STATE_W_GREEN:
      // 6 saniye tamamlandıysa Batı-Sarı aşamasına geç
      if (elapsed >= GREEN_TIME) {
        currentState   = STATE_W_YELLOW;
        stateStartTime = now;
        applyAutoState();
      }
      break;

    case STATE_W_YELLOW:
      // 4 saniye tamamlandıysa tekrar Kuzey-Yeşil aşamasına dön
      if (elapsed >= YELLOW_TIME) {
        currentState   = STATE_N_GREEN;
        stateStartTime = now;
        applyAutoState();
      }
      break;
  }
}


// ==========================
// 4) SETUP()
// ==========================

void setup() {
  // 4.1) Seri portu 115200 baud hızında başlat (hata ayıklama için).
  Serial.begin(115200);

  // 4.2) IRReceiver.begin(...): IR alıcıyı başlatır.
  //      DISABLE_LED_FEEDBACK: IRremote kütüphanesinin dahili
  //      LED’ini kapatır; modül üzerindeki LED’i yakmaz.
  IrReceiver.begin(RECV_PIN, DISABLE_LED_FEEDBACK);

  // 4.3) Tüm trafik ışığı pinlerini “output” olarak ayarla
  pinMode(PIN_NORTH_RED,    OUTPUT);
  pinMode(PIN_NORTH_YELLOW, OUTPUT);
  pinMode(PIN_NORTH_GREEN,  OUTPUT);

  pinMode(PIN_SOUTH_RED,    OUTPUT);
  pinMode(PIN_SOUTH_YELLOW, OUTPUT);
  pinMode(PIN_SOUTH_GREEN,  OUTPUT);

  pinMode(PIN_EAST_RED,     OUTPUT);
  pinMode(PIN_EAST_YELLOW,  OUTPUT);
  pinMode(PIN_EAST_GREEN,   OUTPUT);

  pinMode(PIN_WEST_RED,     OUTPUT);
  pinMode(PIN_WEST_YELLOW,  OUTPUT);
  pinMode(PIN_WEST_GREEN,   OUTPUT);

  // 4.4) İlk durumda, tüm ışıkları kırmızı yap ve
  // Kuzey-Yeşil (STATE_N_GREEN) aşamasını başlat.
  currentMode    = MODE_AUTO;        // Önce otomatik modda
  currentState   = STATE_N_GREEN;    // Başlangıç state’i
  stateStartTime = millis();         // Zamanlayıcıyı başlat
  applyAutoState();                  // LED’leri ilk konuma getir
}



// ==========================
// 5) LOOP()
// ==========================

void loop() {
  // 5.1) Öncelikle IRReceiver.decode() ile IR paketi var mı diye kontrol et.
  //      Eğer True dönerse, yeni bir IR paketi gelmiş demektir.
  if (IrReceiver.decode()) {
    // Gelen paketin 32-bit ham verisini oku:
    unsigned long irCode = IrReceiver.decodedIRData.decodedRawData;

    // 5.2) IR koduna göre “override” durumuna geç veya otomatik moda dön:

    if (irCode == BUTTON_2) {
      // “2” tuşuna basıldıysa:
      Serial.println(F("BUTTON_2"));  // Debug çıktısı

      currentMode = MODE_OVERRIDE;    // Override moduna geç
      setAllRed();                    // Önce tüm ışıkları kırmızı yap
      // Sonra sadece Kuzey-Yeşil yansın (örnek durum)
      digitalWrite(PIN_NORTH_RED,   LOW);
      digitalWrite(PIN_NORTH_GREEN, HIGH);
    }
    else if (irCode == BUTTON_4) {
      // “4” tuşuna basıldıysa:
      Serial.println(F("BUTTON_4"));

      currentMode = MODE_OVERRIDE;    // Override moduna geç
      setAllRed();                    // Tüm ışıkları kırmızı yap
      // Sadece Batı-Yeşil yansın (örnek durum)
      digitalWrite(PIN_WEST_RED,   LOW);
      digitalWrite(PIN_WEST_GREEN, HIGH);
    }
    else if (irCode == BUTTON_5) {
      // “5” tuşuna basıldıysa:
      Serial.println(F("BUTTON_5"));
      setAllRed();
      delay(2000);
      // Tekrar otomatik moda dön:
      currentMode    = MODE_AUTO;
      currentState   = STATE_N_GREEN;   // Yeniden Kuzey-Yeşil’den başla
      stateStartTime = millis();         // Zamanlayıcıyı sıfırla
      applyAutoState();                  // LED’leri ayarla
    }
    else if (irCode == BUTTON_6) {
      // “6” tuşuna basıldıysa:
      Serial.println(F("BUTTON_6"));

      currentMode = MODE_OVERRIDE;
      setAllRed();
      // Sadece Doğu-Yeşil yansın (örnek durum)
      digitalWrite(PIN_EAST_RED,   LOW);
      digitalWrite(PIN_EAST_GREEN, HIGH);
    }
    else if (irCode == BUTTON_8) {
      // “8” tuşuna basıldıysa:
      Serial.println(F("BUTTON_8"));

      currentMode = MODE_OVERRIDE;
      setAllRed();
      // Sadece Güney-Yeşil yansın (örnek durum)
      digitalWrite(PIN_SOUTH_RED,   LOW);
      digitalWrite(PIN_SOUTH_GREEN, HIGH);
    }
    else {
      // Hiçbir önceden tanımlı BUTON_2/4/5/6/8 ile eşleşmiyorsa:
      // Ham 32-bit kodu HEX olarak ekrana yazdır (debug amacıyla).
      Serial.print(F("Alinan IR Kodu (HAM): 0x"));
      Serial.println(irCode, HEX);
    }

    // 5.3) Yeni bir IR paketi okumak için hazır ol
    IrReceiver.resume();
  }

  // 5.4) Eğer hâlâ otomatik moddaysak, trafik ışığı state-machine’i çalıştır
  if (currentMode == MODE_AUTO) {
    updateAutoState();  
  }
  // Eğer MODE_OVERRIDE ise buraya girmez; override durumunda ışıklar sabit kalır
  // ve bir sonraki IR kodu gelene kadar değişmezler.
}
