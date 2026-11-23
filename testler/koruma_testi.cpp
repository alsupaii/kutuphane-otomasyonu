// Poka-yoke korumalarinin testi: negatif ID hash kovasi, katalog surumu,
// oduncte kitap varken dosyadan yukleme (-2), gecersiz graf birim indisi.
#include "modul1.h"
#include "modul4.h"
#include <iostream>
int main() {
    int hata = 0;
    {
        KitapListesi k;
        const int s0 = k.surum();
        k.sonaEkle(Kitap{-5, "Negatif", "ID", 10, false, 0, ""});   // kovaNo(-5) dizi disina tasmamali
        k.sonaEkle(Kitap{-36, "Negatif2", "ID", 10, false, 0, ""});
        bool b = false;
        k.hashAramaAdimi(-5, b); if (!b) hata++;
        k.hashAramaAdimi(-36, b); if (!b) hata++;
        k.temizle();
        if (k.surum() == s0) hata++;                            // temizle surumu artirmali
        k.sonaEkle(Kitap{1, "A", "B", 10, true, 0, ""});
        int atlanan = 0;
        if (k.dosyadanYukle("yok.txt", atlanan) != -2) hata++;  // oduncte varken -2
        if (k.boyut() != 1) hata++;                             // katalog degismemeli
        if (k.bosalt() != -2 || k.boyut() != 1) hata++;          // oduncte varken bosalt reddeder
        k.durumDegistir(1, false);
    }
    const KampusGrafi g;
    if (g.dijkstra(0, 6) != -1) hata++;
    if (g.dijkstra(-1, 0) != -1) hata++;
    g.bfs(6); g.dfs(-1);
    if (KampusGrafi::birimAdi(6) != "(gecersiz birim)") hata++;
    std::cout << "\nkoruma testi hata: " << hata << ", canli dugum: " << canliDugum << '\n';
    return hata;
}
