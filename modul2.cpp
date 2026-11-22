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

// Kuyrugu bir tam tur dondurur: her ogrenci onden cikar, arkaya geri girer.
// kitapId'yi bekleyen ILK ogrenci geri girmez. Tur bitince digerleri eski sirasindadir.
// Ornek: [A:101, B:104, C:101], kitap 101 -> A cikar, kuyruk [B:104, C:101]. O(n).
bool BeklemeKuyrugu::ilkBekleyeniCikar(int kitapId, Ogrenci& cikan) {
    const int n = adet;  // Tur uzunlugu dongu basinda sabitlenir.
    bool bulundu = false;
    for (int i = 0; i < n; i++) {
        Ogrenci o;
        dequeue(o);
        if (!bulundu && o.kitapId == kitapId) {
            cikan = o;
            bulundu = true;
        } else {
            enqueue(o);
        }
    }
    return bulundu;
}

// Ayni ogrenci ayni kitap icin iki kez siraya girmesin. O(n).
bool BeklemeKuyrugu::siradaMi(const std::string& ogrenciNo, int kitapId) const {
    for (const KuyrukDugum* p = on; p != nullptr; p = p->next) {
        if (p->veri.ogrenciNo == ogrenciNo && p->veri.kitapId == kitapId) {
            return true;
        }
    }
    return false;
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

// Odunc talebi. Kitap raftaysa hemen verilir; oduncteyse ogrenci siraya girer (enqueue).
// Dokundugu yapilar: katalog (durum), kuyruk, yigin.
static void oduncTalebi(IslemYigini& gecmis, BeklemeKuyrugu& kuyruk, KitapListesi& katalog) {
    Ogrenci o;
    o.ogrenciNo = metinOku("Ogrenci numarasi: ");
    if (!sadeceRakamMi(o.ogrenciNo)) {
        std::cout << "Ogrenci numarasi yalnizca rakamlardan olusmali.\n";
        return;
    }
    o.adSoyad = metinOku("Ad soyad: ");
    o.kitapId = tamSayiOku("Talep edilen kitap ID: ", 1, 999999);
    const KitapDugum* kitap = katalog.bul(o.kitapId);
    if (kitap == nullptr) {
        std::cout << "Bu ID katalogda yok: " << o.kitapId << ". Talep alinmadi.\n";
        return;
    }
    o.kitapAdi = kitap->veri.ad;
    const std::string idMetni = std::to_string(o.kitapId);

    if (!kitap->veri.oduncte) {
        katalog.durumDegistir(o.kitapId, true);
        std::cout << "Kitap rafta. Odunc verildi: " << o.kitapAdi << " -> " << o.adSoyad << '\n';
        gecmis.push("Odunc verildi: ID " + idMetni + " -> " + o.ogrenciNo);
    } else if (kuyruk.siradaMi(o.ogrenciNo, o.kitapId)) {
        std::cout << "Bu ogrenci bu kitap icin zaten sirada.\n";
    } else {
        kuyruk.enqueue(o);
        std::cout << "Kitap oduncte. " << o.adSoyad << " bekleme sirasina eklendi. Kuyruktaki yeri: "
                  << kuyruk.boyut() << '\n';
        gecmis.push("Bekleme sirasina eklendi: " + o.ogrenciNo + " -> ID " + idMetni);
    }
}

// Iade. Kitabi bekleyen varsa kitap rafa donmez, siradaki ogrenciye gecer ve duyuru yazilir.
// Dokundugu yapilar: katalog, kuyruk, yigin, duyuru listesi.
static void iadeAl(IslemYigini& gecmis, BeklemeKuyrugu& kuyruk, KitapListesi& katalog,
                   DuyuruListesi& duyurular) {
    const int id = tamSayiOku("Iade edilen kitap ID: ", 1, 999999);
    const KitapDugum* kitap = katalog.bul(id);
    if (kitap == nullptr) {
        std::cout << "Bu ID katalogda yok: " << id << '\n';
        return;
    }
    if (!kitap->veri.oduncte) {
        std::cout << "Bu kitap zaten rafta. Iade alinmadi.\n";
        return;
    }
    const std::string idMetni = std::to_string(id);
    Ogrenci siradaki;
    if (kuyruk.ilkBekleyeniCikar(id, siradaki)) {
        katalog.durumDegistir(id, true);  // Kitap rafa donmeden yeni ogrenciye gecer.
        std::cout << "Iade alindi. Kitap bekleyen ilk ogrenciye verildi: " << siradaki.ogrenciNo
                  << " - " << siradaki.adSoyad << '\n';
        duyurular.ekle("Sira size geldi: " + siradaki.adSoyad + " (" + siradaki.ogrenciNo +
                       "), kitap: " + siradaki.kitapAdi);
        gecmis.push("Iade alindi: ID " + idMetni + ", siradaki ogrenciye verildi: " +
                    siradaki.ogrenciNo);
    } else {
        katalog.durumDegistir(id, false);
        std::cout << "Iade alindi. Bekleyen yok, kitap rafa dondu: " << kitap->veri.ad << '\n';
        gecmis.push("Iade alindi: ID " + idMetni + ", rafa dondu");
    }
}

void modul2Menu(IslemYigini& gecmis, BeklemeKuyrugu& kuyruk, KitapListesi& katalog,
                DuyuruListesi& duyurular) {
    int secim = -1;
    while (secim != 0) {
        std::cout << "\n=== MODUL 2: ISLEM GECMISI (STACK) VE BEKLEME SIRASI (QUEUE) ===\n"
                  << "  1) Push: yigina islem kaydi ekle\n"
                  << "  2) Pop: ustteki kaydi cikar\n"
                  << "  3) Peek: ustteki kaydi goster\n"
                  << "  4) Yigini listele\n"
                  << "  5) Odunc talebi (rafta ise ver, oduncte ise siraya ekle: Enqueue)\n"
                  << "  6) Kitap iade al (bekleyen varsa siradakine ver)\n"
                  << "  7) Dequeue: en ondeki ogrenciyi siradan cikar (talep iptali)\n"
                  << "  8) Kuyrugu listele (FIFO)\n"
                  << "  0) Ana menuye don\n";
        secim = tamSayiOku("Seciminiz: ", 0, 8);

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
        case 5:
            oduncTalebi(gecmis, kuyruk, katalog);
            break;
        case 6:
            iadeAl(gecmis, kuyruk, katalog, duyurular);
            break;
        case 7: {
            Ogrenci o;
            if (kuyruk.dequeue(o)) {
                std::cout << "Siradan cikti (talep iptal): " << o.ogrenciNo << " - " << o.adSoyad
                          << " -> " << o.kitapAdi << '\n';
                gecmis.push("Talep iptal: " + o.ogrenciNo + " -> ID " + std::to_string(o.kitapId));
            } else {
                std::cout << "Kuyruk bos. Siradan cikacak ogrenci yok.\n";
            }
            break;
        }
        case 8:
            kuyruk.listele();
            break;
        default:
            break;
        }
    }
}
