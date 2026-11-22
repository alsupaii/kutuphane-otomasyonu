// Ortanca-uc Quick Sort dogrulama: 1..200 boyutunda rastgele, sirali, ters ve tekrarli diziler.
#include "modul3.h"
#include <cstdlib>
#include <iostream>

static bool siraliMi(const Kitap a[], int n) {
    for (int i = 1; i < n; i++) {
        if (a[i - 1].id > a[i].id) return false;
    }
    return true;
}

int main() {
    std::srand(12345);
    int hata = 0, deneme = 0;
    for (int n = 0; n <= 200; n++) {
        for (int tur = 0; tur < 4; tur++) {
            Kitap* a = new Kitap[n > 0 ? n : 1];
            Kitap* b = new Kitap[n > 0 ? n : 1];
            for (int i = 0; i < n; i++) {
                int id = 0;
                if (tur == 0) id = std::rand() % 1000;
                if (tur == 1) id = i;
                if (tur == 2) id = n - i;
                if (tur == 3) id = std::rand() % 5;  // cok tekrar
                a[i] = Kitap{id, "", "", 0, false, 0};
                b[i] = a[i];
            }
            quickSortIdOrtancaUc(a, n);
            quickSortId(b, n);
            deneme++;
            bool esit = true;
            for (int i = 0; i < n; i++) if (a[i].id != b[i].id) esit = false;
            if (!siraliMi(a, n) || !esit) hata++;
            delete[] a;
            delete[] b;
        }
    }
    std::cout << deneme << " dizi denendi, hata: " << hata << '\n';
    return hata == 0 ? 0 : 1;
}
