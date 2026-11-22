#ifndef MODUL3_H
#define MODUL3_H

// MODUL 3: Dizi uzerinde Bubble Sort, Insertion Sort, Quick Sort ve Binary Search.

#include "modul1.h"

class IslemYigini;

// Siralama sayaclari. degisim: Bubble ve Quick'te takas, Insertion'da kaydirma.
struct SiralamaSayaci {
    long long karsilastirma;
    long long degisim;
};

SiralamaSayaci bubbleSortSayfa(Kitap dizi[], int n);
SiralamaSayaci insertionSortSayfa(Kitap dizi[], int n);
SiralamaSayaci quickSortId(Kitap dizi[], int n);

// ID'ye gore SIRALI dizide arar. Bulursa indeksi, bulamazsa -1 dondurur.
// adimSayisi: kac kez ortadaki elemana bakildigi.
int binarySearchId(const Kitap dizi[], int n, int arananId, int& adimSayisi);

void diziYazdir(const Kitap dizi[], int n);

void modul3Menu(const KitapListesi& katalog, IslemYigini& gecmis);

#endif
