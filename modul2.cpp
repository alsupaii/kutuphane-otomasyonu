#include "modul2.h"
#include "modul1.h"
#include "ortak.h"

#include <iostream>

// ---------------------------------------------------------------
// IslemYigini: bagli liste tabanli yigin
// Butun islemler ust dugumde yapilir, bu yuzden push/pop/peek O(1).
// ---------------------------------------------------------------

IslemYigini::IslemYigini() : ust(nullptr), adet(0) {}

IslemYigini::~IslemYigini() { temizle(); }

bool IslemYigini::bosMu() const { return ust == nullptr; }

int IslemYigini::boyut() const { return adet; }

void IslemYigini::push(const std::string& aciklama) {
    YiginDugum* yeni = new YiginDugum{aciklama, ust};  // Yeni dugum eski ustu gosterir.
    ust = yeni;                                         // Ust artik yeni dugum.
    adet++;
}

bool IslemYigini::pop(std::string& cikan) {
    if (ust == nullptr) {
        return false;  // Bos yigindan cikarma yapilamaz (underflow).
    }
    YiginDugum* silinecek = ust;
    cikan = silinecek->aciklama;
    ust = ust->next;
    delete silinecek;
    adet--;
    return true;
}

bool IslemYigini::peek(std::string& ustteki) const {
    if (ust == nullptr) {
        return false;
    }
    ustteki = ust->aciklama;
    return true;
}

// Ustten alta dogru yazar: en yeni islem en ustte. O(n).
void IslemYigini::listele() const {
    if (ust == nullptr) {
        std::cout << "Islem gecmisi (yigin) bos.\n";
        return;
    }
    std::cout << "Islem gecmisi (ustten alta, en yeni en ustte):\n";
    int sira = 1;
    for (const YiginDugum* p = ust; p != nullptr; p = p->next) {
        std::cout << "  " << sira << ") " << p->aciklama << (sira == 1 ? "   <- ust" : "") << '\n';
        sira++;
    }
}

int IslemYigini::temizle() {
    int silinen = 0;
    while (ust != nullptr) {
        YiginDugum* sonraki = ust->next;
        delete ust;
        ust = sonraki;
        silinen++;
    }
    adet = 0;
    return silinen;
}

// ---------------------------------------------------------------
// BeklemeKuyrugu: bagli liste tabanli kuyruk
// Ekleme arkaya, cikarma ondan. Iki isaretci tutuldugu icin ikisi de O(1).
// ---------------------------------------------------------------

BeklemeKuyrugu::BeklemeKuyrugu() : on(nullptr), arka(nullptr), adet(0) {}

BeklemeKuyrugu::~BeklemeKuyrugu() { temizle(); }

bool BeklemeKuyrugu::bosMu() const { return on == nullptr; }

int BeklemeKuyrugu::boyut() const { return adet; }

void BeklemeKuyrugu::enqueue(const Ogrenci& o) {
    KuyrukDugum* yeni = new KuyrukDugum{o, nullptr};
    if (arka == nullptr) {     // Kuyruk bos: yeni dugum hem on hem arka.
        on = yeni;
        arka = yeni;
    } else {
        arka->next = yeni;     // Eski arka yeni dugumu gosterir.
        arka = yeni;
    }
    adet++;
}

bool BeklemeKuyrugu::dequeue(Ogrenci& cikan) {
    if (on == nullptr) {
        return false;
    }
    KuyrukDugum* silinecek = on;
    cikan = silinecek->veri;
    on = on->next;
    if (on == nullptr) {       // Son eleman cikti: arka da bosa dusmeli.
        arka = nullptr;
    }
    delete silinecek;
    adet--;
    return true;
}

// Ondan arkaya yazar: FIFO sirasi. O(n).
void BeklemeKuyrugu::listele() const {
    if (on == nullptr) {
        std::cout << "Bekleme kuyrugu bos.\n";
        return;
    }
    std::cout << "Bekleme sirasi (ilk gelen en onde):\n";
    int sira = 1;
    for (const KuyrukDugum* p = on; p != nullptr; p = p->next) {
        std::cout << "  " << sira << ") " << p->veri.ogrenciNo << " - " << p->veri.adSoyad
                  << " -> " << p->veri.kitapAdi << " (ID " << p->veri.kitapId << ")"
                  << (sira == 1 ? "   <- on" : "") << '\n';
        sira++;
    }
}

int BeklemeKuyrugu::temizle() {
    int silinen = 0;
    while (on != nullptr) {
        KuyrukDugum* sonraki = on->next;
        delete on;
        on = sonraki;
        silinen++;
    }
    arka = nullptr;
    adet = 0;
    return silinen;
}

// ---------------------------------------------------------------
// Menu
// ---------------------------------------------------------------

static bool sadeceRakamMi(const std::string& s) {
    for (std::string::size_type i = 0; i < s.size(); i++) {
        if (s[i] < '0' || s[i] > '9') {
            return false;
        }
    }
    return !s.empty();
}

void modul2Menu(IslemYigini& gecmis, BeklemeKuyrugu& kuyruk, const KitapListesi& katalog) {
    int secim = -1;
    while (secim != 0) {
        std::cout << "\n=== MODUL 2: ISLEM GECMISI (STACK) VE BEKLEME SIRASI (QUEUE) ===\n"
                  << "  1) Push: yigina islem kaydi ekle\n"
                  << "  2) Pop: ustteki kaydi cikar\n"
                  << "  3) Peek: ustteki kaydi goster\n"
                  << "  4) Yigini listele\n"
                  << "  5) Enqueue: bekleme sirasina ogrenci ekle\n"
                  << "  6) Dequeue: siradaki ogrenciye hizmet ver\n"
                  << "  7) Kuyrugu listele (FIFO)\n"
                  << "  0) Ana menuye don\n";
        secim = tamSayiOku("Seciminiz: ", 0, 7);

        switch (secim) {
        case 1:
            gecmis.push(metinOku("Islem aciklamasi: "));
            std::cout << "Kayit yigina eklendi. Yigin boyutu: " << gecmis.boyut() << '\n';
            break;
        case 2: {
            std::string cikan;
            if (gecmis.pop(cikan)) {
                std::cout << "Cikarilan kayit: " << cikan << '\n';
            } else {
                std::cout << "Yigin bos. Pop yapilamaz.\n";
            }
            break;
        }
        case 3: {
            std::string ustteki;
            if (gecmis.peek(ustteki)) {
                std::cout << "Ustteki kayit: " << ustteki << '\n';
            } else {
                std::cout << "Yigin bos. Ustte kayit yok.\n";
            }
            break;
        }
        case 4:
            gecmis.listele();
            break;
        case 5: {
            Ogrenci o;
            o.ogrenciNo = metinOku("Ogrenci numarasi: ");
            if (!sadeceRakamMi(o.ogrenciNo)) {
                std::cout << "Ogrenci numarasi yalnizca rakamlardan olusmali.\n";
                break;
            }
            o.adSoyad = metinOku("Ad soyad: ");
            o.kitapId = tamSayiOku("Talep edilen kitap ID: ", 1, 999999);
            const KitapDugum* kitap = katalog.bul(o.kitapId);
            if (kitap == nullptr) {
                std::cout << "Bu ID katalogda yok: " << o.kitapId << ". Ogrenci siraya eklenmedi.\n";
                break;
            }
            o.kitapAdi = kitap->veri.ad;
            kuyruk.enqueue(o);
            std::cout << o.adSoyad << " siraya eklendi. Siradaki yeri: " << kuyruk.boyut() << '\n';
            gecmis.push("Bekleme sirasina eklendi: " + o.ogrenciNo);
            break;
        }
        case 6: {
            Ogrenci o;
            if (kuyruk.dequeue(o)) {
                std::cout << "Hizmet verildi: " << o.ogrenciNo << " - " << o.adSoyad
                          << " -> " << o.kitapAdi << '\n';
                gecmis.push("Odunc verildi: " + o.ogrenciNo + " -> ID " + std::to_string(o.kitapId));
            } else {
                std::cout << "Kuyruk bos. Hizmet verilecek ogrenci yok.\n";
            }
            break;
        }
        case 7:
            kuyruk.listele();
            break;
        default:
            break;
        }
    }
}
