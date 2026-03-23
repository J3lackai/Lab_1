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
        Encryptor.h\
        run_tests.cpp \


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    Encryptor.h \
    openssl/include/openssl/__DECC_INCLUDE_EPILOGUE.H \
    openssl/include/openssl/__DECC_INCLUDE_PROLOGUE.H \
    openssl/include/openssl/aes.h \
    openssl/include/openssl/asn1.h \
    openssl/include/openssl/asn1err.h \
    openssl/include/openssl/asn1t.h \
    openssl/include/openssl/async.h \
    openssl/include/openssl/asyncerr.h \
    openssl/include/openssl/bio.h \
    openssl/include/openssl/bioerr.h \
    openssl/include/openssl/blowfish.h \
    openssl/include/openssl/bn.h \
    openssl/include/openssl/bnerr.h \
    openssl/include/openssl/buffer.h \
    openssl/include/openssl/buffererr.h \
    openssl/include/openssl/byteorder.h \
    openssl/include/openssl/camellia.h \
    openssl/include/openssl/cast.h \
    openssl/include/openssl/cmac.h \
    openssl/include/openssl/cmp.h \
    openssl/include/openssl/cmp_util.h \
    openssl/include/openssl/cmperr.h \
    openssl/include/openssl/cms.h \
    openssl/include/openssl/cmserr.h \
    openssl/include/openssl/comp.h \
    openssl/include/openssl/comperr.h \
    openssl/include/openssl/conf.h \
    openssl/include/openssl/conf_api.h \
    openssl/include/openssl/conferr.h \
    openssl/include/openssl/configuration.h \
    openssl/include/openssl/conftypes.h \
    openssl/include/openssl/core.h \
    openssl/include/openssl/core_dispatch.h \
    openssl/include/openssl/core_names.h \
    openssl/include/openssl/core_object.h \
    openssl/include/openssl/crmf.h \
    openssl/include/openssl/crmferr.h \
    openssl/include/openssl/crypto.h \
    openssl/include/openssl/cryptoerr.h \
    openssl/include/openssl/cryptoerr_legacy.h \
    openssl/include/openssl/ct.h \
    openssl/include/openssl/cterr.h \
    openssl/include/openssl/decoder.h \
    openssl/include/openssl/decodererr.h \
    openssl/include/openssl/des.h \
    openssl/include/openssl/dh.h \
    openssl/include/openssl/dherr.h \
    openssl/include/openssl/dsa.h \
    openssl/include/openssl/dsaerr.h \
    openssl/include/openssl/dtls1.h \
    openssl/include/openssl/e_os2.h \
    openssl/include/openssl/e_ostime.h \
    openssl/include/openssl/ebcdic.h \
    openssl/include/openssl/ec.h \
    openssl/include/openssl/ecdh.h \
    openssl/include/openssl/ecdsa.h \
    openssl/include/openssl/ecerr.h \
    openssl/include/openssl/encoder.h \
    openssl/include/openssl/encodererr.h \
    openssl/include/openssl/engine.h \
    openssl/include/openssl/engineerr.h \
    openssl/include/openssl/err.h \
    openssl/include/openssl/ess.h \
    openssl/include/openssl/esserr.h \
    openssl/include/openssl/evp.h \
    openssl/include/openssl/evperr.h \
    openssl/include/openssl/fips_names.h \
    openssl/include/openssl/fipskey.h \
    openssl/include/openssl/hmac.h \
    openssl/include/openssl/hpke.h \
    openssl/include/openssl/http.h \
    openssl/include/openssl/httperr.h \
    openssl/include/openssl/idea.h \
    openssl/include/openssl/indicator.h \
    openssl/include/openssl/kdf.h \
    openssl/include/openssl/kdferr.h \
    openssl/include/openssl/lhash.h \
    openssl/include/openssl/macros.h \
    openssl/include/openssl/md2.h \
    openssl/include/openssl/md4.h \
    openssl/include/openssl/md5.h \
    openssl/include/openssl/mdc2.h \
    openssl/include/openssl/ml_kem.h \
    openssl/include/openssl/modes.h \
    openssl/include/openssl/obj_mac.h \
    openssl/include/openssl/objects.h \
    openssl/include/openssl/objectserr.h \
    openssl/include/openssl/ocsp.h \
    openssl/include/openssl/ocsperr.h \
    openssl/include/openssl/opensslconf.h \
    openssl/include/openssl/opensslv.h \
    openssl/include/openssl/ossl_typ.h \
    openssl/include/openssl/param_build.h \
    openssl/include/openssl/params.h \
    openssl/include/openssl/pem.h \
    openssl/include/openssl/pem2.h \
    openssl/include/openssl/pemerr.h \
    openssl/include/openssl/pkcs12.h \
    openssl/include/openssl/pkcs12err.h \
    openssl/include/openssl/pkcs7.h \
    openssl/include/openssl/pkcs7err.h \
    openssl/include/openssl/prov_ssl.h \
    openssl/include/openssl/proverr.h \
    openssl/include/openssl/provider.h \
    openssl/include/openssl/quic.h \
    openssl/include/openssl/rand.h \
    openssl/include/openssl/randerr.h \
    openssl/include/openssl/rc2.h \
    openssl/include/openssl/rc4.h \
    openssl/include/openssl/rc5.h \
    openssl/include/openssl/ripemd.h \
    openssl/include/openssl/rsa.h \
    openssl/include/openssl/rsaerr.h \
    openssl/include/openssl/safestack.h \
    openssl/include/openssl/seed.h \
    openssl/include/openssl/self_test.h \
    openssl/include/openssl/sha.h \
    openssl/include/openssl/srp.h \
    openssl/include/openssl/srtp.h \
    openssl/include/openssl/ssl.h \
    openssl/include/openssl/ssl2.h \
    openssl/include/openssl/ssl3.h \
    openssl/include/openssl/sslerr.h \
    openssl/include/openssl/sslerr_legacy.h \
    openssl/include/openssl/stack.h \
    openssl/include/openssl/store.h \
    openssl/include/openssl/storeerr.h \
    openssl/include/openssl/symhacks.h \
    openssl/include/openssl/thread.h \
    openssl/include/openssl/tls1.h \
    openssl/include/openssl/trace.h \
    openssl/include/openssl/ts.h \
    openssl/include/openssl/tserr.h \
    openssl/include/openssl/txt_db.h \
    openssl/include/openssl/types.h \
    openssl/include/openssl/ui.h \
    openssl/include/openssl/uierr.h \
    openssl/include/openssl/whrlpool.h \
    openssl/include/openssl/x509.h \
    openssl/include/openssl/x509_acert.h \
    openssl/include/openssl/x509_vfy.h \
    openssl/include/openssl/x509err.h \
    openssl/include/openssl/x509v3.h \
    openssl/include/openssl/x509v3err.h \
    TestEncryptor.h

DISTFILES += \
    openssl/lib/libcrypto.lib \
    openssl/lib/libssl.lib \
    _CRT_SECURE_NO_WARNINGS

