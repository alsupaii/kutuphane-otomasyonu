#ifndef MODUL2_H
#define MODUL2_H

// MODUL 2: Bagli liste tabanli yigin (Stack) ve kuyruk (Queue).

#include "modul1.h"  // Kitap: geri alma kaydi kitabin kopyasini tasir.

#include <string>

// ---------------- Yigin (LIFO: son giren ilk cikar) ----------------

// bilgi: yalniz metin (islem gecmisi). Digerleri geri alinabilir (geri alma yigini).
enum class IslemTuru { bilgi, kitapEklendi, kitapSilindi };

struct IslemKaydi {
    std::string aciklama;
    IslemTuru tur;
    Kitap kitap;         // Eklenen ya da silinen kitabin kopyasi
    int sonrakiKomsuId;  // Silinen kitabin arkasindaki kitabin ID'si; -1 = sondaydi
};

struct YiginDugum {
    IslemKaydi veri;
    YiginDugum* next;  // Bir alttaki kayit
    YiginDugum(const IslemKaydi& v, YiginDugum* n) : veri(v), next(n) { canliDugum++; }
    ~YiginDugum() { canliDugum--; }
    YiginDugum(const YiginDugum&) = delete;
    YiginDugum& operator=(const YiginDugum&) = delete;
};

class IslemYigini {
public:
    IslemYigini();
    ~IslemYigini();
    IslemYigini(const IslemYigini&) = delete;
    IslemYigini& operator=(const IslemYigini&) = delete;

    void push(const IslemKaydi& kayit);
    void push(const std::string& aciklama);  // Yalniz metinli bilgi kaydi
    bool pop(IslemKaydi& cikan);             // Bossa false
    bool peek(IslemKaydi& ustteki) const;
    void listele() const;
    bool bosMu() const;
    int boyut() const;
    int temizle();

private:
    YiginDugum* ust;
    int adet;
};

// ---------------- Kuyruk (FIFO: ilk giren ilk cikar) ----------------

struct Ogrenci {
    std::string ogrenciNo;
    std::string adSoyad;
    int kitapId;
    std::string kitapAdi;
};

struct KuyrukDugum {
    Ogrenci veri;
    KuyrukDugum* next;  // Arkadaki ogrenci
    KuyrukDugum(const Ogrenci& v, KuyrukDugum* n) : veri(v), next(n) { canliDugum++; }
    ~KuyrukDugum() { canliDugum--; }
    KuyrukDugum(const KuyrukDugum&) = delete;
    KuyrukDugum& operator=(const KuyrukDugum&) = delete;
};

class BeklemeKuyrugu {
public:
    BeklemeKuyrugu();
    ~BeklemeKuyrugu();
    BeklemeKuyrugu(const BeklemeKuyrugu&) = delete;
    BeklemeKuyrugu& operator=(const BeklemeKuyrugu&) = delete;

    void enqueue(const Ogrenci& o);
    bool dequeue(Ogrenci& cikan);       // Bossa false
    // kitapId'yi bekleyen ILK ogrenciyi cikarir; digerlerinin sirasi korunur.
    // Yalniz enqueue/dequeue kullanir. Bekleyen yoksa false.
    bool ilkBekleyeniCikar(int kitapId, Ogrenci& cikan);
    bool siradaMi(const std::string& ogrenciNo, int kitapId) const;
    void listele() const;
    bool bosMu() const;
    int boyut() const;
    int temizle();

private:
    KuyrukDugum* on;    // front: hizmet alacak ilk ogrenci
    KuyrukDugum* arka;  // rear: en son gelen ogrenci
    int adet;
};

// ---------------- Dairesel tampon (sabit kapasiteli dizi) ----------------
// Son 10 odunc/iade hareketini tutar. Dolunca yeni kayit EN ESKININ ustune yazilir.
// new/delete yok: bellek sabit, sizinti olamaz. UART alim tamponu ayni mantikla calisir.

const int hareketKapasitesi = 10;

class HareketTamponu {
public:
    HareketTamponu();
    void yaz(const std::string& hareket);  // O(1)
    void listele() const;                  // Eskiden yeniye, O(kapasite)
    int boyut() const;

private:
    std::string kayit[hareketKapasitesi];
    int bas;      // En eski kaydin indisi
    int adet;     // Dolu hucre sayisi (0..kapasite). Bos ve dolu tamponu bu ayirt eder.
    int toplam;   // Simdiye kadar yazilan hareket sayisi (uzerine yazilanlar dahil)
};

void modul2Menu(IslemYigini& gecmis, IslemYigini& geriAlma, HareketTamponu& hareketler,
                BeklemeKuyrugu& kuyruk, KitapListesi& katalog, DuyuruListesi& duyurular);

#endif
