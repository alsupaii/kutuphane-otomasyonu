#include "modul1.h"
#include "modul2.h"
#include "ortak.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

static const std::string katalogDosyasi = "katalog.txt";

// ---------------------------------------------------------------
// Yardimci yazdirma fonksiyonlari
// ---------------------------------------------------------------

// durumSutunu = false: dosyadan okunan kayitlar icin (dosyada durum alani yok).
static void tabloBasligiYaz(bool durumSutunu) {
    std::cout << std::left << std::setw(7) << "ID" << std::setw(32) << "Kitap adi"
              << std::setw(26) << "Yazar" << std::setw(7) << "Sayfa"
              << (durumSutunu ? "Durum (odunc sayisi)" : "") << '\n';
    std::cout << std::string(durumSutunu ? 92 : 72, '-') << '\n';
}

static void kitapSatiriYaz(const Kitap& k) {
    std::cout << std::left << std::setw(7) << k.id << std::setw(32) << k.ad
              << std::setw(26) << k.yazar << std::setw(7) << k.sayfa
              << (k.oduncte ? "oduncte" : "rafta") << " (" << k.oduncSayisi << ")\n";
}

// ---------------------------------------------------------------
// KitapListesi: cift yonlu dogrusal bagli liste
// ---------------------------------------------------------------

KitapListesi::KitapListesi() : bas(nullptr), son(nullptr), adet(0) {}

KitapListesi::~KitapListesi() { temizle(); }

// Bastan sona dolasarak ID arar. O(n).
KitapDugum* KitapListesi::dugumBul(int id) const {
    KitapDugum* p = bas;
    while (p != nullptr) {
        if (p->veri.id == id) {
            return p;
        }
        p = p->next;
    }
    return nullptr;
}

const KitapDugum* KitapListesi::bul(int id) const { return dugumBul(id); }

bool KitapListesi::bosMu() const { return bas == nullptr; }

int KitapListesi::boyut() const { return adet; }

// Basa ekleme. O(1) (ID kontrolu haric).
EklemeSonucu KitapListesi::basaEkle(const Kitap& k) {
    if (dugumBul(k.id) != nullptr) {
        return EklemeSonucu::idZatenVar;
    }
    KitapDugum* yeni = new KitapDugum{k, nullptr, nullptr};
    if (bas == nullptr) {         // Liste bos: yeni dugum hem bas hem son.
        bas = yeni;
        son = yeni;
    } else {
        yeni->next = bas;         // 1) yeni -> eski bas
        bas->prev = yeni;         // 2) eski bas <- yeni
        bas = yeni;               // 3) bas artik yeni dugum
    }
    adet++;
    return EklemeSonucu::basarili;
}

// Sona ekleme. son isaretcisi tutuldugu icin O(1) (ID kontrolu haric).
EklemeSonucu KitapListesi::sonaEkle(const Kitap& k) {
    if (dugumBul(k.id) != nullptr) {
        return EklemeSonucu::idZatenVar;
    }
    KitapDugum* yeni = new KitapDugum{k, nullptr, nullptr};
    if (son == nullptr) {
        bas = yeni;
        son = yeni;
    } else {
        yeni->prev = son;         // 1) eski son <- yeni
        son->next = yeni;         // 2) eski son -> yeni
        son = yeni;               // 3) son artik yeni dugum
    }
    adet++;
    return EklemeSonucu::basarili;
}

// hedefId'li kitabin hemen onune ekler. O(n), cunku hedef aranir.
EklemeSonucu KitapListesi::oncesineEkle(int hedefId, const Kitap& k) {
    KitapDugum* hedef = dugumBul(hedefId);
    if (hedef == nullptr) {
        return EklemeSonucu::hedefYok;
    }
    if (dugumBul(k.id) != nullptr) {
        return EklemeSonucu::idZatenVar;
    }
    if (hedef == bas) {           // Bastaki dugumun onu = basa ekleme.
        return basaEkle(k);
    }
    KitapDugum* yeni = new KitapDugum{k, nullptr, nullptr};
    yeni->prev = hedef->prev;     // 1) yeni, hedefin eski oncekisini gosterir
    yeni->next = hedef;           // 2) yeni -> hedef
    hedef->prev->next = yeni;     // 3) eski onceki -> yeni
    hedef->prev = yeni;           // 4) hedef <- yeni
    adet++;
    return EklemeSonucu::basarili;
}

// ID'ye gore silme. Bas, orta, son ve tek dugum durumlarini kapsar. O(n).
// Kural: oduncteki kitap silinmez. Yoksa bekleme kuyrugu olmayan bir kitabi bekler.
SilmeSonucu KitapListesi::sil(int id) {
    KitapDugum* hedef = dugumBul(id);
    if (hedef == nullptr) {
        return SilmeSonucu::bulunamadi;
    }
    if (hedef->veri.oduncte) {
        return SilmeSonucu::oduncte;
    }
    // Onceki dugum varsa onu hedefin sonrakine bagla; yoksa hedef bastir.
    if (hedef->prev != nullptr) {
        hedef->prev->next = hedef->next;
    } else {
        bas = hedef->next;
    }
    // Sonraki dugum varsa onu hedefin oncekine bagla; yoksa hedef sondur.
    if (hedef->next != nullptr) {
        hedef->next->prev = hedef->prev;
    } else {
        son = hedef->prev;
    }
    delete hedef;
    adet--;
    return SilmeSonucu::basarili;
}

// Kitabi bulur ve durumunu degistirir. O(n), cunku kitap aranir.
bool KitapListesi::durumDegistir(int id, bool oduncte) {
    KitapDugum* d = dugumBul(id);
    if (d == nullptr) {
        return false;
    }
    if (oduncte) {
        d->veri.oduncSayisi++;  // Her odunc verme bir kez sayilir.
    }
    d->veri.oduncte = oduncte;
    return true;
}

// En az bir kitap oduncteyse true. O(n).
bool KitapListesi::oduncteKitapVarMi() const {
    for (const KitapDugum* p = bas; p != nullptr; p = p->next) {
        if (p->veri.oduncte) {
            return true;
        }
    }
    return false;
}

// Tum dugumleri siler. O(n).
int KitapListesi::temizle() {
    int silinen = 0;
    while (bas != nullptr) {
        KitapDugum* sonraki = bas->next;  // Silmeden ONCE sonrakini sakla.
        delete bas;
        bas = sonraki;
        silinen++;
    }
    son = nullptr;
    adet = 0;
    return silinen;
}

// next baglantisiyla bastan sona. O(n).
void KitapListesi::ileriListele() const {
    if (bas == nullptr) {
        std::cout << "Katalog bos. Listelenecek kitap yok.\n";
        return;
    }
    tabloBasligiYaz(true);
    for (const KitapDugum* p = bas; p != nullptr; p = p->next) {
        kitapSatiriYaz(p->veri);
    }
    std::cout << "Toplam " << adet << " kitap (bastan sona).\n";
}

// prev baglantisiyla sondan basa. O(n).
void KitapListesi::geriListele() const {
    if (son == nullptr) {
        std::cout << "Katalog bos. Listelenecek kitap yok.\n";
        return;
    }
    tabloBasligiYaz(true);
    for (const KitapDugum* p = son; p != nullptr; p = p->prev) {
        kitapSatiriYaz(p->veri);
    }
    std::cout << "Toplam " << adet << " kitap (sondan basa).\n";
}

// Tek dolasimda toplam, ortalama, en az ve en cok sayfali kitap. O(n).
void KitapListesi::analizYazdir() const {
    if (bas == nullptr) {
        std::cout << "Katalog bos. Analiz yapilamaz.\n";
        return;
    }
    long long toplamSayfa = 0;
    const KitapDugum* enAz = bas;
    const KitapDugum* enCok = bas;
    for (const KitapDugum* p = bas; p != nullptr; p = p->next) {
        toplamSayfa += p->veri.sayfa;
        if (p->veri.sayfa < enAz->veri.sayfa) {
            enAz = p;
        }
        if (p->veri.sayfa > enCok->veri.sayfa) {
            enCok = p;
        }
    }
    const double ortalama = static_cast<double>(toplamSayfa) / adet;

    std::cout << "Toplam kitap sayisi : " << adet << '\n'
              << "Toplam sayfa sayisi : " << toplamSayfa << '\n'
              << "Ortalama sayfa      : " << std::fixed << std::setprecision(2) << ortalama << '\n'
              << "En az sayfali kitap : " << enAz->veri.ad << " (" << enAz->veri.sayfa << ")\n"
              << "En cok sayfali kitap: " << enCok->veri.ad << " (" << enCok->veri.sayfa << ")\n";
    std::cout.unsetf(std::ios::fixed);
    std::cout << std::setprecision(6);
}

// Sayfa sayisi esikten BUYUK olan kitaplar. O(n).
void KitapListesi::esikUstuListele(int esik) const {
    if (bas == nullptr) {
        std::cout << "Katalog bos.\n";
        return;
    }
    int bulunan = 0;
    for (const KitapDugum* p = bas; p != nullptr; p = p->next) {
        if (p->veri.sayfa > esik) {
            if (bulunan == 0) {
                tabloBasligiYaz(true);
            }
            kitapSatiriYaz(p->veri);
            bulunan++;
        }
    }
    if (bulunan == 0) {
        std::cout << esik << " sayfa esigini asan kitap yok.\n";
    } else {
        std::cout << esik << " sayfayi asan " << bulunan << " kitap bulundu.\n";
    }
}

// Her kitap bir satir: id|ad|yazar|sayfa
bool KitapListesi::dosyayaYaz(const std::string& dosyaAdi) const {
    std::ofstream dosya(dosyaAdi);
    if (!dosya) {
        return false;
    }
    for (const KitapDugum* p = bas; p != nullptr; p = p->next) {
        dosya << p->veri.id << '|' << p->veri.ad << '|' << p->veri.yazar << '|'
              << p->veri.sayfa << '\n';
    }
    return static_cast<bool>(dosya);
}

// Listeyi diziye kopyalar. Kopyalanan kayit sayisini dondurur. O(n).
int KitapListesi::diziyeAktar(Kitap dizi[], int kapasite) const {
    int i = 0;
    for (const KitapDugum* p = bas; p != nullptr && i < kapasite; p = p->next) {
        dizi[i] = p->veri;
        i++;
    }
    return i;
}

// ---------------------------------------------------------------
// DuyuruListesi: tek yonlu dairesel bagli liste
// ---------------------------------------------------------------

DuyuruListesi::DuyuruListesi() : bas(nullptr), son(nullptr), adet(0) {}

DuyuruListesi::~DuyuruListesi() { temizle(); }

int DuyuruListesi::boyut() const { return adet; }

// Sona ekler ve halkayi kapali tutar: son->next her zaman bas. O(1).
void DuyuruListesi::ekle(const std::string& metin) {
    DuyuruDugum* yeni = new DuyuruDugum{metin, nullptr};
    if (bas == nullptr) {
        bas = yeni;
        son = yeni;
        yeni->next = yeni;        // Tek dugum kendini gosterir.
    } else {
        son->next = yeni;
        yeni->next = bas;         // Halkayi basa geri bagla.
        son = yeni;
    }
    adet++;
}

// Bastan baslar, basa geri gelince durur. do-while: bas en az bir kez yazilir.
void DuyuruListesi::turYazdir() const {
    if (bas == nullptr) {
        std::cout << "Duyuru yok.\n";
        return;
    }
    const DuyuruDugum* p = bas;
    int sira = 1;
    do {
        std::cout << "  " << sira << ") " << p->metin << '\n';
        p = p->next;
        sira++;
    } while (p != bas);
    std::cout << "Bir tam tur tamamlandi, baslangica donuldu: \"" << p->metin << "\"\n";
}

// Once halkayi kirar (son->next = nullptr), sonra dogrusal liste gibi siler.
// Halka kirilmazsa dongu hic bitmez.
int DuyuruListesi::temizle() {
    if (bas == nullptr) {
        return 0;
    }
    son->next = nullptr;
    int silinen = 0;
    while (bas != nullptr) {
        DuyuruDugum* sonraki = bas->next;
        delete bas;
        bas = sonraki;
        silinen++;
    }
    son = nullptr;
    adet = 0;
    return silinen;
}

// ---------------------------------------------------------------
// Dosyadan okuma
// ---------------------------------------------------------------

int dosyadanOkuGoster(const std::string& dosyaAdi) {
    std::ifstream dosya(dosyaAdi);
    if (!dosya) {
        return -1;
    }
    std::string satir;
    int kayitSayisi = 0;
    bool baslikYazildi = false;
    while (std::getline(dosya, satir)) {
        if (satir.empty()) {
            continue;
        }
        std::istringstream akis(satir);
        std::string alanlar[4];
        int alanSayisi = 0;
        std::string alan;
        while (alanSayisi < 4 && std::getline(akis, alan, '|')) {
            alanlar[alanSayisi] = alan;
            alanSayisi++;
        }
        kayitSayisi++;
        if (alanSayisi != 4) {
            std::cout << "  Bozuk satir atlandi: " << satir << '\n';
            continue;
        }
        if (!baslikYazildi) {
            tabloBasligiYaz(false);
            baslikYazildi = true;
        }
        std::cout << std::left << std::setw(7) << alanlar[0] << std::setw(32) << alanlar[1]
                  << std::setw(26) << alanlar[2] << alanlar[3] << '\n';
    }
    return kayitSayisi;
}

// ---------------------------------------------------------------
// Menu
// ---------------------------------------------------------------

static Kitap kitapBilgisiOku(int id) {
    Kitap k;
    k.id = id;
    k.ad = metinOku("Kitap adi: ");
    k.yazar = metinOku("Yazar adi: ");
    k.sayfa = tamSayiOku("Sayfa sayisi (1-100000): ", 1, 100000);
    k.oduncte = false;  // Yeni kitap rafta baslar.
    k.oduncSayisi = 0;
    return k;
}

// Basarili eklemede iki yigina yazar: gecmis (metin) ve geriAlma (tersine cevirme verisi).
static void eklemeSonucuYaz(EklemeSonucu sonuc, const Kitap& k, const std::string& yer,
                            IslemYigini& gecmis, IslemYigini& geriAlma) {
    if (sonuc == EklemeSonucu::basarili) {
        const std::string aciklama = "Kitap eklendi (" + yer + "): ID " + std::to_string(k.id);
        std::cout << "Kitap eklendi (" << yer << "): " << k.ad << '\n';
        gecmis.push(aciklama);
        geriAlma.push(IslemKaydi{aciklama, IslemTuru::kitapEklendi, k, -1});
    } else if (sonuc == EklemeSonucu::idZatenVar) {
        std::cout << "Bu ID zaten kayitli: " << k.id << '\n';
    } else {
        std::cout << "Hedef ID bulunamadi.\n";
    }
}

void modul1Menu(KitapListesi& katalog, DuyuruListesi& duyurular, IslemYigini& gecmis,
                IslemYigini& geriAlma) {
    int secim = -1;
    while (secim != 0) {
        std::cout << "\n=== MODUL 1: KATALOG VE DUYURULAR ===\n"
                  << "  1) Kitaplari listele (ileri, next)\n"
                  << "  2) Kitaplari listele (geri, prev)\n"
                  << "  3) Basa kitap ekle\n"
                  << "  4) Sona kitap ekle\n"
                  << "  5) Bir ID'nin onune kitap ekle\n"
                  << "  6) ID ile kitap sil\n"
                  << "  7) Katalog analizi\n"
                  << "  8) Sayfa esigini asan kitaplar\n"
                  << "  9) Duyuru ekle\n"
                  << " 10) Duyurulari bir tam tur yazdir\n"
                  << " 11) Katalogu dosyaya yaz (" << katalogDosyasi << ")\n"
                  << " 12) Dosyadan oku ve goster\n"
                  << " 13) Katalogu bosalt (bos liste testi)\n"
                  << "  0) Ana menuye don\n";
        secim = tamSayiOku("Seciminiz: ", 0, 13);

        switch (secim) {
        case 1:
            katalog.ileriListele();
            break;
        case 2:
            katalog.geriListele();
            break;
        case 3:
        case 4: {
            const int id = tamSayiOku("Kitap ID (1-999999): ", 1, 999999);
            if (katalog.bul(id) != nullptr) {
                std::cout << "Bu ID zaten kayitli: " << id << '\n';
                break;
            }
            const Kitap k = kitapBilgisiOku(id);
            if (secim == 3) {
                eklemeSonucuYaz(katalog.basaEkle(k), k, "basa", gecmis, geriAlma);
            } else {
                eklemeSonucuYaz(katalog.sonaEkle(k), k, "sona", gecmis, geriAlma);
            }
            break;
        }
        case 5: {
            if (katalog.bosMu()) {
                std::cout << "Katalog bos. Onune eklenecek kitap yok.\n";
                break;
            }
            const int hedefId = tamSayiOku("Hangi ID'nin onune eklensin: ", 1, 999999);
            if (katalog.bul(hedefId) == nullptr) {
                std::cout << "Hedef ID bulunamadi: " << hedefId << '\n';
                break;
            }
            const int id = tamSayiOku("Yeni kitap ID (1-999999): ", 1, 999999);
            if (katalog.bul(id) != nullptr) {
                std::cout << "Bu ID zaten kayitli: " << id << '\n';
                break;
            }
            const Kitap k = kitapBilgisiOku(id);
            eklemeSonucuYaz(katalog.oncesineEkle(hedefId, k), k,
                            "ID " + std::to_string(hedefId) + " onune", gecmis, geriAlma);
            break;
        }
        case 6: {
            if (katalog.bosMu()) {
                std::cout << "Katalog bos. Silinecek kitap yok.\n";
                break;
            }
            const int id = tamSayiOku("Silinecek kitap ID: ", 1, 999999);
            // Geri alma icin silmeden ONCE kitabin kopyasi ve arka komsusunun ID'si saklanir.
            // Silmeden sonra dugum yok; bu bilgiye artik ulasilamaz.
            const KitapDugum* d = katalog.bul(id);
            Kitap kopya{0, "", "", 0, false, 0};
            int sonrakiId = -1;
            if (d != nullptr) {
                kopya = d->veri;
                sonrakiId = (d->next != nullptr) ? d->next->veri.id : -1;
            }
            const SilmeSonucu sonuc = katalog.sil(id);  // d bu satirdan sonra kullanilmaz.
            if (sonuc == SilmeSonucu::basarili) {
                const std::string aciklama = "Kitap silindi: ID " + std::to_string(id);
                std::cout << "Kitap silindi: ID " << id << '\n';
                gecmis.push(aciklama);
                geriAlma.push(IslemKaydi{aciklama, IslemTuru::kitapSilindi, kopya, sonrakiId});
            } else if (sonuc == SilmeSonucu::oduncte) {
                std::cout << "Kitap oduncte, silinemez: ID " << id << ". Once iade alin.\n";
            } else {
                std::cout << "Bu ID ile kitap bulunamadi: " << id << '\n';
            }
            break;
        }
        case 7:
            katalog.analizYazdir();
            break;
        case 8:
            katalog.esikUstuListele(tamSayiOku("Sayfa esigi (0-100000): ", 0, 100000));
            break;
        case 9: {
            const std::string metin = metinOku("Duyuru metni: ");
            duyurular.ekle(metin);
            std::cout << "Duyuru eklendi. Toplam duyuru: " << duyurular.boyut() << '\n';
            gecmis.push("Duyuru eklendi");
            break;
        }
        case 10:
            duyurular.turYazdir();
            break;
        case 11:
            if (katalog.dosyayaYaz(katalogDosyasi)) {
                std::cout << katalog.boyut() << " kayit " << katalogDosyasi << " dosyasina yazildi.\n";
                gecmis.push("Katalog dosyaya yazildi (" + std::to_string(katalog.boyut()) + " kayit)");
            } else {
                std::cout << "Dosyaya yazilamadi: " << katalogDosyasi << '\n';
            }
            break;
        case 12: {
            const int okunan = dosyadanOkuGoster(katalogDosyasi);
            if (okunan < 0) {
                std::cout << "Dosya acilamadi. Once 11 ile katalogu dosyaya yazin.\n";
            } else if (okunan == 0) {
                std::cout << "Dosya bos.\n";
            } else {
                std::cout << okunan << " kayit dosyadan okundu.\n";
            }
            break;
        }
        case 13: {
            if (katalog.oduncteKitapVarMi()) {
                std::cout << "Oduncte kitap var. Katalog bosaltilamaz; once iadeleri alin.\n";
                break;
            }
            const int onay = tamSayiOku("Tum kitaplar silinsin mi? (1 = evet, 0 = hayir): ", 0, 1);
            if (onay == 1) {
                const int silinen = katalog.temizle();
                std::cout << silinen << " kitap dugumu silindi. Katalog bos.\n";
                gecmis.push("Katalog bosaltildi (" + std::to_string(silinen) + " kitap)");
                // Eski kayitlarin komsu ID'leri artik yok; bu kayitlar geri alinamaz.
                const int atilan = geriAlma.temizle();
                if (atilan > 0) {
                    std::cout << "Geri alma yigini da bosaltildi (" << atilan << " kayit).\n";
                }
            }
            break;
        }
        default:
            break;  // 0: donguden cikar
        }
    }
}
