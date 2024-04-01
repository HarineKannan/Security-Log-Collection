#include <jni.h>
#include <vector>
#include "org_example_Test.h"
using namespace std;

struct StudentRecord {
    jstring name;
    jint rollNumber;
    jstring departement;
    jfloat totalMark;
    jboolean hasReservation;

    StudentRecord(JNIEnv* env, const string& name, int rollNumber, const string& departement, float totalMark, bool hasReservation)
        : rollNumber(rollNumber) {
        this->name = env->NewStringUTF(name.c_str());
        this->departement = env->NewStringUTF(departement.c_str());
        this->totalMark = totalMark;
        this->hasReservation = hasReservation;
    }

    ~StudentRecord() {
        if (name) {
            JNIEnv* env;
            jvm->AttachCurrentThread(&env, NULL);
            env->DeleteLocalRef(name);
        }
        if (departement) {
            JNIEnv* env;
            jvm->AttachCurrentThread(&env, NULL);
            env->DeleteLocalRef(departement);
        }
    }
};

vector<StudentRecord> studentRecords = {
    StudentRecord(env, "John", 101, "Computer Science", 75.5, false),
    StudentRecord(env, "Jane", 102, "Electrical Engineering", 82.3, true),
    StudentRecord(env, "Jack", 103, "Mechanical Engineering", 78.9, false)
};

extern "C" JNIEXPORT jobjectArray JNICALL
Java_com_test_Test_getStudentDetails(JNIEnv * env, jclass) {
    int len = studentRecords.size();
    jobjectArray result = env->NewObjectArray(len, env->FindClass("com/test/StudentRecord"), NULL);
    for (int i = 0; i < len; i++) {
        StudentRecord& record = studentRecords[i];
        jclass studentRecordClass = env->FindClass("com/test/StudentRecord");
        jmethodID constructor = env->GetMethodID(studentRecordClass, "<init>", "(Ljava/lang/String;ILjava/lang/String;FZ)V");
        jobject studentRecord = env->NewObject(studentRecordClass, constructor, record.name, record.rollNumber, record.departement, record.totalMark, record.hasReservation);
        env->SetObjectArrayElement(result, i, studentRecord);
    }
    return result;
}