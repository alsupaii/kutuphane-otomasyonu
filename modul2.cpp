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

void IslemYigini::push(const IslemKaydi& kayit) {
    YiginDugum* yeni = new YiginDugum{kayit, ust};  // Yeni dugum eski ustu gosterir.
    ust = yeni;                                      // Ust artik yeni dugum.
    adet++;
}

void IslemYigini::push(const std::string& aciklama) {
    push(IslemKaydi{aciklama, IslemTuru::bilgi, Kitap{0, "", "", 0, false, 0}, -1});
}

bool IslemYigini::pop(IslemKaydi& cikan) {
    if (ust == nullptr) {
        return false;  // Bos yigindan cikarma yapilamaz (underflow).
    }
    YiginDugum* silinecek = ust;
    cikan = silinecek->veri;
    ust = ust->next;   // Once ustu bir alta indir...
    delete silinecek;  // ...sonra eski ustu sil. Ters sirada ust->next silinmis bellekten okunurdu.
    adet--;
    return true;
}

bool IslemYigini::peek(IslemKaydi& ustteki) const {
    if (ust == nullptr) {
        return false;
    }
    ustteki = ust->veri;
    return true;
}

// Ustten alta dogru yazar: en yeni islem en ustte. O(n).
void IslemYigini::listele() const {
    if (ust == nullptr) {
        std::cout << "Yigin bos.\n";
        return;
    }
    std::cout << "Yigin (ustten alta, en yeni en ustte):\n";
    int sira = 1;
    for (const YiginDugum* p = ust; p != nullptr; p = p->next) {
        std::cout << "  " << sira << ") " << p->veri.aciklama << (sira == 1 ? "   <- ust" : "") << '\n';
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
// HareketTamponu: dizi tabanli dairesel tampon
// Yazma indisi: (bas + adet) % kapasite. Son hucreden sonra 0. hucreye donulur.
// ---------------------------------------------------------------

HareketTamponu::HareketTamponu() : bas(0), adet(0), toplam(0) {}

int HareketTamponu::boyut() const { return adet; }

void HareketTamponu::yaz(const std::string& hareket) {
    if (adet < hareketKapasitesi) {
        kayit[(bas + adet) % hareketKapasitesi] = hareket;  // Bos hucre var: oraya yaz.
        adet++;
    } else {
        kayit[bas] = hareket;                       // Dolu: en eskinin ustune yaz...
        bas = (bas + 1) % hareketKapasitesi;        // ...en eski artik bir sonraki hucre.
    }
    toplam++;
}

// bas'tan baslar, adet kadar ilerler. Indis sona gelince % ile 0'a doner.
void HareketTamponu::listele() const {
    if (adet == 0) {
        std::cout << "Hareket yok (tampon bos).\n";
        return;
    }
    std::cout << "Son " << adet << " hareket (eskiden yeniye), kapasite " << hareketKapasitesi
              << ":\n";
    for (int i = 0; i < adet; i++) {
        const int hucre = (bas + i) % hareketKapasitesi;
        std::cout << "  " << (i + 1) << ") [hucre " << hucre << "] " << kayit[hucre] << '\n';
    }
    std::cout << "Toplam yazilan: " << toplam << ", uzerine yazilan (dusen): "
              << (toplam - adet) << '\n';
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
// Odunc hareketini iki yere yazar: gecmis yigini (sinirsiz) ve hareket tamponu (son 10).
static void hareketKaydet(IslemYigini& gecmis, HareketTamponu& hareketler, const std::string& metin) {
    gecmis.push(metin);
    hareketler.yaz(metin);
}

static void oduncTalebi(IslemYigini& gecmis, HareketTamponu& hareketler, BeklemeKuyrugu& kuyruk,
                        KitapListesi& katalog) {
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
        hareketKaydet(gecmis, hareketler, "Odunc verildi: ID " + idMetni + " -> " + o.ogrenciNo);
    } else if (kuyruk.siradaMi(o.ogrenciNo, o.kitapId)) {
        std::cout << "Bu ogrenci bu kitap icin zaten sirada.\n";
    } else {
        kuyruk.enqueue(o);
        std::cout << "Kitap oduncte. " << o.adSoyad << " bekleme sirasina eklendi. Kuyruktaki yeri: "
                  << kuyruk.boyut() << '\n';
        hareketKaydet(gecmis, hareketler,
                      "Bekleme sirasina eklendi: " + o.ogrenciNo + " -> ID " + idMetni);
    }
}

// Iade. Kitabi bekleyen varsa kitap rafa donmez, siradaki ogrenciye gecer ve duyuru yazilir.
// Dokundugu yapilar: katalog, kuyruk, yigin, duyuru listesi.
static void iadeAl(IslemYigini& gecmis, HareketTamponu& hareketler, BeklemeKuyrugu& kuyruk,
                   KitapListesi& katalog, DuyuruListesi& duyurular) {
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
        hareketKaydet(gecmis, hareketler,
                      "Iade alindi: ID " + idMetni + ", siradaki ogrenciye verildi: " +
                          siradaki.ogrenciNo);
    } else {
        katalog.durumDegistir(id, false);
        std::cout << "Iade alindi. Bekleyen yok, kitap rafa dondu: " << kitap->veri.ad << '\n';
        hareketKaydet(gecmis, hareketler, "Iade alindi: ID " + idMetni + ", rafa dondu");
    }
}

// Geri alma yiginin ustundeki katalog islemini tersine cevirir. LIFO: en son islem ilk geri alinir.
// Once peek: ters islem basarisiz olursa kayit yiginda kalir. Basariliysa pop.
// Ters islem geri alma yiginina yeni kayit YAZMAZ. Yazsaydi ikinci "geri al" ilkini iptal ederdi.
static void geriAl(IslemYigini& geriAlma, IslemYigini& gecmis, KitapListesi& katalog) {
    IslemKaydi kayit;
    if (!geriAlma.peek(kayit)) {
        std::cout << "Geri alinacak islem yok.\n";
        return;
    }
    const Kitap& k = kayit.kitap;
    if (kayit.tur == IslemTuru::kitapEklendi) {
        const SilmeSonucu sonuc = katalog.sil(k.id);  // Eklemenin tersi: sil.
        if (sonuc == SilmeSonucu::oduncte) {
            std::cout << "Kitap oduncte, ekleme geri alinamaz: ID " << k.id << ". Once iade alin.\n";
            return;
        }
        if (sonuc == SilmeSonucu::bulunamadi) {
            std::cout << "Kitap katalogda yok, geri alinamadi: ID " << k.id << '\n';
            return;
        }
    } else {
        // Silmenin tersi: kitabi eski arka komsusunun onune koy. Komsu yoksa kitap sondaydi.
        // Komsu sonradan silindiyse? Onun silinmesi yiginda daha ustte; LIFO ile once o geri gelir.
        const EklemeSonucu sonuc = (kayit.sonrakiKomsuId == -1)
                                       ? katalog.sonaEkle(k)
                                       : katalog.oncesineEkle(kayit.sonrakiKomsuId, k);
        if (sonuc != EklemeSonucu::basarili) {
            std::cout << "Kitap eski yerine konamadi, geri alinamadi: ID " << k.id << '\n';
            return;
        }
    }
    geriAlma.pop(kayit);
    std::cout << "Geri alindi: " << kayit.aciklama << '\n';
    gecmis.push("Geri alindi: " + kayit.aciklama);
}

void modul2Menu(IslemYigini& gecmis, IslemYigini& geriAlma, HareketTamponu& hareketler,
                BeklemeKuyrugu& kuyruk, KitapListesi& katalog, DuyuruListesi& duyurular) {
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
                  << "  9) Geri al: son kitap ekleme/silme islemini geri al (undo)\n"
                  << " 10) Geri alma yiginini listele\n"
                  << " 11) Son 10 odunc/iade hareketi (dairesel tampon)\n"
                  << "  0) Ana menuye don\n";
        secim = tamSayiOku("Seciminiz: ", 0, 11);

        switch (secim) {
        case 1:
            gecmis.push(metinOku("Islem aciklamasi: "));
            std::cout << "Kayit yigina eklendi. Yigin boyutu: " << gecmis.boyut() << '\n';
            break;
        case 2: {
            IslemKaydi cikan;
            if (gecmis.pop(cikan)) {
                std::cout << "Cikarilan kayit: " << cikan.aciklama << '\n';
            } else {
                std::cout << "Yigin bos. Pop yapilamaz.\n";
            }
            break;
        }
        case 3: {
            IslemKaydi ustteki;
            if (gecmis.peek(ustteki)) {
                std::cout << "Ustteki kayit: " << ustteki.aciklama << '\n';
            } else {
                std::cout << "Yigin bos. Ustte kayit yok.\n";
            }
            break;
        }
        case 4:
            gecmis.listele();
            break;
        case 9:
            geriAl(geriAlma, gecmis, katalog);
            break;
        case 10:
            geriAlma.listele();
            break;
        case 11:
            hareketler.listele();
            break;
        case 5:
            oduncTalebi(gecmis, hareketler, kuyruk, katalog);
            break;
        case 6:
            iadeAl(gecmis, hareketler, kuyruk, katalog, duyurular);
            break;
        case 7: {
            Ogrenci o;
            if (kuyruk.dequeue(o)) {
                std::cout << "Siradan cikti (talep iptal): " << o.ogrenciNo << " - " << o.adSoyad
                          << " -> " << o.kitapAdi << '\n';
                hareketKaydet(gecmis, hareketler,
                              "Talep iptal: " + o.ogrenciNo + " -> ID " + std::to_string(o.kitapId));
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
