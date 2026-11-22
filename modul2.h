#ifndef MODUL2_H
#define MODUL2_H

// MODUL 2: Bagli liste tabanli yigin (Stack) ve kuyruk (Queue).

#include <string>

class KitapListesi;  // Modul 1'de tanimli. Kuyruk, talep edilen kitabi katalogda arar.

// ---------------- Yigin (LIFO: son giren ilk cikar) ----------------

struct YiginDugum {
    std::string aciklama;
    YiginDugum* next;  // Bir alttaki kayit
};

class IslemYigini {
public:
    IslemYigini();
    ~IslemYigini();
    IslemYigini(const IslemYigini&) = delete;
    IslemYigini& operator=(const IslemYigini&) = delete;

    void push(const std::string& aciklama);
    bool pop(std::string& cikan);       // Bossa false
    bool peek(std::string& ustteki) const;
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
};

class BeklemeKuyrugu {
public:
    BeklemeKuyrugu();
    ~BeklemeKuyrugu();
    BeklemeKuyrugu(const BeklemeKuyrugu&) = delete;
    BeklemeKuyrugu& operator=(const BeklemeKuyrugu&) = delete;

    void enqueue(const Ogrenci& o);
    bool dequeue(Ogrenci& cikan);       // Bossa false
    void listele() const;
    bool bosMu() const;
    int boyut() const;
    int temizle();

private:
    KuyrukDugum* on;    // front: hizmet alacak ilk ogrenci
    KuyrukDugum* arka;  // rear: en son gelen ogrenci
    int adet;
};

void modul2Menu(IslemYigini& gecmis, BeklemeKuyrugu& kuyruk, const KitapListesi& katalog);

#endif
