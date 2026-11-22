#ifndef MODUL1_H
#define MODUL1_H

// MODUL 1: Cift yonlu bagli liste (kitap katalogu),
// tek yonlu dairesel bagli liste (duyurular), dosya islemleri.

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
struct KitapDugum {
    Kitap veri;
    KitapDugum* prev;
    KitapDugum* next;
};

enum class EklemeSonucu { basarili, idZatenVar, hedefYok };
enum class SilmeSonucu { basarili, bulunamadi, oduncte };

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
    bool bosMu() const;
    int boyut() const;

    void ileriListele() const;
    void geriListele() const;
    void analizYazdir() const;
    void esikUstuListele(int esik) const;

    bool dosyayaYaz(const std::string& dosyaAdi) const;
    int diziyeAktar(Kitap dizi[], int kapasite) const;  // Modul 3 icin

private:
    KitapDugum* bas;
    KitapDugum* son;
    int adet;

    KitapDugum* dugumBul(int id) const;
};

// Tek yonlu dairesel liste dugumu: son dugumun next'i bastaki dugumu gosterir.
struct DuyuruDugum {
    std::string metin;
    DuyuruDugum* next;
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

void modul1Menu(KitapListesi& katalog, DuyuruListesi& duyurular, IslemYigini& gecmis);

#endif
