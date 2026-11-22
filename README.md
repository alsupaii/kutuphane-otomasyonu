# Kampüs Kütüphanesi ve Öğrenci Hizmetleri Otomasyonu

BLP 205 Veri Yapıları final projesi (2026-2027 Güz). Menüyle kullanılan C++11 konsol uygulaması.

## Amaç
Kitap kataloğunu, duyuru panosunu, işlem geçmişini, ödünç bekleme sırasını ve kampüs rotalarını tek uygulamada yönetmek.

## Kullanılan veri yapıları
| Modül | Dosya | Yapı | Kullanım |
|---|---|---|---|
| 1 | `modul1.h/.cpp` | Çift yönlü doğrusal bağlı liste + zincirlemeli hash indeksi, tek yönlü dairesel bağlı liste, dosya G/Ç | Kitap kataloğu, duyurular, `katalog.txt` (yazma, gösterme, kataloğa yükleme) |
| 2 | `modul2.h/.cpp` | Bağlı liste tabanlı Stack (iki adet), Queue, dizi tabanlı dairesel tampon | İşlem geçmişi, geri alma (undo), ödünç verme, iade, ödünç bekleme sırası, son 10 hareket |
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
Windows'ta `derle.bat` dosyasına çift tıklamak da yeterli. Bu betik `-static` ile derler: `kutuphane.exe` DLL istemez ve Gezgin'den de açılır.

**Uyarı (Windows):** `-static` olmadan derlenen exe, PATH'te bulduğu ilk `libstdc++-6.dll` dosyasını yükler. Git Bash'te bu Git'in kendi sürümüdür. Sürüm uyuşmazsa program yanlış çalışabilir. Bu projede dosya yokken açılmış gibi davrandığı görüldü. Gezgin'den açılırsa da DLL bulunamaz.

## Menü özeti
Ana menü → Modül 1–4 → her modülde alt menü → `0` ile ana menüye dönüş → ana menüde `0` ile çıkış.
Çıkışta `new` ile açılan tüm düğümler `delete` edilir ve her yapı için silinen düğüm sayısı yazılır.

**Bellek kanıtı:** Her düğüm türünün kurucusu `canliDugum` sayacını artırır, yıkıcısı azaltır. `new[]` dizileri `canliDizi` ile sayılır. Çıkışta iki sayaç da 0 olmalıdır; değilse program "SIZINTI VAR" yazar.

## Hash indeksi (katalogda ID ile arama)
Katalog iki yapıdan oluşur: sırayı ve gezinmeyi çift yönlü liste tutar, ID ile erişimi hash tablosu sağlar. Tablo 31 kovalıdır (asal); hash fonksiyonu `id % 31`. Aynı kovaya düşen kayıtlar o kovanın zincirinde durur, yeni kayıt zincirin başına girer. Kova, kitabın kopyasını değil katalog düğümünün adresini tutar.
- Ekleme, silme, temizleme ve dosyadan yükleme listeyi ve indeksi aynı fonksiyonda günceller.
- Silme sırası: önce indeksten çıkar, sonra listeden sök, en son `delete`. Ters sırada indekste sarkan işaretçi kalırdı.
- ID ile arama, silme, ödünç/iade ve tekrar ID kontrolü O(n) yerine ortalama O(1 + α) oldu (α = kayıt / kova).
- M1 → 15 aynı ID'yi iki yolla arar; M1 → 16 kova dağılımını ve 1000 kitaplık denemeyi gösterir (doğrusal ortalama 500,5, hash 16,6 karşılaştırma).

**Neden BST değil?** ID'ler artarak eklenir. Dengesiz BST'de her yeni ID en sağa gider ve ağaç bir listeye döner (O(n)). Aynı artan ID'ler `id % 31` ile kovalara eşit dağılır.

## Dairesel tampon (son 10 hareket)
Ödünç, sıraya ekleme, iade ve talep iptali hem geçmiş yığınına hem `HareketTamponu`'na yazılır. Tampon 10 hücreli sabit bir dizidir; `new`/`delete` kullanmaz. Yazma indisi `(bas + adet) % 10`'dur. Tampon doluysa yeni kayıt en eskinin üstüne yazılır ve `bas` bir ilerler. `adet` sayacı boş ile dolu tamponu ayırır; yalnız `bas` ile bu ayrım yapılamaz. M2 → 11 kayıtları eskiden yeniye, dizideki hücre numarasıyla gösterir.

**Neden yığın değil?** Yığın sınırsız büyür ve en yeniyi önce verir. Tampon sabit bellek kullanır, en eskiyi kendiliğinden düşürür ve kayıtları geliş sırasıyla verir. Mikrodenetleyicideki UART alım tamponu aynı yapıdır.

## Dosya biçimi
`katalog.txt`: her kitap bir satır, `id|ad|yazar|sayfa|oduncte|oduncSayisi` (örnek `101|Suc ve Ceza|Dostoyevski|687|1|4`). Eski 4 alanlı satırlar da okunur; bu kitaplar rafta ve sayaç 0 sayılır.
- **Göster (M1 → 12):** dosyayı okur, kataloğa dokunmaz.
- **Yükle (M1 → 14):** dosya önce geçici bir listeye okunur, sonra katalogla yer değiştirir. Yalnız `bas`, `son` ve `adet` takas edilir (O(1)); eski düğümleri geçici listenin yıkıcısı siler. Dosya açılamazsa katalog değişmez. Bozuk ya da tekrar eden ID'li satır atlanır. Ödünçte kitap varken yükleme yapılmaz. Yüklemeden sonra geri alma yığını boşalır.

## Quick Sort pivot seçimi
Lomuto bölümleme pivotu aralığın son elemanından alır. Dizi zaten sıralıysa pivot hep en büyük eleman olur ve sıralama O(n²)'ye düşer. Ortanca-üç sürümü ilk, orta ve son elemanın ortancasını pivot yapar. Modül 3 → 6 ikisini aynı girdide karşılaştırır: 1000 sıralı ID'de 499 500'e karşı 9 451 karşılaştırma. Küçük ve karışık dizide ortanca-üç birkaç karşılaştırma fazla yapar.

## Kampüs modeli
Yönergedeki 6 birimlik referans model kullanıldı: Merkez Kütüphane, MYO Bilgisayar Lab., Mühendislik Binası, Öğrenci İşleri, Yemekhane ve Sosyal Alan, Konferans Salonu. 9 yürüme yolu vardır. Mesafeler örnek değerdir (metre).

## Örnek kullanım
(Ekran görüntüleri eklenecek.)

## Testler
71 senaryo: `TEST_SENARYOLARI.md`. Hepsini yeniden kosmak icin `testler\kos.bat` (12 girdi dosyasi + 2 birim testi, ciktilar onayli ciktiyla karsilastirilir).

## Bilinen sınırlılıklar
- Kampüs grafı sabittir; menüden yol eklenip silinemez.
- Metin alanlarında `|` karakteri kullanılamaz, çünkü dosyada ayıraç olarak kullanılıyor.
- Kitabın hangi öğrencide olduğu tutulmaz, yalnız ödünçte olup olmadığı tutulur. Bu yüzden iadede kimin getirdiği sorulmaz.
- Bekleme kuyruğu ve işlem geçmişi dosyaya yazılmaz; yalnız katalog kaydedilir.
- Hash tablosunun kova sayısı sabittir (31); büyümez. Kitap sayısı çok artarsa zincirler uzar (1000 kitapta ortalama 16,6 adım).
- Geri alma yalnız kitap ekleme ve silmeyi kapsar. Ödünç, iade ve kuyruk işlemleri geri alınmaz. Katalog boşaltılınca geri alma yığını da boşalır.
- Konsol mesajları ASCII'dir (yönerge gereği Türkçe karakter yok).
