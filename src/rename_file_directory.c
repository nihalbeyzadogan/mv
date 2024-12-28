#include <stdio.h> // Standart giriş/çıkış kütüphanesi
#include <stdlib.h> // Standart kütüphane, genel amaçlı fonksiyonlar için
#include <stdbool.h> // Boolean türlerini kullanmak için
#include <string.h> // String işlemleri için
#include <sys/stat.h> // Dosya durumunu kontrol etmek için
#include <unistd.h> // UNIX sistem çağrıları, örn. access() ve remove()
#include <errno.h> // Hata numaralarını ve hata mesajlarını işlemek için

// Bir yolun dizin olup olmadığını kontrol eden fonksiyon
bool is_directory(const char *path) {
    struct stat path_stat; // Dosya/dizin durum bilgilerini tutmak için struct
    if (stat(path, &path_stat) != 0) { // Dosya/dizinin durumunu al
        perror("Stat failed"); // Hata durumunda mesaj yazdır
        return false; // İşlem başarısız
    }
    return S_ISDIR(path_stat.st_mode); // Eğer bir dizinse true döner
}

// Basit kopyalama fonksiyonu
bool copy(const char *source, const char *dest) {
    FILE *src = fopen(source, "rb"); // Kaynak dosyayı okuma modunda aç
    if (!src) {
        perror("Source file could not be opened"); // Hata durumunda mesaj yazdır
        return false; // İşlem başarısız
    }

    FILE *dst = fopen(dest, "wb"); // Hedef dosyayı yazma modunda aç
    if (!dst) {
        perror("Destination file could not be opened"); // Hata durumunda mesaj yazdır
        fclose(src); // Kaynak dosyayı kapat
        return false; // İşlem başarısız
    }

    char buffer[4096]; // Verileri taşımak için tampon bellek
    size_t bytes; // Okunan bayt sayısını tutmak için
    while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0) { // Kaynak dosyadan oku
        if (fwrite(buffer, 1, bytes, dst) != bytes) { // Hedef dosyaya yaz
            perror("Write error"); // Hata durumunda mesaj yazdır
            fclose(src); // Kaynak dosyayı kapat
            fclose(dst); // Hedef dosyayı kapat
            return false; // İşlem başarısız
        }
    }

    fclose(src); // Kaynak dosyayı kapat
    fclose(dst); // Hedef dosyayı kapat
    return true; // İşlem başarılı
}

// Taşıma veya yeniden adlandırma işlemi???????????????????????????? silinebilir ????????????????????????
static bool do_move(const char *source, const char *dest) {
    // 1. `rename` sistem çağrısı
    if (rename(source, dest) == 0) { // Dosyayı yeniden adlandır veya taşı
        return true; // İşlem başarılı
    }

    // 2. Eğer `rename` başarısız olursa, kopyalama işlemi devreye girer
    if (copy(source, dest)) { // Kaynak dosyayı hedefe kopyala
        if (remove(source) != 0) { // Kaynak dosyayı sil
            perror("Source file could not be deleted"); // Hata durumunda mesaj yazdır
            return false; // İşlem başarısız
        }
        return true; // İşlem başarılı
    }

    return false; // İşlem başarısız
}

int main(int argc, char *argv[]) {
    if (argc != 3) { // Kullanıcıdan tam 3 argüman bekleniyor
        fprintf(stderr, "Usage: %s <source> <destination>\n", argv[0]); // Yanlış kullanım mesajı
        return EXIT_FAILURE; // Başarısız çıkış
    }

    const char *source = argv[1]; // Kaynak dosya adı
    const char *dest = argv[2]; // Hedef dosya adı

    // Kaynak dosyanın varlığını kontrol et
    if (access(source, F_OK) != 0) { // Dosya yoksa hata döner
        perror("Source file not found"); // Hata mesajı yazdır
        return EXIT_FAILURE; // Başarısız çıkış
    }

    // Hedef dosyanın üzerine yazmadan önce kontrol
    if (access(dest, F_OK) == 0) { // Hedef dosya zaten mevcut mu?
        fprintf(stderr, "Destination file already exists: %s\n", dest); // Hata mesajı
        return EXIT_FAILURE; // Başarısız çıkış
    }

    // Taşıma/yeniden adlandırma işlemini gerçekleştir
    if (do_move(source, dest)) { // İşlem başarılı mı?
        printf("File '%s' successfully moved or renamed to '%s'.\n", source, dest); // Başarı mesajı
        return EXIT_SUCCESS; // Başarılı çıkış
    } else {
        fprintf(stderr, "File could not be moved or renamed.\n"); // Hata mesajı
        return EXIT_FAILURE; // Başarısız çıkış
    }
}
