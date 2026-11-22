// Liste ile hash indeksinin tutarliligi: rastgele 20000 islem (basa/sona/onune ekle, sil,
// odunc durumu, arada bosalt). Her islemden sonra 1..150 arasindaki her ID icin
// dogrusal arama ile hash aramasi ayni sonucu vermeli.
#include "modul1.h"
#include <cstdlib>
#include <iostream>

int main() {
    std::srand(2026);
    int hata = 0;
    {
        KitapListesi k;
        for (int adim = 0; adim < 20000; adim++) {
            const int islem = std::rand() % 100;
            const int id = 1 + std::rand() % 150;
            const Kitap kitap{id, "A", "B", 10, false, 0};
            if (islem < 25) k.basaEkle(kitap);
            else if (islem < 50) k.sonaEkle(kitap);
            else if (islem < 65) k.oncesineEkle(1 + std::rand() % 150, kitap);
            else if (islem < 92) k.sil(id);
            else if (islem < 99) k.durumDegistir(id, std::rand() % 2 == 0);
            else { for (int i = 1; i <= 150; i++) k.durumDegistir(i, false); k.temizle(); }

            int sayilan = 0;
            for (int i = 1; i <= 150; i++) {
                bool d = false, h = false;
                k.dogrusalAramaAdimi(i, d);
                k.hashAramaAdimi(i, h);
                if (d != h) hata++;
                if (d) sayilan++;
                const KitapDugum* p = k.bul(i);
                if (d && (p == nullptr || p->veri.id != i)) hata++;
            }
            if (sayilan != k.boyut()) hata++;
        }
        std::cout << "Son boyut: " << k.boyut() << '\n';
    }
    std::cout << "20000 islem, hata: " << hata << ", canli dugum: " << canliDugum << '\n';
    return (hata == 0 && canliDugum == 0) ? 0 : 1;
}
