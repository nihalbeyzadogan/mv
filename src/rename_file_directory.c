#include <stdio.h> // Standart giriş/çıkış kütüphanesi
#include <stdlib.h> // Standart kütüphane, genel amaçlı fonksiyonlar için
#include <stdbool.h> // Boolean türlerini kullanmak için
#include <string.h> // String işlemleri için
#include <sys/stat.h> // Dosya ve dizin durumlarını kontrol etmek için
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

// Taşıma veya yeniden adlandırma işlemi
static bool do_move(const char *source, const char *dest) {
    if (is_directory(source)) { // Eğer kaynak bir dizinse kontrol et
        if (rename(source, dest) == 0) { // Dizin taşıma/yeniden adlandırma işlemini dene
            return true; // İşlem başarılı
        } else {
            perror("Directory rename failed"); // Hata durumunda mesaj yazdır
            return false; // İşlem başarısız
        }
    } else { // Eğer kaynak bir dosyaysa
        if (rename(source, dest) == 0) { // Dosyayı yeniden adlandır veya taşı
            return true; // İşlem başarılı
        }

        if (copy(source, dest)) { // Eğer `rename` başarısız olursa dosyayı kopyala
            if (remove(source) != 0) { // Kaynak dosyayı sil
                perror("Source file could not be deleted"); // Hata durumunda mesaj yazdır
                return false; // İşlem başarısız
            }
            return true; // İşlem başarılı
        }
    }

    return false; // İşlem başarısız
}

int main(int argc, char *argv[]) {
    if (argc != 3) { // Kullanıcıdan tam 3 argüman bekleniyor
        fprintf(stderr, "Usage: %s <source> <destination>\n", argv[0]); // Yanlış kullanım mesajı
        return EXIT_FAILURE; // Başarısız çıkış
    }

    const char *source = argv[1]; // Kaynak dosya veya dizin adı
    const char *dest = argv[2]; // Hedef dosya veya dizin adı

    if (access(source, F_OK) != 0) { // Kaynak dosya/dizinin varlığını kontrol et
        perror("Source not found"); // Hata mesajı yazdır
        return EXIT_FAILURE; // Başarısız çıkış
    }

    if (access(dest, F_OK) == 0) { // Hedef dosya/dizinin varlığını kontrol et
        fprintf(stderr, "Destination already exists: %s\n", dest); // Hata mesajı
        return EXIT_FAILURE; // Başarısız çıkış
    }

    if (is_directory(source)) { // Kaynak bir dizin mi kontrol et
        printf("Attempting to rename/move directory...\n"); // Bilgilendirme mesajı
    } else { // Kaynak bir dosya
        printf("Attempting to rename/move file...\n"); // Bilgilendirme mesajı
    }

    if (do_move(source, dest)) { // Taşıma veya yeniden adlandırma işlemini gerçekleştir
        printf("'%s' successfully moved or renamed to '%s'.\n", source, dest); // Başarı mesajı
        return EXIT_SUCCESS; // Başarılı çıkış
    } else {
        fprintf(stderr, "Operation failed.\n"); // Hata mesajı
        return EXIT_FAILURE; // Başarısız çıkış
    }
}
