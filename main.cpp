// Kampus Kutuphanesi ve Ogrenci Hizmetleri Otomasyonu
// BLP 205 Veri Yapilari final projesi
// Derleme: g++ -std=c++11 -Wall -Wextra main.cpp ortak.cpp modul1.cpp modul2.cpp modul3.cpp modul4.cpp -o kutuphane

#include "modul1.h"
#include "modul2.h"
#include "modul3.h"
#include "modul4.h"
#include "ortak.h"

#include <iostream>

// Test icin baslangic verileri. ID'ler bilerek karisik sirada:
// boylece Quick Sort'un isi gorunur.
// Son iki alan: oduncte mi, kac kez odunc verildi (gecmis donemden ornek sayilar).
// Kuyruktaki iki ogrenci 101 ve 104'u bekliyor; bu yuzden o iki kitap oduncte.
static void ornekVerileriYukle(KitapListesi& katalog, DuyuruListesi& duyurular,
                               BeklemeKuyrugu& kuyruk, IslemYigini& gecmis) {
    katalog.sonaEkle(Kitap{105, "Ince Memed", "Yasar Kemal", 436, false, 3});
    katalog.sonaEkle(Kitap{102, "Kurk Mantolu Madonna", "Sabahattin Ali", 160, false, 5});
    katalog.sonaEkle(Kitap{108, "1984", "George Orwell", 352, false, 2});
    katalog.sonaEkle(Kitap{101, "Suc ve Ceza", "Fyodor Dostoyevski", 687, true, 4});
    katalog.sonaEkle(Kitap{107, "Kucuk Prens", "Antoine de Saint-Exupery", 112, false, 1});
    katalog.sonaEkle(Kitap{104, "Sefiller", "Victor Hugo", 1463, true, 2});
    katalog.sonaEkle(Kitap{106, "Saatleri Ayarlama Enstitusu", "Ahmet Hamdi Tanpinar", 382, false, 0});
    katalog.sonaEkle(Kitap{103, "Simyaci", "Paulo Coelho", 188, false, 6});

    duyurular.ekle("Final haftasinda kutuphane 24 saat acik.");
    duyurular.ekle("Yeni gelen kitaplar 2. katta sergileniyor.");
    duyurular.ekle("Odunc suresi 15 gundur; gecikmede yeni odunc verilmez.");

    kuyruk.enqueue(Ogrenci{"2026001", "Ayse Yilmaz", 101, "Suc ve Ceza"});
    kuyruk.enqueue(Ogrenci{"2026002", "Mehmet Demir", 104, "Sefiller"});

    gecmis.push("Ornek veriler yuklendi");
}

static void bellekTemizle(KitapListesi& katalog, DuyuruListesi& duyurular,
                          BeklemeKuyrugu& kuyruk, IslemYigini& gecmis) {
    std::cout << "\nBellek temizleniyor (delete):\n"
              << "  Kitap dugumu   : " << katalog.temizle() << '\n'
              << "  Duyuru dugumu  : " << duyurular.temizle() << '\n'
              << "  Kuyruk dugumu  : " << kuyruk.temizle() << '\n'
              << "  Yigin dugumu   : " << gecmis.temizle() << '\n';
}

int main() {
    KitapListesi katalog;
    DuyuruListesi duyurular;
    IslemYigini gecmis;
    BeklemeKuyrugu kuyruk;
    const KampusGrafi kampus;

    ornekVerileriYukle(katalog, duyurular, kuyruk, gecmis);

    try {
        int secim = -1;
        while (secim != 0) {
            std::cout << "\n========================================\n"
                      << " KAMPUS KUTUPHANESI OTOMASYONU\n"
                      << "========================================\n"
                      << "  1) Modul 1 - Katalog ve duyurular (bagli listeler, dosya)\n"
                      << "  2) Modul 2 - Islem gecmisi ve bekleme sirasi (stack, queue)\n"
                      << "  3) Modul 3 - Siralama ve arama\n"
                      << "  4) Modul 4 - Kampus rotalari (graf)\n"
                      << "  0) Cikis\n";
            secim = tamSayiOku("Seciminiz: ", 0, 4);
            switch (secim) {
            case 1:
                modul1Menu(katalog, duyurular, gecmis);
                break;
            case 2:
                modul2Menu(gecmis, kuyruk, katalog, duyurular);
                break;
            case 3:
                modul3Menu(katalog, gecmis);
                break;
            case 4:
                modul4Menu(kampus, gecmis);
                break;
            default:
                break;
            }
        }
    } catch (const GirdiBitti&) {
        std::cout << "\nGirdi sona erdi.\n";
    }

    bellekTemizle(katalog, duyurular, kuyruk, gecmis);
    std::cout << "Program kapandi.\n";
    return 0;
}
