#include "ortak.h"

#include <iostream>
#include <sstream>

int canliDugum = 0;
int canliDizi = 0;

// Bir satir okur. Windows'tan gelen sondaki '\r' karakterini atar.
// Girdi bittiyse GirdiBitti firlatir.
static std::string satirOku() {
    std::string satir;
    if (!std::getline(std::cin, satir)) {
        throw GirdiBitti();
    }
    if (!satir.empty() && satir[satir.size() - 1] == '\r') {
        satir.erase(satir.size() - 1);
    }
    return satir;
}

int tamSayiOku(const std::string& mesaj, int enAz, int enCok) {
    while (true) {
        std::cout << mesaj;
        const std::string satir = satirOku();

        std::istringstream akis(satir);
        int deger = 0;
        char fazla = '\0';
        // Sayi okunabilmeli VE arkasinda baska karakter kalmamali.
        const bool sayiMi = (akis >> deger) && !(akis >> fazla);

        if (!sayiMi) {
            std::cout << "Gecersiz giris. Lutfen bir tam sayi girin.\n";
        } else if (deger < enAz || deger > enCok) {
            std::cout << "Gecersiz secim. " << enAz << " ile " << enCok
                      << " arasinda bir sayi girin.\n";
        } else {
            return deger;
        }
    }
}

std::string metinOku(const std::string& mesaj) {
    while (true) {
        std::cout << mesaj;
        const std::string satir = satirOku();

        if (satir.find_first_not_of(" \t") == std::string::npos) {
            std::cout << "Bos birakilamaz.\n";
        } else if (satir.find('|') != std::string::npos) {
            std::cout << "'|' karakteri kullanilamaz (dosyada ayirac olarak kullaniliyor).\n";
        } else {
            return satir;
        }
    }
}
