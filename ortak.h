#ifndef ORTAK_H
#define ORTAK_H

#include <string>

// Girdi akisi bittiginde (Ctrl+Z veya dosya sonu) firlatilir.
// main() bunu yakalar. Boylece yikicilar calisir ve bellek temizlenir.
struct GirdiBitti {};

// Kullanicidan enAz..enCok araliginda bir tam sayi okur.
// Gecersiz girdide uyari verir ve yeniden sorar ("12abc", "x", bos satir gecersiz).
int tamSayiOku(const std::string& mesaj, int enAz, int enCok);

// Bos olmayan bir metin satiri okur.
// '|' karakterine izin vermez, cunku katalog dosyasinda alan ayiraci '|'.
std::string metinOku(const std::string& mesaj);

#endif
