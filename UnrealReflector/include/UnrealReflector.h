#pragma once

enum class UEVersion {
    V4_0, V4_1, V4_2, V4_3, V4_4, V4_5, V4_6, V4_7, V4_8, V4_9, V4_10, V4_11, V4_12, V4_13, V4_14, V4_15, V4_16, V4_17,
    V4_18, V4_19, V4_20, V4_21, V4_22, V4_23, V4_24, V4_25, V4_26, V4_27, V5_0, Unknown
};

template<UEVersion>
struct UEAbstractApi;

template<>
struct UEAbstractApi<UEVersion::V4_0> {

};

template<>
struct UEAbstractApi<UEVersion::V4_1> {

};

template<>
struct UEAbstractApi<UEVersion::V4_2> {

};

template<>
struct UEAbstractApi<UEVersion::V4_3> {

};

template<>
struct UEAbstractApi<UEVersion::V4_4> {

};

template<>
struct UEAbstractApi<UEVersion::V4_5> {

};

template<>
struct UEAbstractApi<UEVersion::V4_6> {

};

template<>
struct UEAbstractApi<UEVersion::V4_7> {

};

template<>
struct UEAbstractApi<UEVersion::V4_8> {

};

template<>
struct UEAbstractApi<UEVersion::V4_9> {

};

template<>
struct UEAbstractApi<UEVersion::V4_10> {

};

template<>
struct UEAbstractApi<UEVersion::V4_11> {

};

template<>
struct UEAbstractApi<UEVersion::V4_12> {

};

template<>
struct UEAbstractApi<UEVersion::V4_13> {

};

template<>
struct UEAbstractApi<UEVersion::V4_14> {

};

template<>
struct UEAbstractApi<UEVersion::V4_15> {

};

template<>
struct UEAbstractApi<UEVersion::V4_16> {

};

template<>
struct UEAbstractApi<UEVersion::V4_17> {

};

template<>
struct UEAbstractApi<UEVersion::V4_18> {

};

template<>
struct UEAbstractApi<UEVersion::V4_19> {

};

template<>
struct UEAbstractApi<UEVersion::V4_20> {

};

template<>
struct UEAbstractApi<UEVersion::V4_21> {

};

template<>
struct UEAbstractApi<UEVersion::V4_22> {

};

template<>
struct UEAbstractApi<UEVersion::V4_23> {

};

template<>
struct UEAbstractApi<UEVersion::V4_24> {

};

template<>
struct UEAbstractApi<UEVersion::V4_25> {

};

template<>
struct UEAbstractApi<UEVersion::V4_26> {

};

template<>
struct UEAbstractApi<UEVersion::V4_27> {

};

template<>
struct UEAbstractApi<UEVersion::V5_0> {

};

template<>
struct UEAbstractApi<UEVersion::Unknown> {

};
