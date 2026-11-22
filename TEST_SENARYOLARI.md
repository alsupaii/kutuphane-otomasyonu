# Test Senaryolari

Derleme: `g++ -std=c++11 -Wall -Wextra -pedantic` (GCC 16.2), uyari yok.
Testler girdi dosyasi programa verilerek calistirildi. Ilk kosu: 22 Kasim 2026.
Odunc akisi (35-44) eklendi. 1-34 ayni gun yeniden kosuldu. 101 artik oduncte basladigi icin
6, 10, 11 ve 14 yeni duruma gore guncellendi; digerlerinin sonucu degismedi.
Geri alma (45-51) 23 Kasim'da eklendi; onceki testler yeniden kosuldu, sonuc degismedi.
Ortanca-uc pivot ve bellek sayaci (52-56) 23 Kasim'da eklendi; tum test dosyalari yeniden kosuldu.

| # | Test | Girdi | Beklenen | Gerceklesen |
|---|---|---|---|---|
| 1 | Ileri ve geri listeleme | M1 → 1, 2 | Ayni 8 kayit, ters sirada | Dogru (105…103 / 103…105) |
| 2 | Basa ekleme | M1 → 3, ID 100 | 100 listenin basinda | Dogru |
| 3 | Sona ekleme | M1 → 4, ID 200 | 200 listenin sonunda | Dogru |
| 4 | ID oncesine ekleme | M1 → 5, hedef 101, yeni 150 | 150, 108 ile 101 arasinda | Dogru |
| 5 | Ilk dugumu silme | M1 → 6, ID 100 | Bas 105 olur | Dogru |
| 6 | Orta dugumu silme | M1 → 6, ID 108 | 102 ile 150 birbirine baglanir | Dogru (ileri ve geri listede) |
| 7 | Son dugumu silme | M1 → 6, ID 200 | Son 103 olur | Dogru |
| 8 | Bulunamayan ID silme | M1 → 6, ID 999 | "bulunamadi" mesaji | Dogru |
| 9 | Ayni ID ile ekleme | M1 → 3, var olan ID | "zaten kayitli" mesaji | Dogru |
| 10 | Katalog analizi | M1 → 7 (150'nin sayfasi 300) | 8 kitap, 3728 sayfa, ort. 466.00, en az Kucuk Prens (112), en cok Sefiller (1463) | Dogru (elle hesapla ayni) |
| 11 | Sayfa esigi | M1 → 8, esik 400 | Ince Memed (436), Suc ve Ceza (687), Sefiller (1463) | Dogru |
| 12 | Dairesel duyuru turu | M1 → 9, 10 | 4 duyuru, sonra ilk duyuruya donus, sonsuz dongu yok | Dogru |
| 13 | Dosyaya yaz ve oku | M1 → 11, 12 | 8 kayit yazilir, 8 kayit okunup gosterilir | Dogru (`id\|ad\|yazar\|sayfa`) |
| 14 | Bos katalog | M2 → 6 ile 101 ve 104'u iki kez iade al; M1 → 13, sonra 1, 2, 7, 8, 6, 5; M3 → 2 | Her islemde "Katalog bos" mesaji, cokme yok | Dogru |
| 15 | Bos yigin | M2 → 2 (yigin bosken), 3 | "Yigin bos" mesaji | Dogru |
| 16 | Stack LIFO | M2 → 1, 4 | Son eklenen en ustte | Dogru |
| 17 | Queue FIFO | M2 → 7, 7 | Sira: 2026001, 2026002 | Dogru |
| 18 | Bos kuyruk | M2 → 7 (kuyruk bosken), 8 | "Kuyruk bos" mesaji | Dogru |
| 19 | Katalogda olmayan kitap talebi | M2 → 5, kitap ID 999 | "Talep alinmadi", kuyruk degismez | Dogru |
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
| 35 | Rafta kitap talebi | M2 → 5, ogr 2026010, ID 105 | Hemen verilir; 105 "oduncte", odunc sayisi 3 → 4 | Dogru |
| 36 | Oduncte kitap talebi | M2 → 5, ogr 2026011, ID 105 | Kuyrugun sonuna eklenir (yer 3) | Dogru |
| 37 | Ayni talep iki kez | M2 → 5, ayni ogrenci ve ID | "zaten sirada", kuyruk degismez | Dogru |
| 38 | Iade, bekleyen var (siranin ortasindan) | Kuyruk [101, 104, 105, 101]; M2 → 6, ID 101 | Ilk 101 bekleyeni (2026001) alir; kalan sira [104, 105, 101] korunur; duyuru eklenir | Dogru |
| 39 | Iade, bekleyen yok | M2 → 6, ID 105 (ikinci kez) | Kitap rafa doner | Dogru |
| 40 | Rafta kitabi iade | M2 → 6, ID 106 | "zaten rafta", degisiklik yok | Dogru |
| 41 | Olmayan kitabi iade | M2 → 6, ID 999 | "katalogda yok" | Dogru |
| 42 | Oduncteki kitabi silme | M1 → 6, ID 101 | "oduncte, silinemez" | Dogru |
| 43 | Oduncte kitap varken bosaltma | M1 → 13 | "bosaltilamaz" | Dogru |
| 44 | Duyuruya otomatik kayit | Test 38'den sonra M1 → 10 | "Sira size geldi: ..." satiri turda gorunur | Dogru |
| 45 | Undo: ekle, sil, geri al, geri al | M1 → 4 (ID 300), 6 (300); M2 → 9, 9 | Ilk geri al 300'u sona geri koyar, ikincisi 300'u siler; katalog eski hali | Dogru |
| 46 | Undo: komsusu da silinen kitap | M1 → 6 (105), 6 (102); M2 → 9, 9 | 102, 108'in onune; sonra 105, 102'nin onune doner; sira 105, 102, 108, ... | Dogru (ileri ve geri listede) |
| 47 | Undo: son dugum | M1 → 6 (103); M2 → 9 | 103 yine en sonda; geri liste 103 ile baslar | Dogru |
| 48 | Undo: bos geri alma yigini | M2 → 9 (yigin bosken) | "Geri alinacak islem yok" | Dogru |
| 49 | Undo: oduncteki kitabin eklenmesi | M1 → 4 (400); M2 → 5 (400), 9 | "oduncte, geri alinamaz"; kayit yiginda kalir (M2 → 10). Iade sonrasi 9 basarili | Dogru |
| 50 | Undo kaydi yeniden yazilmaz | Test 45 sonrasi M2 → 10 | Geri alma yigini bos; gecmiste "Geri alindi: ..." satirlari var | Dogru |
| 51 | Katalog bosaltilinca undo | Iadeler alinir; M1 → 6 (108), 13; M2 → 9 | "Geri alma yigini da bosaltildi (1 kayit)", sonra "islem yok" | Dogru |
| 52 | Pivot karsilastirmasi, sirali girdi | M3 → 6 | 1000 sirali ID: son eleman 499500 karsilastirma (= n(n-1)/2), ortanca-uc 9451 | Dogru |
| 53 | Pivot karsilastirmasi, kucuk karisik girdi | M3 → 6 | Katalog (n = 8): son eleman 16, ortanca-uc 23 karsilastirma. Kucuk karisik dizide ortanca-uc pahali | Dogru |
| 54 | Ortanca-uc dogrulugu | Ayri test programi: n = 0..200; rastgele, sirali, ters, cok tekrarli | 804 dizinin hepsi sirali ve son eleman surumuyle ayni | Dogru (hata 0) |
| 55 | Canli dugum sayaci | Her test dosyasinin sonunda cikis | "temizlikten once" > 0, "temizlikten sonra" 0, canli dizi 0 | Dogru (8 dosyada) |
| 56 | Sayacin sizinti yakaladigini kanitlama | Kopyada pop'taki `delete` kaldirildi; M2 → 2 | "Canli dugum: 1 -> SIZINTI VAR" | Dogru (asil kod degismedi) |

Not: 21 ve 22'de takas ve kaydirma sayisi esit (14). Bu dogru: ikisi de dizideki
ters sirali cift sayisina esittir.
