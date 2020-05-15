#include <jni.h>

#include "kSerial.h"

#include <android/log.h>
#define LOG_TAG     "KS_LIB"
#define LOGD(...)   __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...)   __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

extern "C"
{

#define MAX_KSERIAL_PACKET_LENS     (1024)
uint8_t kspackbuf[1032] = {0};
uint32_t kspacketcnt = 0;
kserial_packet_t kspacket[MAX_KSERIAL_PACKET_LENS] = {0};

JNIEXPORT jbyteArray
Java_com_kitsprout_ks_KSerial_pack(
        JNIEnv* env, jclass clazz, jintArray jparam, jint jtype, jint jlens, jdoubleArray jdata) {
    uint8_t param[2] = {0};
    uint32_t type = (uint32_t) jtype;
    uint32_t lens = (uint32_t) jlens;
    uint32_t packetTotalBytes;
    int *pkparam = env->GetIntArrayElements(jparam, nullptr);
    double *pkdata = env->GetDoubleArrayElements(jdata, nullptr);

    param[0] = (uint8_t) pkparam[0];
    param[1] = (uint8_t) pkparam[1];

    switch (type) {
        case KS_I8: {
            int8_t data[1024];
            for (uint32_t i = 0; i < lens; i++) {
                data[i] = (int8_t) pkdata[i];
            }
            packetTotalBytes = kSerial_Pack(kspackbuf, param, type, lens, data);
            break;
        }
        case KS_I16: {
            int16_t data[512];
            for (uint32_t i = 0; i < lens; i++) {
                data[i] = (int16_t) pkdata[i];
            }
            packetTotalBytes = kSerial_Pack(kspackbuf, param, type, lens, data);
            break;
        }
        case KS_I32: {
            int32_t data[256];
            for (uint32_t i = 0; i < lens; i++) {
                data[i] = (int32_t) pkdata[i];
            }
            packetTotalBytes = kSerial_Pack(kspackbuf, param, type, lens, data);
            break;
        }
        case KS_I64: {
            int64_t data[128];
            for (uint32_t i = 0; i < lens; i++) {
                data[i] = (int64_t) pkdata[i];
            }
            packetTotalBytes = kSerial_Pack(kspackbuf, param, type, lens, data);
            break;
        }
        case KS_R0:
        case KS_R1:
        case KS_R2:
        case KS_R3:
        case KS_R4:
        case KS_U8: {
            uint8_t data[1024];
            for (uint32_t i = 0; i < lens; i++) {
                data[i] = (uint8_t) pkdata[i];
            }
            packetTotalBytes = kSerial_Pack(kspackbuf, param, type, lens, data);
            break;
        }
        case KS_U16: {
            uint16_t data[512];
            for (uint32_t i = 0; i < lens; i++) {
                data[i] = (uint16_t) pkdata[i];
            }
            packetTotalBytes = kSerial_Pack(kspackbuf, param, type, lens, data);
            break;
        }
        case KS_U32: {
            uint32_t data[256];
            for (uint32_t i = 0; i < lens; i++) {
                data[i] = (uint32_t) pkdata[i];
            }
            packetTotalBytes = kSerial_Pack(kspackbuf, param, type, lens, data);
            break;
        }
        case KS_U64: {
            uint64_t data[128];
            for (uint32_t i = 0; i < lens; i++) {
                data[i] = (uint64_t) pkdata[i];
            }
            packetTotalBytes = kSerial_Pack(kspackbuf, param, type, lens, data);
            break;
        }
//        case KS_F16: {
//            half data[512];
//            for (uint32_t i = 0; i < lens; i++) {
//                data[i] = (half) pkdata[i];
//            }
//            break;
//        }
        case KS_F32: {
            float data[256];
            for (uint32_t i = 0; i < lens; i++) {
                data[i] = (float) pkdata[i];
            }
            packetTotalBytes = kSerial_Pack(kspackbuf, param, type, lens, data);
            break;
        }
        case KS_F64: {
            double data[128];
            for (uint32_t i = 0; i < lens; i++) {
                data[i] = pkdata[i];
            }
            packetTotalBytes = kSerial_Pack(kspackbuf, param, type, lens, data);
            break;
        }
        default: {
            uint8_t data[1024];
            for (uint32_t i = 0; i < lens; i++) {
                data[i] = 0;
            }
            packetTotalBytes = kSerial_Pack(kspackbuf, param, type, lens, data);
            break;
        }
    }
    jbyteArray jpacket = env->NewByteArray(packetTotalBytes);
    env->SetByteArrayRegion(jpacket, 0, packetTotalBytes, (jbyte*)kspackbuf);

    return jpacket;
}

JNIEXPORT jint
Java_com_kitsprout_ks_KSerial_unpackBuffer(
        JNIEnv* env, jclass clazz, jbyteArray jbuf, jint jlens) {
    const uint32_t pkbufsize = (uint32_t) jlens;
    uint8_t *pkbuf = (uint8_t *)env->GetByteArrayElements(jbuf, nullptr);
    uint32_t newindex = kSerial_UnpackBuffer(pkbuf, pkbufsize, kspacket, &kspacketcnt);
    return newindex;
}

JNIEXPORT jint
Java_com_kitsprout_ks_KSerial_getPacketCount(
        JNIEnv* env, jclass clazz) {
    return kspacketcnt;
}

JNIEXPORT jint
Java_com_kitsprout_ks_KSerial_getPacketType(
        JNIEnv* env, jclass clazz, jint index) {
    return kspacket[index].type;
}

JNIEXPORT jint
Java_com_kitsprout_ks_KSerial_getPacketBytes(
        JNIEnv* env, jclass clazz, jint index) {

    return kspacket[index].nbyte;
}

JNIEXPORT jintArray
Java_com_kitsprout_ks_KSerial_getPacketParam(
        JNIEnv* env, jclass clazz, jint index) {
    jintArray jparam = env->NewIntArray(2);
    int param[2];
    param[0] = kspacket[index].param[0];
    param[1] = kspacket[index].param[1];
    env->SetIntArrayRegion(jparam, 0, 2, param);
    return jparam;
}

JNIEXPORT jdoubleArray
Java_com_kitsprout_ks_KSerial_getPacketData(
        JNIEnv* env, jclass clazz, jint index) {

    jdoubleArray jdata = env->NewDoubleArray(kspacket[index].lens);
    double data[kspacket[index].lens];
    switch (kspacket[index].type) {
        case KS_I8: {
            int8_t pdata[kspacket[index].lens];
            kSerial_GetPacketData(kspacket, pdata, (uint32_t)index);
            for (uint32_t i = 0; i < kspacket[index].lens; i++) {
                data[i] = (double)pdata[i];
            }
            break;
        }
        case KS_I16: {
            int16_t pdata[kspacket[index].lens];
            kSerial_GetPacketData(kspacket, pdata, (uint32_t)index);
            for (uint32_t i = 0; i < kspacket[index].lens; i++) {
                data[i] = (double)pdata[i];
            }
            break;
        }
        case KS_I32: {
            int32_t pdata[kspacket[index].lens];
            kSerial_GetPacketData(kspacket, pdata, (uint32_t)index);
            for (uint32_t i = 0; i < kspacket[index].lens; i++) {
                data[i] = (double)pdata[i];
            }
            break;
        }
        case KS_I64: {
            int64_t pdata[kspacket[index].lens];
            kSerial_GetPacketData(kspacket, pdata, (uint32_t)index);
            for (uint32_t i = 0; i < kspacket[index].lens; i++) {
                data[i] = (double)pdata[i];
            }
            break;
        }
        case KS_R0:
        case KS_R1:
        case KS_R2:
        case KS_R3:
        case KS_R4:
        case KS_U8: {
            uint8_t pdata[kspacket[index].lens];
            kSerial_GetPacketData(kspacket, pdata, (uint32_t)index);
            for (uint32_t i = 0; i < kspacket[index].lens; i++) {
                data[i] = (double)pdata[i];
            }
            break;
        }
        case KS_U16: {
            uint16_t pdata[kspacket[index].lens];
            kSerial_GetPacketData(kspacket, pdata, (uint32_t)index);
            for (uint32_t i = 0; i < kspacket[index].lens; i++) {
                data[i] = (double)pdata[i];
            }
            break;
        }
        case KS_U32: {
            uint32_t pdata[kspacket[index].lens];
            kSerial_GetPacketData(kspacket, pdata, (uint32_t)index);
            for (uint32_t i = 0; i < kspacket[index].lens; i++) {
                data[i] = (double)pdata[i];
            }
            break;
        }
        case KS_U64: {
            uint64_t pdata[kspacket[index].lens];
            kSerial_GetPacketData(kspacket, pdata, (uint32_t)index);
            for (uint32_t i = 0; i < kspacket[index].lens; i++) {
                data[i] = (double)pdata[i];
            }
            break;
        }
//        case KS_F16: {
//            float16_t pdata[kspacket[index].lens];
//            for (uint32_t i = 0; i < kspacket[index].lens; i++) {
//                kSerial_GetPacketData(kspacket, pdata, (uint32_t)index);
//                data[i] = (double)pdata[i];
//            }
//            break;
//        }
        case KS_F32: {
            float pdata[kspacket[index].lens];
            kSerial_GetPacketData(kspacket, pdata, (uint32_t)index);
            for (uint32_t i = 0; i < kspacket[index].lens; i++) {
                data[i] = (double)pdata[i];
            }
            break;
        }
        case KS_F64: {
            kSerial_GetPacketData(kspacket, data, (uint32_t)index);
            break;
        }
        default: {
            for (uint32_t i = 0; i < kspacket[index].lens; i++) {
                data[i] = 0;
            }
            break;
        }
    }
    env->SetDoubleArrayRegion(jdata, 0, kspacket[index].lens, data);

    return jdata;
}

}
