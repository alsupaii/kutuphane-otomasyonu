# Kampüs Kütüphanesi ve Öğrenci Hizmetleri Otomasyonu

BLP 205 Veri Yapıları final projesi (2026-2027 Güz). Menüyle kullanılan C++11 konsol uygulaması.

## Amaç
Kitap kataloğunu, duyuru panosunu, işlem geçmişini, ödünç bekleme sırasını ve kampüs rotalarını tek uygulamada yönetmek.

## Kullanılan veri yapıları
| Modül | Dosya | Yapı | Kullanım |
|---|---|---|---|
| 1 | `modul1.h/.cpp` | Çift yönlü doğrusal bağlı liste, tek yönlü dairesel bağlı liste, dosya G/Ç | Kitap kataloğu, duyurular, `katalog.txt` |
| 2 | `modul2.h/.cpp` | Bağlı liste tabanlı Stack ve Queue | İşlem geçmişi, ödünç verme, iade, ödünç bekleme sırası |
| 3 | `modul3.h/.cpp` | Dinamik dizi + Bubble, Insertion, Quick Sort, Binary Search | Sayfa/ID sıralama, ID arama |
| 4 | `modul4.h/.cpp` | Komşuluk matrisli ağırlıklı graf, BFS, DFS, Dijkstra | Kampüs rotaları |
| — | `ortak.h/.cpp` | Güvenli girdi okuma | Tüm menüler |

**Modüller arası bağlantı:**
- **Ödünç akışı dört yapıyı birlikte değiştirir:**
  - Ödünç talebinde kitap raftaysa katalogda "ödünçte" olur. Kitap ödünçteyse öğrenci kuyruğa girer.
  - İadede kitabı bekleyen ilk öğrenci kuyruktan çıkar ve kitap ona geçer. Duyuru listesine "Sıra size geldi" yazılır.
  - Her adım yığına kayıt yazar.
- Bekleyen öğrenciyi bulmak için kuyruk bir tam tur döndürülür: her öğrenci önden çıkar, arkaya geri girer. Yalnız Enqueue ve Dequeue kullanılır, FIFO sırası bozulmaz.
- Ödünçteki kitap silinemez. Ödünçte kitap varken katalog boşaltılamaz.
- Modül 1, 3 ve 4'teki işlemler Modül 2'nin yığınına kayıt yazar.
- Modül 3, Modül 1'in bağlı listesini diziye aktarır.

## Derleme ve çalıştırma
```
g++ -std=c++11 -Wall -Wextra main.cpp ortak.cpp modul1.cpp modul2.cpp modul3.cpp modul4.cpp -o kutuphane
./kutuphane
```
Windows'ta `derle.bat` dosyasına çift tıklamak da yeterli.

## Menü özeti
Ana menü → Modül 1–4 → her modülde alt menü → `0` ile ana menüye dönüş → ana menüde `0` ile çıkış.
Çıkışta `new` ile açılan tüm düğümler `delete` edilir ve her yapı için silinen düğüm sayısı yazılır.

## Kampüs modeli
Yönergedeki 6 birimlik referans model kullanıldı: Merkez Kütüphane, MYO Bilgisayar Lab., Mühendislik Binası, Öğrenci İşleri, Yemekhane ve Sosyal Alan, Konferans Salonu. 9 yürüme yolu vardır. Mesafeler örnek değerdir (metre).

## Örnek kullanım
(Ekran görüntüleri eklenecek.)

## Testler
44 senaryo: `TEST_SENARYOLARI.md`.

## Bilinen sınırlılıklar
- Kampüs grafı sabittir; menüden yol eklenip silinemez.
- Dosyadan okuma kayıtları ekranda gösterir, kataloğa geri yüklemez.
- Metin alanlarında `|` karakteri kullanılamaz, çünkü dosyada ayıraç olarak kullanılıyor.
- Kitabın hangi öğrencide olduğu tutulmaz, yalnız ödünçte olup olmadığı tutulur. Bu yüzden iadede kimin getirdiği sorulmaz.
- Dosyaya ödünç durumu yazılmaz; dosyada yalnız `id|ad|yazar|sayfa` vardır.
- Konsol mesajları ASCII'dir (yönerge gereği Türkçe karakter yok).
