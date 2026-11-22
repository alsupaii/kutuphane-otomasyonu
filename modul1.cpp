#include "modul1.h"
#include "modul2.h"
#include "ortak.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <utility>

static const std::string katalogDosyasi = "katalog.txt";

// ---------------------------------------------------------------
// Yardimci yazdirma fonksiyonlari
// ---------------------------------------------------------------

static void tabloBasligiYaz() {
    std::cout << std::left << std::setw(7) << "ID" << std::setw(32) << "Kitap adi"
              << std::setw(26) << "Yazar" << std::setw(7) << "Sayfa" << "Durum (odunc sayisi)\n";
    std::cout << std::string(92, '-') << '\n';
}

static void kitapSatiriYaz(const Kitap& k) {
    std::cout << std::left << std::setw(7) << k.id << std::setw(32) << k.ad
              << std::setw(26) << k.yazar << std::setw(7) << k.sayfa
              << (k.oduncte ? "oduncte" : "rafta") << " (" << k.oduncSayisi << ")\n";
}

// ---------------------------------------------------------------
// IdIndeksi: zincirlemeli hash tablosu
// alfa (yuk faktoru) = kayit sayisi / kova sayisi. Ortalama zincir boyu alfa'dir.
// ---------------------------------------------------------------

IdIndeksi::IdIndeksi() : adet(0) {
    for (int i = 0; i < kovaSayisi; i++) {
        kova[i] = nullptr;
    }
}

IdIndeksi::~IdIndeksi() { temizle(); }

int IdIndeksi::boyut() const { return adet; }

// Yeni kayit zincirin BASINA girer: zincirin sonunu aramaya gerek yok, O(1).
// Tekrar eden ID kontrolu KitapListesi'nde yapilir.
void IdIndeksi::ekle(int id, KitapDugum* kitap) {
    const int k = kovaNo(id);
    kova[k] = new IndeksDugum{id, kitap, kova[k]};  // Yeni kayit eski zincir basini gosterir.
    adet++;
}

// Yalniz ID'nin dustugu kovanin zinciri taranir. adim: kac kayitla karsilastirildi.
KitapDugum* IdIndeksi::bul(int id, int& adim) const {
    adim = 0;
    for (const IndeksDugum* p = kova[kovaNo(id)]; p != nullptr; p = p->next) {
        adim++;
        if (p->id == id) {
            return p->kitap;
        }
    }
    return nullptr;
}

// Tek yonlu zincirden silme: onceki kaydi da tutarak ilerle.
bool IdIndeksi::sil(int id) {
    const int k = kovaNo(id);
    IndeksDugum* onceki = nullptr;
    IndeksDugum* p = kova[k];
    while (p != nullptr && p->id != id) {
        onceki = p;
        p = p->next;
    }
    if (p == nullptr) {
        return false;
    }
    if (onceki == nullptr) {
        kova[k] = p->next;        // Silinen zincirin basiydi.
    } else {
        onceki->next = p->next;   // Oncekini silinenin arkasina bagla.
    }
    delete p;                     // Yalniz indeks kaydi silinir; kitap dugumu katalogun isi.
    adet--;
    return true;
}

int IdIndeksi::temizle() {
    int silinen = 0;
    for (int i = 0; i < kovaSayisi; i++) {
        while (kova[i] != nullptr) {
            IndeksDugum* sonraki = kova[i]->next;
            delete kova[i];
            kova[i] = sonraki;
            silinen++;
        }
    }
    adet = 0;
    return silinen;
}

void IdIndeksi::takas(IdIndeksi& diger) {
    for (int i = 0; i < kovaSayisi; i++) {
        std::swap(kova[i], diger.kova[i]);
    }
    std::swap(adet, diger.adet);
}

// Her dolu kovanin zincirini yazar ve yuk faktorunu hesaplar.
void IdIndeksi::dagilimYazdir() const {
    int dolu = 0;
    int enUzun = 0;
    for (int i = 0; i < kovaSayisi; i++) {
        int uzunluk = 0;
        for (const IndeksDugum* p = kova[i]; p != nullptr; p = p->next) {
            uzunluk++;
        }
        if (uzunluk == 0) {
            continue;
        }
        dolu++;
        if (uzunluk > enUzun) {
            enUzun = uzunluk;
        }
        std::cout << "  kova " << std::setw(2) << i << ":";
        for (const IndeksDugum* p = kova[i]; p != nullptr; p = p->next) {
            std::cout << ' ' << p->id << (p->next != nullptr ? " ->" : "");
        }
        std::cout << '\n';
    }
    std::cout << adet << " kayit, " << kovaSayisi << " kova, " << dolu << " kova dolu, en uzun zincir "
              << enUzun << ". Yuk faktoru alfa = " << adet << "/" << kovaSayisi << " = " << std::fixed
              << std::setprecision(2) << static_cast<double>(adet) / kovaSayisi << '\n';
    std::cout.unsetf(std::ios::fixed);
    std::cout << std::setprecision(6);
}

// ---------------------------------------------------------------
// KitapListesi: cift yonlu dogrusal bagli liste + hash indeksi
// ---------------------------------------------------------------

KitapListesi::KitapListesi() : bas(nullptr), son(nullptr), adet(0) {}

KitapListesi::~KitapListesi() { temizle(); }

// ID ile arama artik indeksten yapilir: ortalama O(1 + alfa). Liste taranmaz.
KitapDugum* KitapListesi::dugumBul(int id) const {
    int adim = 0;
    return indeks.bul(id, adim);
}

const KitapDugum* KitapListesi::bul(int id) const { return dugumBul(id); }

// Karsilastirma icin eski yontem: bastan sona yuru. O(n).
int KitapListesi::dogrusalAramaAdimi(int id, bool& bulundu) const {
    int adim = 0;
    for (const KitapDugum* p = bas; p != nullptr; p = p->next) {
        adim++;
        if (p->veri.id == id) {
            bulundu = true;
            return adim;
        }
    }
    bulundu = false;
    return adim;
}

int KitapListesi::hashAramaAdimi(int id, bool& bulundu) const {
    int adim = 0;
    bulundu = (indeks.bul(id, adim) != nullptr);
    return adim;
}

void KitapListesi::indeksDagilimiYazdir() const { indeks.dagilimYazdir(); }

bool KitapListesi::bosMu() const { return bas == nullptr; }

int KitapListesi::boyut() const { return adet; }

// Basa ekleme. O(1): ID kontrolu de artik indeksten, ortalama O(1).
EklemeSonucu KitapListesi::basaEkle(const Kitap& k) {
    if (dugumBul(k.id) != nullptr) {
        return EklemeSonucu::idZatenVar;
    }
    KitapDugum* yeni = new KitapDugum{k, nullptr, nullptr};
    indeks.ekle(k.id, yeni);      // Liste ve indeks birlikte guncellenir.
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

// Sona ekleme. son isaretcisi tutuldugu icin O(1); ID kontrolu ortalama O(1).
EklemeSonucu KitapListesi::sonaEkle(const Kitap& k) {
    if (dugumBul(k.id) != nullptr) {
        return EklemeSonucu::idZatenVar;
    }
    KitapDugum* yeni = new KitapDugum{k, nullptr, nullptr};
    indeks.ekle(k.id, yeni);      // Liste ve indeks birlikte guncellenir.
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

// hedefId'li kitabin hemen onune ekler. Hedef indeksten bulunur: ortalama O(1).
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
    indeks.ekle(k.id, yeni);      // Liste ve indeks birlikte guncellenir.
    yeni->prev = hedef->prev;     // 1) yeni, hedefin eski oncekisini gosterir
    yeni->next = hedef;           // 2) yeni -> hedef
    hedef->prev->next = yeni;     // 3) eski onceki -> yeni
    hedef->prev = yeni;           // 4) hedef <- yeni
    adet++;
    return EklemeSonucu::basarili;
}

// ID'ye gore silme. Bas, orta, son ve tek dugum durumlarini kapsar.
// Hedef indeksten bulunur, prev isaretcisi sayesinde sokme O(1): toplam ortalama O(1).
// Kural: oduncteki kitap silinmez. Yoksa bekleme kuyrugu olmayan bir kitabi bekler.
SilmeSonucu KitapListesi::sil(int id) {
    KitapDugum* hedef = dugumBul(id);
    if (hedef == nullptr) {
        return SilmeSonucu::bulunamadi;
    }
    if (hedef->veri.oduncte) {
        return SilmeSonucu::oduncte;
    }
    // SIRA: once indeksten cikar, sonra listeden sok, en son delete.
    // Ters sirada indekste silinmis dugumu gosteren (sarkan) bir isaretci kalirdi.
    indeks.sil(id);
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

// Kitabi indeksten bulur ve durumunu degistirir. Ortalama O(1).
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
    indeks.temizle();  // Indeks kayitlari silinmis dugumleri gostermesin.
    return silinen;
}

// next baglantisiyla bastan sona. O(n).
void KitapListesi::ileriListele() const {
    if (bas == nullptr) {
        std::cout << "Katalog bos. Listelenecek kitap yok.\n";
        return;
    }
    tabloBasligiYaz();
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
    tabloBasligiYaz();
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
                tabloBasligiYaz();
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

// ---------------------------------------------------------------
// Dosya bicimi: her kitap bir satir, alanlar '|' ile ayrilir.
//   id|ad|yazar|sayfa|oduncte|oduncSayisi     ornek: 101|Suc ve Ceza|Dostoyevski|687|1|4
// Eski 4 alanli satir (id|ad|yazar|sayfa) da okunur: kitap rafta, sayac 0.
// ---------------------------------------------------------------

// Bastan sona ileri yonde yazar. Yuklemede sona ekleme ayni sirayi geri kurar.
bool KitapListesi::dosyayaYaz(const std::string& dosyaAdi) const {
    std::ofstream dosya(dosyaAdi);
    if (!dosya) {
        return false;
    }
    for (const KitapDugum* p = bas; p != nullptr; p = p->next) {
        dosya << p->veri.id << '|' << p->veri.ad << '|' << p->veri.yazar << '|'
              << p->veri.sayfa << '|' << (p->veri.oduncte ? 1 : 0) << '|'
              << p->veri.oduncSayisi << '\n';
    }
    return static_cast<bool>(dosya);
}

// Metnin tamami enAz..enCok araliginda bir tam sayiysa true. "12abc" ve "" gecersiz.
static bool tamSayiCoz(const std::string& metin, int enAz, int enCok, int& deger) {
    std::istringstream akis(metin);
    char fazla = '\0';
    return (akis >> deger) && !(akis >> fazla) && deger >= enAz && deger <= enCok;
}

// Windows'ta yazilan dosyada satir sonunda '\r' kalabilir; onu atar.
static void sondakiCrSil(std::string& satir) {
    if (!satir.empty() && satir[satir.size() - 1] == '\r') {
        satir.erase(satir.size() - 1);
    }
}

// Bir dosya satirini Kitap'a cevirir. Alan sayisi ya da bir deger gecersizse false.
static bool satirCoz(const std::string& satir, Kitap& k) {
    std::string alan[6];
    int alanSayisi = 0;
    std::istringstream akis(satir);
    std::string parca;
    while (std::getline(akis, parca, '|')) {
        if (alanSayisi == 6) {
            return false;  // 6'dan fazla alan
        }
        alan[alanSayisi] = parca;
        alanSayisi++;
    }
    if (alanSayisi != 4 && alanSayisi != 6) {
        return false;
    }
    int oduncte = 0;
    int sayac = 0;
    if (!tamSayiCoz(alan[0], 1, 999999, k.id) || alan[1].empty() || alan[2].empty() ||
        !tamSayiCoz(alan[3], 1, 100000, k.sayfa)) {
        return false;
    }
    if (alanSayisi == 6 &&
        (!tamSayiCoz(alan[4], 0, 1, oduncte) || !tamSayiCoz(alan[5], 0, 1000000, sayac))) {
        return false;
    }
    k.ad = alan[1];
    k.yazar = alan[2];
    k.oduncte = (oduncte == 1);
    k.oduncSayisi = sayac;
    return true;
}

// Dosyadaki kitaplari ONCE bos bir gecici listeye okur, sonra katalogla yer degistirir.
// Yer degistirme uc degeri (bas, son, adet) ve 31 kova isaretcisini takas eder: dugum kopyalanmaz.
// Fonksiyon bitince gecici listenin yikicisi ESKI katalogun dugumlerini siler.
// Dosya acilamazsa katalog hic degismez. Bozuk ya da tekrar eden ID'li satir atlanir.
// Dondurur: yuklenen kitap sayisi, dosya acilamazsa -1. Sure: ortalama O(n), ID kontrolu indeksten.
int KitapListesi::dosyadanYukle(const std::string& dosyaAdi, int& atlanan) {
    atlanan = 0;
    std::ifstream dosya(dosyaAdi);
    if (!dosya) {
        return -1;
    }
    KitapListesi gecici;
    std::string satir;
    while (std::getline(dosya, satir)) {
        sondakiCrSil(satir);
        if (satir.empty()) {
            continue;
        }
        Kitap k{0, "", "", 0, false, 0};
        if (!satirCoz(satir, k) || gecici.sonaEkle(k) != EklemeSonucu::basarili) {
            atlanan++;
        }
    }
    std::swap(bas, gecici.bas);
    std::swap(son, gecici.son);
    std::swap(adet, gecici.adet);
    indeks.takas(gecici.indeks);  // Indeks de yer degistirir; eskisini gecici siler.
    return adet;
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

// Dosyayi okur ve ekrana yazar; katalogu DEGISTIRMEZ. Okunan satir sayisini dondurur.
int dosyadanOkuGoster(const std::string& dosyaAdi) {
    std::ifstream dosya(dosyaAdi);
    if (!dosya) {
        return -1;
    }
    std::string satir;
    int kayitSayisi = 0;
    bool baslikYazildi = false;
    while (std::getline(dosya, satir)) {
        sondakiCrSil(satir);
        if (satir.empty()) {
            continue;
        }
        kayitSayisi++;
        Kitap k{0, "", "", 0, false, 0};
        if (!satirCoz(satir, k)) {
            std::cout << "  Bozuk satir atlandi: " << satir << '\n';
            continue;
        }
        if (!baslikYazildi) {
            tabloBasligiYaz();
            baslikYazildi = true;
        }
        kitapSatiriYaz(k);
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

// Ayni ID'yi iki yolla arar ve karsilastirma sayilarini yan yana yazar.
static void aramaKarsilastir(const KitapListesi& katalog, int id) {
    bool dogrusalBuldu = false;
    bool hashBuldu = false;
    const int dogrusal = katalog.dogrusalAramaAdimi(id, dogrusalBuldu);
    const int hash = katalog.hashAramaAdimi(id, hashBuldu);
    std::cout << "ID " << id << (hashBuldu ? " bulundu" : " bulunamadi") << " (kova " << id % kovaSayisi
              << ")\n"
              << "  Dogrusal arama (liste): " << dogrusal << " karsilastirma\n"
              << "  Hash indeksi (zincir) : " << hash << " karsilastirma\n";
    if (dogrusalBuldu != hashBuldu) {
        std::cout << "  HATA: liste ve indeks uyusmuyor!\n";  // Tutarlilik kurali bozulursa gorunur.
    }
}

// 1000 kitaplik gecici bir katalog kurar; her ID'yi iki yolla arayip ortalama adimi hesaplar.
// ID'ler 1..1000 artan: kovalara esit dagilir (id % 31). Gecici katalog fonksiyon sonunda silinir.
static void hashDenemesi() {
    const int n = 1000;
    KitapListesi deneme;
    for (int id = 1; id <= n; id++) {
        deneme.sonaEkle(Kitap{id, "Deneme", "Yazar", 100, false, 0});
    }
    long long dogrusalToplam = 0;
    long long hashToplam = 0;
    int dogrusalEnKotu = 0;
    int hashEnKotu = 0;
    bool bulundu = false;
    for (int id = 1; id <= n; id++) {
        const int d = deneme.dogrusalAramaAdimi(id, bulundu);
        const int h = deneme.hashAramaAdimi(id, bulundu);
        dogrusalToplam += d;
        hashToplam += h;
        if (d > dogrusalEnKotu) dogrusalEnKotu = d;
        if (h > hashEnKotu) hashEnKotu = h;
    }
    std::cout << "1000 kitaplik deneme (her ID bir kez aranir):\n" << std::fixed << std::setprecision(1)
              << "  Dogrusal: ortalama " << static_cast<double>(dogrusalToplam) / n
              << " karsilastirma, en kotu " << dogrusalEnKotu << '\n'
              << "  Hash    : ortalama " << static_cast<double>(hashToplam) / n
              << " karsilastirma, en kotu " << hashEnKotu << '\n';
    std::cout.unsetf(std::ios::fixed);
    std::cout << std::setprecision(6);
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
                  << " 14) Dosyadan kataloga yukle (" << katalogDosyasi << ")\n"
                  << " 15) ID ile ara (hash indeksi ve dogrusal arama karsilastirmasi)\n"
                  << " 16) Hash indeksi: kova dagilimi ve 1000 kitaplik deneme\n"
                  << "  0) Ana menuye don\n";
        secim = tamSayiOku("Seciminiz: ", 0, 16);

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
        case 14: {
            // Kural: bekleme kuyrugu oduncteki kitaplari bekler. Katalog degisirse kuyruk
            // olmayan kitaplari bekleyebilir. Bu yuzden oduncte kitap varken yukleme yapilmaz.
            if (katalog.oduncteKitapVarMi()) {
                std::cout << "Oduncte kitap var. Dosyadan yukleme yapilamaz; once iadeleri alin.\n";
                break;
            }
            const int onay = tamSayiOku("Mevcut katalog dosyadakiyle degissin mi? (1 = evet, 0 = hayir): ", 0, 1);
            if (onay == 0) {
                break;
            }
            int atlanan = 0;
            const int yuklenen = katalog.dosyadanYukle(katalogDosyasi, atlanan);
            if (yuklenen < 0) {
                std::cout << "Dosya acilamadi. Katalog degismedi. Once 11 ile katalogu dosyaya yazin.\n";
                break;
            }
            std::cout << yuklenen << " kitap dosyadan kataloga yuklendi.";
            if (atlanan > 0) {
                std::cout << " " << atlanan << " satir atlandi (bozuk ya da tekrar eden ID).";
            }
            std::cout << '\n';
            gecmis.push("Katalog dosyadan yuklendi (" + std::to_string(yuklenen) + " kitap)");
            // Eski katalogun komsu ID'leri gecersiz; eski geri alma kayitlari atilir.
            const int atilan = geriAlma.temizle();
            if (atilan > 0) {
                std::cout << "Geri alma yigini bosaltildi (" << atilan << " kayit).\n";
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
        case 15:
            aramaKarsilastir(katalog, tamSayiOku("Aranan kitap ID: ", 1, 999999));
            break;
        case 16:
            std::cout << "Katalog indeksi (hash = ID % " << kovaSayisi << "):\n";
            katalog.indeksDagilimiYazdir();
            hashDenemesi();
            break;
        default:
            break;  // 0: donguden cikar
        }
    }
}
