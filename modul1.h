#ifndef MODUL1_H
#define MODUL1_H

// MODUL 1: Cift yonlu bagli liste (kitap katalogu),
// tek yonlu dairesel bagli liste (duyurular), dosya islemleri.

#include "ortak.h"  // canliDugum sayaci

#include <string>

class IslemYigini;  // Modul 2'de tanimli. Menu, yapilan islemleri bu yigina yazar.

// Bir kitabin verisi. Modul 3 ayni yapiyi dizide kullanir.
struct Kitap {
    int id;
    std::string ad;
    std::string yazar;
    int sayfa;
    bool oduncte;     // true: bir ogrencide, false: rafta
    int oduncSayisi;  // Kac kez odunc verildi (Modul 3 bu alana gore siralar)
};

// Cift yonlu bagli liste dugumu: hem onceki (prev) hem sonraki (next) dugumu bilir.
// Kurucu sayaci artirir, yikici azaltir. "new KitapDugum{k, nullptr, nullptr}" bu kurucuyu cagirir.
struct KitapDugum {
    Kitap veri;
    KitapDugum* prev;
    KitapDugum* next;
    KitapDugum(const Kitap& k, KitapDugum* p, KitapDugum* n) : veri(k), prev(p), next(n) { canliDugum++; }
    ~KitapDugum() { canliDugum--; }
    KitapDugum(const KitapDugum&) = delete;  // Dugum kopyalanmaz; sayac sasmasin.
    KitapDugum& operator=(const KitapDugum&) = delete;
};

// ---------------- Hash tablosu (zincirleme): ID -> katalog dugumu ----------------
// Kova sayisi asal. Hash fonksiyonu: id % kovaSayisi. Ayni kovaya dusenler o kovanin
// kucuk tek yonlu listesinde (zincir) durur. Kova kitabin KOPYASINI degil, katalog
// dugumunun ADRESINI tutar: veri tek yerde durur.

const int kovaSayisi = 31;

struct IndeksDugum {
    int id;
    KitapDugum* kitap;  // Katalogdaki dugum (sahibi katalog, indeks silmez)
    IndeksDugum* next;  // Ayni kovadaki bir sonraki kayit
    IndeksDugum(int i, KitapDugum* k, IndeksDugum* n) : id(i), kitap(k), next(n) { canliDugum++; }
    ~IndeksDugum() { canliDugum--; }
    IndeksDugum(const IndeksDugum&) = delete;
    IndeksDugum& operator=(const IndeksDugum&) = delete;
};

class IdIndeksi {
public:
    IdIndeksi();
    ~IdIndeksi();
    IdIndeksi(const IdIndeksi&) = delete;
    IdIndeksi& operator=(const IdIndeksi&) = delete;

    void ekle(int id, KitapDugum* kitap);           // Zincirin basina, O(1)
    KitapDugum* bul(int id, int& adim) const;       // Ortalama O(1 + alfa)
    bool sil(int id);                               // Ortalama O(1 + alfa)
    int temizle();                                  // Indeks dugumlerini siler; kitaplara dokunmaz
    void takas(IdIndeksi& diger);                   // Iki indeksin kovalarini degistirir, O(kova)
    void dagilimYazdir() const;
    int boyut() const;

private:
    IndeksDugum* kova[kovaSayisi];
    int adet;
    // Sonuc her zaman 0..kovaSayisi-1. Negatif ID'de de dizi disina tasmaz
    // (C++'ta -5 % 31 = -5 olur; + kovaSayisi ile pozitife cekilir).
    static int kovaNo(int id) { return ((id % kovaSayisi) + kovaSayisi) % kovaSayisi; }
};

enum class EklemeSonucu { basarili, idZatenVar, hedefYok };
enum class SilmeSonucu { basarili, bulunamadi, oduncte };

// Katalog: cift yonlu liste (sira ve gezinme) + hash indeksi (ID ile hizli erisim).
// Iki yapi AYNI fonksiyonlarda guncellenir: listede olan her ID indekste de vardir.
class KitapListesi {
public:
    KitapListesi();
    ~KitapListesi();
    // Kopyalama kapali: iki nesne ayni dugumleri iki kez delete etmesin.
    KitapListesi(const KitapListesi&) = delete;
    KitapListesi& operator=(const KitapListesi&) = delete;

    EklemeSonucu basaEkle(const Kitap& k);
    EklemeSonucu sonaEkle(const Kitap& k);
    EklemeSonucu oncesineEkle(int hedefId, const Kitap& k);
    SilmeSonucu sil(int id);  // Oduncteki kitap silinmez.
    int temizle();  // Tum dugumleri siler, silinen dugum sayisini dondurur.

    // Odunc durumunu degistirir. oduncte = true ise oduncSayisi 1 artar.
    // Kitap yoksa false dondurur.
    bool durumDegistir(int id, bool oduncte);
    bool oduncteKitapVarMi() const;

    const KitapDugum* bul(int id) const;
    // Ayni ID'yi iki yolla arar, kac karsilastirma yapildigini dondurur (savunma/olcum icin).
    int dogrusalAramaAdimi(int id, bool& bulundu) const;  // Bastan sona, O(n)
    int hashAramaAdimi(int id, bool& bulundu) const;      // Kova zinciri, O(1 + alfa)
    void indeksDagilimiYazdir() const;
    // Katalog bastan degisince (temizle, dosyadan yukleme) 1 artar. Geri alma kaydi bu
    // numarayi tasir; numara tutmazsa kayit uygulanmaz (eski komsu ID'leri gecersizdir).
    int surum() const;
    bool bosMu() const;
    int boyut() const;

    void ileriListele() const;
    void geriListele() const;
    void analizYazdir() const;
    void esikUstuListele(int esik) const;

    bool dosyayaYaz(const std::string& dosyaAdi) const;
    // Katalogu dosyadakiyle degistirir. Yuklenen kitap sayisi; dosya acilamazsa -1;
    // oduncte kitap varsa -2 (yukleme yapilmaz, katalog degismez).
    int dosyadanYukle(const std::string& dosyaAdi, int& atlanan);
    int diziyeAktar(Kitap dizi[], int kapasite) const;  // Modul 3 icin

private:
    KitapDugum* bas;
    KitapDugum* son;
    int adet;
    int surumNo;
    IdIndeksi indeks;

    KitapDugum* dugumBul(int id) const;  // Artik indeksten bulur
};

// Tek yonlu dairesel liste dugumu: son dugumun next'i bastaki dugumu gosterir.
struct DuyuruDugum {
    std::string metin;
    DuyuruDugum* next;
    DuyuruDugum(const std::string& m, DuyuruDugum* n) : metin(m), next(n) { canliDugum++; }
    ~DuyuruDugum() { canliDugum--; }
    DuyuruDugum(const DuyuruDugum&) = delete;
    DuyuruDugum& operator=(const DuyuruDugum&) = delete;
};

class DuyuruListesi {
public:
    DuyuruListesi();
    ~DuyuruListesi();
    DuyuruListesi(const DuyuruListesi&) = delete;
    DuyuruListesi& operator=(const DuyuruListesi&) = delete;

    void ekle(const std::string& metin);
    void turYazdir() const;
    int temizle();
    int boyut() const;

private:
    DuyuruDugum* bas;
    DuyuruDugum* son;
    int adet;
};

// Dosyayi satir satir okur, alanlari '|' ile ayirir ve ekrana yazar.
// Okunan kayit sayisini, dosya acilamazsa -1 dondurur.
int dosyadanOkuGoster(const std::string& dosyaAdi);

// gecmis: tum islemlerin metin kaydi. geriAlma: yalniz geri alinabilir kitap ekleme/silme.
void modul1Menu(KitapListesi& katalog, DuyuruListesi& duyurular, IslemYigini& gecmis,
                IslemYigini& geriAlma);

#endif
