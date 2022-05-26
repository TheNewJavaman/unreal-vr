#pragma once

#include "ACacheable.h"
#include "Unreal/Core.h"

namespace UEVersionedApi {
    struct UFunction_X;

    /// @todo Fields
    struct UObjectBase_X {};

    /// @todo Fields
    struct UObjectUtils_X : UObjectBase_X {};

    /// @todo Fields
    struct UObject_X : UObjectUtils_X {
        /// @version ? - 4.25 - 4.26 - ?
        struct ProcessEvent_0_t : ACacheableMember<void __cdecl(UObject_X*, UFunction_X*, void*), UObject_X> {
            ProcessEvent_0_t(UObject_X* parent) : ACacheableMember(parent) {}
            bool RefreshCache() override;
            void operator ()(UFunction_X* function, void* params);
        } ProcessEvent_0 = { this };
    };

    /// @todo Fields
    struct UFunction_X : UObject_X {};

    /// @version 4.11 - 4.12
    struct FUObjectItem_0 {
        UObjectBase_X* Object;
        int32_t ClusterAndFlags;
        int32_t SerialNumber;
    };

    /// @version 4.13 - Latest (5.0)
    struct FUObjectItem_1 {
        UObjectBase_X* Object;
        int32_t Flags;
        int32_t ClusterIndex;
        int32_t SerialNumber;
    };

    /// @version ? - 4.19 - 4.20 - ?
    struct FFixedUObjectArray_0 {
        FUObjectItem_1* Objects;
        int32_t MaxElements;
        int32_t NumElements;
    };

    /// @version 4.20 - Latest (5.0)
    /// Some games alter the chunk schema; calculate enum NumElementsPerChunk using MaxElements / NumElements
    struct FChunkedFixedUObjectArray_0 {
        FUObjectItem_1** Objects;
        FUObjectItem_1* PreAllocatedObjects;
        int32_t MaxElements;
        int32_t NumElements;
        int32_t MaxChunks;
        int32_t NumChunks;
    };

    /// @version 4.8 - 10
    using TUObjectArray_0 = TStaticIndirectArrayThreadSafeRead_0<UObjectBase_X, 8 * 1024 * 1024, 16384>;

    /// @version 4.11 - 4.19
    using TUObjectArray_1 = FFixedUObjectArray_0;

    /// @version 4.20 - Latest (5.0)
    using TUObjectArray_2 = FChunkedFixedUObjectArray_0;

    /// @version 4.0 - 4.7
    /// @todo Fields after ObjObjects
    struct FUObjectArray_0 {
        int32_t ObjFirstGCIndex;
        int32_t ObjLastNonGCIndex;
        int32_t OpenForDisregardForGC;
        TArray_0<UObjectBase_X*> ObjObjects;
    };

    /// @version 4.8 - 4.10
    /// @todo Fields after ObjObjects
    struct FUObjectArray_1 {
        int32_t ObjFirstGCIndex;
        int32_t ObjLastNonGCIndex;
        int32_t OpenForDisregardForGC;
        TUObjectArray_0 ObjObjects;
    };

    /// @version 4.11 - 4.19
    /// @todo Fields after ObjObjects
    struct FUObjectArray_2 {
        int32_t ObjFirstGCIndex;
        int32_t ObjLastNonGCIndex;
        int32_t MaxObjectsNotConsideredByGC;
        bool OpenForDisregardForGC;
        TUObjectArray_1 ObjObjects;
    };

    /// @version 4.20 - Latest (5.0)
    /// @todo Fields after ObjObjects
    struct FUObjectArray_3 {
        int32_t ObjFirstGCIndex;
        int32_t ObjLastNonGCIndex;
        int32_t MaxObjectsNotConsideredByGC;
        bool OpenForDisregardForGC;
        TUObjectArray_2 ObjObjects;
    };
}