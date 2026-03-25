QT = core
QT += testlib gui widgets
CONFIG += static c++17 cmdline
LIBS += -lws2_32 -lgdi32 -lcrypt32 -luser32
LIBS += $$PWD/openssl/lib/libcrypto.lib \
        $$PWD/openssl/lib/libssl.lib
INCLUDEPATH = $$PWD/openssl/include
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
SOURCES += \
        Encryptor.cpp \
        main.cpp \
        openssl/include/openssl/applink.c \
        run_tests.cpp \
    tests/run_tests.cpp


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# Оставлены только те заголовки, которые явно используются в Encryptor.cpp: aes, evp, rand, hmac.
# Остальные удалены, так как они не нужны для AES-шифрования файлов.
HEADERS += \
    Encryptor.h \
    TestEncryptor.h \
    openssl/include/openssl/aes.h \
    tests/TestEncryptor.h
    openssl/include/openssl/asn1.h
    openssl/include/openssl/bio.h
    openssl/include/openssl/cmac.h  # Часто используется внутри EVP, если нет полной сборки
    openssl/include/openssl/crypto.h
    openssl/include/openssl/ecdh.h
    openssl/include/openssl/engine.h
    openssl/include/openssl/hmac.h  # Критично для проверки целостности данных
    openssl/include/openssl/modes.h # Необходим для режима CBC
    openssl/include/openssl/rand.h  # Для генерации IV и соли (RAND_bytes)
    openssl/include/openssl/sha.h
    openssl/include/openssl/sslerr_legacy.h
    openssl/include/openssl/types.h


DISTFILES += \
    openssl/lib/libcrypto.lib \
    openssl/lib/libssl.lib \
    _CRT_SECURE_NO_WARNINGS

