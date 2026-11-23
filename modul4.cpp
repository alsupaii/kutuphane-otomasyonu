#include "modul4.h"
#include "modul2.h"
#include "ortak.h"

#include <iomanip>
#include <iostream>
#include <limits>

static const int sonsuz = std::numeric_limits<int>::max();

// Disaridan gelen birim indisi 0..birimSayisi-1 araliginda mi? Menu 1..6 okur ve 1 cikarir;
// cikarmayi unutan bir cagri dizinin disina yazmasin diye her giris noktasi bunu sorar.
static bool gecerliBirim(int indis) { return indis >= 0 && indis < birimSayisi; }

std::string KampusGrafi::birimAdi(int indis) {
    if (!gecerliBirim(indis)) {
        return "(gecersiz birim)";
    }
    static const char* const adlar[birimSayisi] = {
        "Merkez Kutuphane",       "MYO Bilgisayar Lab.", "Muhendislik Binasi",
        "Ogrenci Isleri",         "Yemekhane ve Sosyal Alan", "Konferans Salonu"};
    return adlar[indis];
}

// Yonsuz graf: a'dan b'ye yol varsa b'den a'ya da vardir, matris simetriktir.
void KampusGrafi::kenarEkle(int a, int b, int mesafe) {
    matris[a][b] = mesafe;
    matris[b][a] = mesafe;
}

KampusGrafi::KampusGrafi() {
    for (int i = 0; i < birimSayisi; i++) {
        for (int j = 0; j < birimSayisi; j++) {
            matris[i][j] = 0;
        }
    }
    // Indisler: 0 Kutuphane, 1 MYO Lab, 2 Muhendislik, 3 Ogrenci Isleri,
    //           4 Yemekhane, 5 Konferans
    kenarEkle(0, 1, 120);
    kenarEkle(0, 2, 300);
    kenarEkle(0, 3, 200);
    kenarEkle(1, 2, 150);
    kenarEkle(1, 4, 400);
    kenarEkle(2, 5, 250);
    kenarEkle(3, 4, 180);
    kenarEkle(3, 5, 500);
    kenarEkle(4, 5, 220);
}

void KampusGrafi::birimleriListele() const {
    std::cout << "Kampus birimleri:\n";
    for (int i = 0; i < birimSayisi; i++) {
        std::cout << "  " << i + 1 << ") " << birimAdi(i) << '\n';
    }
}

// O(V^2): matrisin her hucresi bir kez yazilir.
void KampusGrafi::matrisYazdir() const {
    birimleriListele();
    std::cout << "\nKomsuluk matrisi (metre, '-' = dogrudan yol yok):\n     ";
    for (int j = 0; j < birimSayisi; j++) {
        std::cout << std::right << std::setw(6) << j + 1;
    }
    std::cout << '\n';
    for (int i = 0; i < birimSayisi; i++) {
        std::cout << std::right << std::setw(5) << i + 1;
        for (int j = 0; j < birimSayisi; j++) {
            if (i == j) {
                std::cout << std::setw(6) << 0;
            } else if (matris[i][j] == 0) {
                std::cout << std::setw(6) << "-";
            } else {
                std::cout << std::setw(6) << matris[i][j];
            }
        }
        std::cout << '\n';
    }
    std::cout << std::left;
}

// BFS: once baslangicin tum komsulari, sonra komsularin komsulari.
// Kuyruk kullanir. Her dugum kuyruga en fazla bir kez girer, bu yuzden
// birimSayisi boyutlu dizi yeter. Matriste O(V^2).
void KampusGrafi::bfs(int baslangic) const {
    if (!gecerliBirim(baslangic)) {
        return;
    }
    bool ziyaretEdildi[birimSayisi] = {false};
    int kuyruk[birimSayisi];
    int on = 0;
    int arka = 0;

    ziyaretEdildi[baslangic] = true;
    kuyruk[arka++] = baslangic;

    std::cout << "BFS sirasi: ";
    while (on < arka) {
        const int dugum = kuyruk[on++];
        std::cout << birimAdi(dugum);
        for (int komsu = 0; komsu < birimSayisi; komsu++) {
            if (matris[dugum][komsu] > 0 && !ziyaretEdildi[komsu]) {
                ziyaretEdildi[komsu] = true;  // Kuyruga girerken isaretle, iki kez girmesin.
                kuyruk[arka++] = komsu;
            }
        }
        if (on < arka) {
            std::cout << " -> ";
        }
    }
    std::cout << '\n';
}

// DFS: bir yoldan gidebildigi kadar derine gider, cikmaz sokakta geri doner.
// Ozyineleme (recursion) cagri yiginini kullanir. Matriste O(V^2).
void KampusGrafi::dfsYardimci(int dugum, bool ziyaretEdildi[], bool& ilk) const {
    ziyaretEdildi[dugum] = true;
    std::cout << (ilk ? "" : " -> ") << birimAdi(dugum);
    ilk = false;
    for (int komsu = 0; komsu < birimSayisi; komsu++) {
        if (matris[dugum][komsu] > 0 && !ziyaretEdildi[komsu]) {
            dfsYardimci(komsu, ziyaretEdildi, ilk);
        }
    }
}

void KampusGrafi::dfs(int baslangic) const {
    if (!gecerliBirim(baslangic)) {
        return;
    }
    bool ziyaretEdildi[birimSayisi] = {false};
    bool ilk = true;
    std::cout << "DFS sirasi: ";
    dfsYardimci(baslangic, ziyaretEdildi, ilk);
    std::cout << '\n';
}

// Dijkstra: baslangictan her dugume en kisa mesafe.
// Her turda kesinlesmemis en yakin dugumu secer ve komsularini gunceller (gevsetme).
// Dizi ile secim yapildigi icin O(V^2). Negatif agirlik olmamali.
int KampusGrafi::dijkstra(int baslangic, int hedef) const {
    if (!gecerliBirim(baslangic) || !gecerliBirim(hedef)) {
        return -1;
    }
    int mesafe[birimSayisi];
    int onceki[birimSayisi];       // Rotayi geri kurmak icin: hangi dugumden gelindi
    bool kesin[birimSayisi];       // En kisa mesafesi kesinlesti mi
    for (int i = 0; i < birimSayisi; i++) {
        mesafe[i] = sonsuz;
        onceki[i] = -1;
        kesin[i] = false;
    }
    mesafe[baslangic] = 0;

    for (int tur = 0; tur < birimSayisi; tur++) {
        // 1) Kesinlesmemis dugumler icinde mesafesi en kucuk olani bul.
        int u = -1;
        for (int v = 0; v < birimSayisi; v++) {
            if (!kesin[v] && mesafe[v] != sonsuz && (u == -1 || mesafe[v] < mesafe[u])) {
                u = v;
            }
        }
        if (u == -1) {
            break;  // Kalan dugumlere ulasilamiyor.
        }
        kesin[u] = true;
        if (u == hedef) {
            break;  // Hedef kesinlesti, devam etmeye gerek yok.
        }
        // 2) u uzerinden gitmek komsulara daha kisa yol veriyor mu?
        for (int v = 0; v < birimSayisi; v++) {
            if (matris[u][v] > 0 && !kesin[v] && mesafe[u] + matris[u][v] < mesafe[v]) {
                mesafe[v] = mesafe[u] + matris[u][v];
                onceki[v] = u;
            }
        }
    }

    if (mesafe[hedef] == sonsuz) {
        std::cout << birimAdi(baslangic) << " -> " << birimAdi(hedef) << ": rota yok.\n";
        return -1;
    }

    // Rota hedeften geriye kurulur (onceki zinciri), sonra ters yazdirilir.
    int rota[birimSayisi];
    int uzunluk = 0;
    for (int d = hedef; d != -1; d = onceki[d]) {
        rota[uzunluk++] = d;
    }
    std::cout << "En kisa rota: ";
    for (int i = uzunluk - 1; i >= 0; i--) {
        std::cout << birimAdi(rota[i]) << (i > 0 ? " -> " : "\n");
    }
    std::cout << "Toplam mesafe: " << mesafe[hedef] << " metre\n";
    return mesafe[hedef];
}

// ---------------------------------------------------------------
// Menu
// ---------------------------------------------------------------

// Kullanici 1..6 girer, program 0..5 indisini kullanir.
static int birimSec(const std::string& mesaj) {
    return tamSayiOku(mesaj + " (1-" + std::to_string(birimSayisi) + "): ", 1, birimSayisi) - 1;
}

void modul4Menu(const KampusGrafi& kampus, IslemYigini& gecmis) {
    int secim = -1;
    while (secim != 0) {
        std::cout << "\n=== MODUL 4: KAMPUS ROTALARI (GRAF) ===\n"
                  << "  1) Birimleri listele\n"
                  << "  2) Komsuluk matrisini yazdir\n"
                  << "  3) BFS (genislik oncelikli gezinme)\n"
                  << "  4) DFS (derinlik oncelikli gezinme)\n"
                  << "  5) Dijkstra en kisa rota\n"
                  << "  0) Ana menuye don\n";
        secim = tamSayiOku("Seciminiz: ", 0, 5);

        switch (secim) {
        case 1:
            kampus.birimleriListele();
            break;
        case 2:
            kampus.matrisYazdir();
            break;
        case 3:
            kampus.birimleriListele();
            kampus.bfs(birimSec("Baslangic birimi"));
            gecmis.push("BFS calisti");
            break;
        case 4:
            kampus.birimleriListele();
            kampus.dfs(birimSec("Baslangic birimi"));
            gecmis.push("DFS calisti");
            break;
        case 5: {
            kampus.birimleriListele();
            const int bas = birimSec("Baslangic birimi");
            const int hedef = birimSec("Hedef birim");
            const int m = kampus.dijkstra(bas, hedef);
            gecmis.push("Rota sorgusu: " + std::to_string(bas + 1) + " -> " +
                        std::to_string(hedef + 1) +
                        (m >= 0 ? " (" + std::to_string(m) + " m)" : " (rota yok)"));
            break;
        }
        default:
            break;
        }
    }
}
