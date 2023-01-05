#include <jni.h>
#include <cstdlib>
#include "geomag.h"
#include "libwmm.h"
#include "logging.h"

MagneticField W_MF;

MagneticField wmm(double lat, double lon, double alt, double decimal_years) {
    double *values = WMM(lat, lon, alt, decimal_years);

    MagneticField results = {};
    MagneticField* results_t = &results;

    double *item = &results_t -> Declination;
    for (int i = 0; i < 14; i++) {
        double value = values[i];
        if (isnan(value)) {
            (*item++) = 1.7976931348623157E308;
        } else {
            (*item++) = value;
        }
    }

    return results;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sanmer_geomag_core_models_WMM_setDateTime(JNIEnv *env, jobject thiz, jint year, jint month, jint day,
                                                   jint hour, jint min, jint sec) {
    double decimal_years = GEOMAG::getDecimalYears(year, month, day, hour, min, sec);

    jclass cls = (*env).GetObjectClass(thiz);
    jmethodID set_dy = (*env).GetMethodID(cls, "setDecimalYears", "(D)V");
    (*env).CallVoidMethod(thiz, set_dy, decimal_years);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sanmer_geomag_core_models_WMM_wmm(JNIEnv *env, jobject thiz, jdouble latitude,
                                           jdouble longitude, jdouble alt_km) {
    jclass cls = (*env).GetObjectClass(thiz);
    jfieldID dy_id = (*env).GetStaticFieldID(cls, "decimalYears", "D");
    jdouble dy = (*env).GetStaticDoubleField(cls, dy_id);

    LOGD("WMM lat=%f, lon=%f, alt_km=%f, date=%f", latitude, longitude, alt_km, dy);
    W_MF = wmm(latitude, longitude, alt_km, dy);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_sanmer_geomag_core_models_WMM_getMF(JNIEnv *env, jobject thiz) {
    return GEOMAG::toMagneticField(env, W_MF);
}