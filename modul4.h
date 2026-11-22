#ifndef MODUL4_H
#define MODUL4_H

// MODUL 4: Komsuluk matrisli agirlikli graf, BFS, DFS ve Dijkstra.

#include <string>

class IslemYigini;

const int birimSayisi = 6;

class KampusGrafi {
public:
    KampusGrafi();  // Ornek kampus modelini yukler.

    void birimleriListele() const;
    void matrisYazdir() const;
    void bfs(int baslangic) const;
    void dfs(int baslangic) const;
    // En kisa mesafeyi dondurur. Ulasilamiyorsa -1.
    int dijkstra(int baslangic, int hedef) const;

    static std::string birimAdi(int indis);

private:
    // matris[i][j] = i ile j arasi yurume mesafesi (metre). 0 = dogrudan yol yok.
    int matris[birimSayisi][birimSayisi];

    void kenarEkle(int a, int b, int mesafe);
    void dfsYardimci(int dugum, bool ziyaretEdildi[], bool& ilk) const;
};

void modul4Menu(const KampusGrafi& kampus, IslemYigini& gecmis);

#endif
