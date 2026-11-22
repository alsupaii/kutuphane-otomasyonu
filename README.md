# Kampüs Kütüphanesi ve Öğrenci Hizmetleri Otomasyonu

BLP 205 Veri Yapıları final projesi (2026-2027 Güz). Menüyle kullanılan C++11 konsol uygulaması.

## Amaç
Kitap kataloğunu, duyuru panosunu, işlem geçmişini, ödünç bekleme sırasını ve kampüs rotalarını tek uygulamada yönetmek.

## Kullanılan veri yapıları
| Modül | Dosya | Yapı | Kullanım |
|---|---|---|---|
| 1 | `modul1.h/.cpp` | Çift yönlü doğrusal bağlı liste, tek yönlü dairesel bağlı liste, dosya G/Ç | Kitap kataloğu, duyurular, `katalog.txt` |
| 2 | `modul2.h/.cpp` | Bağlı liste tabanlı Stack (iki adet) ve Queue | İşlem geçmişi, geri alma (undo), ödünç verme, iade, ödünç bekleme sırası |
| 3 | `modul3.h/.cpp` | Dinamik dizi + Bubble, Insertion, Quick Sort (iki pivot), Binary Search | Sayfa/ID sıralama, ID arama, pivot karşılaştırması |
| 4 | `modul4.h/.cpp` | Komşuluk matrisli ağırlıklı graf, BFS, DFS, Dijkstra | Kampüs rotaları |
| — | `ortak.h/.cpp` | Güvenli girdi okuma | Tüm menüler |

**Modüller arası bağlantı:**
- **Ödünç akışı dört yapıyı birlikte değiştirir:**
  - Ödünç talebinde kitap raftaysa katalogda "ödünçte" olur. Kitap ödünçteyse öğrenci kuyruğa girer.
  - İadede kitabı bekleyen ilk öğrenci kuyruktan çıkar ve kitap ona geçer. Duyuru listesine "Sıra size geldi" yazılır.
  - Her adım yığına kayıt yazar.
- Bekleyen öğrenciyi bulmak için kuyruk bir tam tur döndürülür: her öğrenci önden çıkar, arkaya geri girer. Yalnız Enqueue ve Dequeue kullanılır, FIFO sırası bozulmaz.
- Ödünçteki kitap silinemez. Ödünçte kitap varken katalog boşaltılamaz.
- **Geri alma (undo):** Kitap ekleme ve silme ikinci bir yığına da yazılır. Kayıt, kitabın kopyasını ve silinen kitabın arkasındaki komşunun ID'sini taşır. "Geri al" üstteki kaydı tersine çevirir: eklenen kitabı siler, silinen kitabı eski yerine koyar. Ters işlem yığına yeni kayıt yazmaz.
  - Komşu da sonradan silindiyse sorun yok: onun kaydı yığında daha üstte durur, LIFO ile önce o geri gelir.
  - Ödünçteki kitabın eklenmesi geri alınmaz; kayıt yığında kalır.
- Modül 1, 3 ve 4'teki işlemler Modül 2'nin geçmiş yığınına kayıt yazar.
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

**Bellek kanıtı:** Her düğüm türünün kurucusu `canliDugum` sayacını artırır, yıkıcısı azaltır. `new[]` dizileri `canliDizi` ile sayılır. Çıkışta iki sayaç da 0 olmalıdır; değilse program "SIZINTI VAR" yazar.

## Quick Sort pivot seçimi
Lomuto bölümleme pivotu aralığın son elemanından alır. Dizi zaten sıralıysa pivot hep en büyük eleman olur ve sıralama O(n²)'ye düşer. Ortanca-üç sürümü ilk, orta ve son elemanın ortancasını pivot yapar. Modül 3 → 6 ikisini aynı girdide karşılaştırır: 1000 sıralı ID'de 499 500'e karşı 9 451 karşılaştırma. Küçük ve karışık dizide ortanca-üç birkaç karşılaştırma fazla yapar.

## Kampüs modeli
Yönergedeki 6 birimlik referans model kullanıldı: Merkez Kütüphane, MYO Bilgisayar Lab., Mühendislik Binası, Öğrenci İşleri, Yemekhane ve Sosyal Alan, Konferans Salonu. 9 yürüme yolu vardır. Mesafeler örnek değerdir (metre).

## Örnek kullanım
(Ekran görüntüleri eklenecek.)

## Testler
56 senaryo: `TEST_SENARYOLARI.md`.

## Bilinen sınırlılıklar
- Kampüs grafı sabittir; menüden yol eklenip silinemez.
- Dosyadan okuma kayıtları ekranda gösterir, kataloğa geri yüklemez.
- Metin alanlarında `|` karakteri kullanılamaz, çünkü dosyada ayıraç olarak kullanılıyor.
- Kitabın hangi öğrencide olduğu tutulmaz, yalnız ödünçte olup olmadığı tutulur. Bu yüzden iadede kimin getirdiği sorulmaz.
- Dosyaya ödünç durumu yazılmaz; dosyada yalnız `id|ad|yazar|sayfa` vardır.
- Geri alma yalnız kitap ekleme ve silmeyi kapsar. Ödünç, iade ve kuyruk işlemleri geri alınmaz. Katalog boşaltılınca geri alma yığını da boşalır.
- Konsol mesajları ASCII'dir (yönerge gereği Türkçe karakter yok).
