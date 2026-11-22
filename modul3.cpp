#include "modul3.h"
#include "modul2.h"
#include "ortak.h"

#include <iomanip>
#include <iostream>
#include <utility>

// ---------------------------------------------------------------
// Siralama algoritmalari
// ---------------------------------------------------------------

// Bubble Sort, sayfa sayisina gore kucukten buyuge.
// Her turda en buyuk eleman sona "kabarcik" gibi cikar.
// En kotu ve ortalama O(n^2). Dizi zaten siraliysa erken cikis ile O(n).
SiralamaSayaci bubbleSortSayfa(Kitap dizi[], int n) {
    SiralamaSayaci s{0, 0};
    for (int tur = 0; tur < n - 1; tur++) {
        bool takasOldu = false;
        // Son "tur" eleman zaten yerinde, onlara bakmaya gerek yok.
        for (int j = 0; j < n - 1 - tur; j++) {
            s.karsilastirma++;
            if (dizi[j].sayfa > dizi[j + 1].sayfa) {
                std::swap(dizi[j], dizi[j + 1]);
                s.degisim++;
                takasOldu = true;
            }
        }
        if (!takasOldu) {
            break;  // Bu turda hic takas yoksa dizi siralidir.
        }
    }
    return s;
}

// Insertion Sort, sayfa sayisina gore kucukten buyuge.
// Soldaki kisim hep siralidir; siradaki elemani (anahtar) uygun yere sokar.
// Buyuk elemanlar bir saga KAYDIRILIR, takas yapilmaz.
// En kotu O(n^2), neredeyse sirali dizide O(n).
SiralamaSayaci insertionSortSayfa(Kitap dizi[], int n) {
    SiralamaSayaci s{0, 0};
    for (int i = 1; i < n; i++) {
        const Kitap anahtar = dizi[i];
        int j = i - 1;
        while (j >= 0) {
            s.karsilastirma++;
            if (dizi[j].sayfa > anahtar.sayfa) {
                dizi[j + 1] = dizi[j];  // Bir saga kaydir.
                s.degisim++;
                j--;
            } else {
                break;
            }
        }
        dizi[j + 1] = anahtar;
    }
    return s;
}

// Lomuto bolumleme: pivot = araligin son elemani.
// Pivottan kucukleri sola toplar, pivotu araya koyar, pivotun yeni indeksini dondurur.
static int bolumle(Kitap dizi[], int alt, int ust, SiralamaSayaci& s) {
    const int pivot = dizi[ust].id;
    int i = alt - 1;  // Pivottan kucuk bolgenin son indeksi
    for (int j = alt; j < ust; j++) {
        s.karsilastirma++;
        if (dizi[j].id < pivot) {
            i++;
            if (i != j) {
                std::swap(dizi[i], dizi[j]);
                s.degisim++;
            }
        }
    }
    if (i + 1 != ust) {
        std::swap(dizi[i + 1], dizi[ust]);
        s.degisim++;
    }
    return i + 1;
}

static void quickSortYardimci(Kitap dizi[], int alt, int ust, SiralamaSayaci& s) {
    if (alt >= ust) {
        return;  // 0 veya 1 elemanli aralik zaten siralidir.
    }
    const int pivotYeri = bolumle(dizi, alt, ust, s);
    quickSortYardimci(dizi, alt, pivotYeri - 1, s);  // Pivotun solu
    quickSortYardimci(dizi, pivotYeri + 1, ust, s);  // Pivotun sagi
}

// Quick Sort, ID'ye gore kucukten buyuge.
// Ortalama O(n log n). En kotu O(n^2): dizi zaten siraliysa pivot hep en buyuk cikar.
SiralamaSayaci quickSortId(Kitap dizi[], int n) {
    SiralamaSayaci s{0, 0};
    quickSortYardimci(dizi, 0, n - 1, s);
    return s;
}

// Binary Search. Her adimda arama araligi yariya iner: O(log n).
// Dizi ID'ye gore sirali OLMALI, yoksa sonuc yanlis olur.
int binarySearchId(const Kitap dizi[], int n, int arananId, int& adimSayisi) {
    int alt = 0;
    int ust = n - 1;
    adimSayisi = 0;
    while (alt <= ust) {
        const int orta = alt + (ust - alt) / 2;  // (alt+ust)/2 ile ayni, tasma riski yok.
        adimSayisi++;
        std::cout << "  Adim " << adimSayisi << ": alt=" << alt << " ust=" << ust
                  << " orta=" << orta << " (ID " << dizi[orta].id << ")\n";
        if (dizi[orta].id == arananId) {
            return orta;
        }
        if (dizi[orta].id < arananId) {
            alt = orta + 1;  // Aranan sagda
        } else {
            ust = orta - 1;  // Aranan solda
        }
    }
    return -1;
}

void diziYazdir(const Kitap dizi[], int n) {
    std::cout << "  " << std::left << std::setw(6) << "Indis" << std::setw(7) << "ID"
              << std::setw(32) << "Kitap adi" << "Sayfa\n";
    for (int i = 0; i < n; i++) {
        std::cout << "  " << std::left << std::setw(6) << i << std::setw(7) << dizi[i].id
                  << std::setw(32) << dizi[i].ad << dizi[i].sayfa << '\n';
    }
}

// ---------------------------------------------------------------
// Menu
// ---------------------------------------------------------------

static void sayacYaz(const SiralamaSayaci& s, const std::string& degisimAdi) {
    std::cout << "Karsilastirma sayisi: " << s.karsilastirma << '\n'
              << degisimAdi << " sayisi: " << s.degisim << '\n';
}

void modul3Menu(const KitapListesi& katalog, IslemYigini& gecmis) {
    int secim = -1;
    while (secim != 0) {
        std::cout << "\n=== MODUL 3: SIRALAMA VE ARAMA ===\n"
                  << "  1) Katalogu diziye aktar ve goster\n"
                  << "  2) Bubble Sort (sayfa sayisina gore)\n"
                  << "  3) Insertion Sort (sayfa sayisina gore)\n"
                  << "  4) Quick Sort (ID'ye gore)\n"
                  << "  5) Binary Search (ID ile, Quick Sort sonrasi)\n"
                  << "  0) Ana menuye don\n";
        secim = tamSayiOku("Seciminiz: ", 0, 5);
        if (secim == 0) {
            break;
        }
        const int n = katalog.boyut();
        if (n == 0) {
            std::cout << "Katalog bos. Diziye aktarilacak kitap yok.\n";
            continue;
        }
        // Aranan ID, dizi acilmadan ONCE okunur. Okuma sirasinda girdi biterse
        // istisna firlar; dizi henuz acilmadigi icin bellek sizmaz.
        int arananId = 0;
        if (secim == 5) {
            arananId = tamSayiOku("Aranan kitap ID: ", 1, 999999);
        }

        // Her islem katalogun TAZE kopyasiyla calisir; bagli liste degismez.
        Kitap* dizi = new Kitap[n];
        katalog.diziyeAktar(dizi, n);

        switch (secim) {
        case 1:
            std::cout << "Bagli listeden diziye " << n << " kayit aktarildi:\n";
            diziYazdir(dizi, n);
            break;
        case 2: {
            std::cout << "Once:\n";
            diziYazdir(dizi, n);
            const SiralamaSayaci s = bubbleSortSayfa(dizi, n);
            std::cout << "Bubble Sort sonrasi (sayfa artan):\n";
            diziYazdir(dizi, n);
            sayacYaz(s, "Takas");
            gecmis.push("Bubble Sort calisti");
            break;
        }
        case 3: {
            std::cout << "Once:\n";
            diziYazdir(dizi, n);
            const SiralamaSayaci s = insertionSortSayfa(dizi, n);
            std::cout << "Insertion Sort sonrasi (sayfa artan):\n";
            diziYazdir(dizi, n);
            sayacYaz(s, "Kaydirma");
            gecmis.push("Insertion Sort calisti");
            break;
        }
        case 4: {
            std::cout << "Once:\n";
            diziYazdir(dizi, n);
            const SiralamaSayaci s = quickSortId(dizi, n);
            std::cout << "Quick Sort sonrasi (ID artan):\n";
            diziYazdir(dizi, n);
            sayacYaz(s, "Takas");
            gecmis.push("Quick Sort calisti");
            break;
        }
        case 5: {
            quickSortId(dizi, n);  // Binary Search sirali dizi ister.
            std::cout << "Dizi Quick Sort ile ID'ye gore siralandi:\n";
            diziYazdir(dizi, n);
            int adim = 0;
            const int indis = binarySearchId(dizi, n, arananId, adim);
            if (indis >= 0) {
                std::cout << "BULUNDU: ID " << arananId << " -> " << dizi[indis].ad
                          << " (indis " << indis << ", " << adim << " adim)\n";
            } else {
                std::cout << "BULUNAMADI: ID " << arananId << " (" << adim << " adim)\n";
            }
            gecmis.push("Binary Search: ID " + std::to_string(arananId) +
                        (indis >= 0 ? " bulundu" : " bulunamadi"));
            break;
        }
        default:
            break;
        }

        delete[] dizi;  // new[] ile acilan dizi delete[] ile silinir.
    }
}
