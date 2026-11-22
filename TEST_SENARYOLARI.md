# Test Senaryolari

Derleme: `g++ -std=c++11 -Wall -Wextra -pedantic` (GCC 16.2), uyari yok.
Testler girdi dosyasi programa verilerek calistirildi. Ilk kosu: 22 Kasim 2026.

| # | Test | Girdi | Beklenen | Gerceklesen |
|---|---|---|---|---|
| 1 | Ileri ve geri listeleme | M1 → 1, 2 | Ayni 8 kayit, ters sirada | Dogru (105…103 / 103…105) |
| 2 | Basa ekleme | M1 → 3, ID 100 | 100 listenin basinda | Dogru |
| 3 | Sona ekleme | M1 → 4, ID 200 | 200 listenin sonunda | Dogru |
| 4 | ID oncesine ekleme | M1 → 5, hedef 101, yeni 150 | 150, 108 ile 101 arasinda | Dogru |
| 5 | Ilk dugumu silme | M1 → 6, ID 100 | Bas 105 olur | Dogru |
| 6 | Orta dugumu silme | M1 → 6, ID 101 | 150 ile 107 birbirine baglanir | Dogru (ileri ve geri listede) |
| 7 | Son dugumu silme | M1 → 6, ID 200 | Son 103 olur | Dogru |
| 8 | Bulunamayan ID silme | M1 → 6, ID 999 | "bulunamadi" mesaji | Dogru |
| 9 | Ayni ID ile ekleme | M1 → 3, var olan ID | "zaten kayitli" mesaji | Dogru |
| 10 | Katalog analizi | M1 → 7 | 8 kitap, 3393 sayfa, ort. 424.12, en az Kucuk Prens (112), en cok Sefiller (1463) | Dogru (elle hesapla ayni) |
| 11 | Sayfa esigi | M1 → 8, esik 400 | Ince Memed (436), Sefiller (1463) | Dogru |
| 12 | Dairesel duyuru turu | M1 → 9, 10 | 4 duyuru, sonra ilk duyuruya donus, sonsuz dongu yok | Dogru |
| 13 | Dosyaya yaz ve oku | M1 → 11, 12 | 8 kayit yazilir, 8 kayit okunup gosterilir | Dogru (`id\|ad\|yazar\|sayfa`) |
| 14 | Bos katalog | M1 → 13, sonra 1, 2, 7, 8, 6, 5; M3 → 2 | Her islemde "Katalog bos" mesaji, cokme yok | Dogru |
| 15 | Bos yigin | M2 → 2 (yigin bosken), 3 | "Yigin bos" mesaji | Dogru |
| 16 | Stack LIFO | M2 → 1, 4 | Son eklenen en ustte | Dogru |
| 17 | Queue FIFO | M2 → 5 (2026003), 6, 6, 6 | Sira: 2026001, 2026002, 2026003 | Dogru |
| 18 | Bos kuyruk | M2 → 6 (kuyruk bosken), 7 | "Kuyruk bos" mesaji | Dogru |
| 19 | Katalogda olmayan kitap talebi | M2 → 5, kitap ID 999 | Ogrenci siraya eklenmez | Dogru |
| 20 | Hatali ogrenci no | M2 → 5, "12ab" | "yalnizca rakam" mesaji | Dogru |
| 21 | Bubble Sort (sayfa) | M3 → 2 | 112 … 1463 artan; 27 karsilastirma, 14 takas | Dogru |
| 22 | Insertion Sort (sayfa) | M3 → 3 | 112 … 1463 artan; 19 karsilastirma, 14 kaydirma | Dogru |
| 23 | Quick Sort (ID) | M3 → 4 | 101 … 108 artan; 16 karsilastirma, 7 takas | Dogru |
| 24 | Binary Search, bulunan | M3 → 5, ID 106 | Bulundu, indis 5, 2 adim | Dogru |
| 25 | Binary Search, bulunamayan | M3 → 5, ID 999 | Bulunamadi, 4 adim | Dogru |
| 26 | Komsuluk matrisi | M4 → 2 | 6x6 simetrik tablo | Dogru |
| 27 | BFS (1'den) | M4 → 3, 1 | Kutuphane, MYO Lab, Muhendislik, Ogrenci Isleri, Yemekhane, Konferans | Dogru (elle ayni) |
| 28 | DFS (1'den) | M4 → 4, 1 | Kutuphane, MYO Lab, Muhendislik, Konferans, Ogrenci Isleri, Yemekhane | Dogru (elle ayni) |
| 29 | Dijkstra 1 → 6 | M4 → 5, 1, 6 | 520 m: Kutuphane → MYO Lab → Muhendislik → Konferans | Dogru (elle ayni) |
| 30 | Dijkstra ayni birim | M4 → 5, 4, 4 | 0 m | Dogru |
| 31 | Gecersiz menu secimi | "x", "9", "12abc", bos satir | Uyari ve yeniden sorma | Dogru |
| 32 | Gecersiz bina no | M4 → 5, "7" | "1 ile 6 arasinda" uyarisi | Dogru |
| 33 | Girdi yarida biterse | Dosya sonu (EOF) | Program bellegi temizleyip kapanir | Dogru |
| 34 | Cikista bellek temizligi | Ana menu → 0 | Her yapinin silinen dugum sayisi yazilir | Dogru |

Not: 21 ve 22'de takas ve kaydirma sayisi esit (14). Bu dogru: ikisi de dizideki
ters sirali cift sayisina esittir.
