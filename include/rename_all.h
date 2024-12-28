#ifndef RENAME_H
#define RENAME_H

#include <stdbool.h> // Boolean veri tipi için

// Basit kopyalama fonksiyonu
// Kaynak dosyayı hedef dosyaya kopyalar.
// Başarılı olursa true, aksi takdirde false döner.
bool copy(const char *source, const char *dest);

// Taşıma veya yeniden adlandırma işlemi
// Kaynak dosyayı hedef dosyaya taşır veya yeniden adlandırır.
// Başarılı olursa true, aksi takdirde false döner.
bool do_move(const char *source, const char *dest);

#endif // RENAME_H
