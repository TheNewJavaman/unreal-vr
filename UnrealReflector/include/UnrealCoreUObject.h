#pragma once

#include "ACacheable.h"

namespace UE {
    class UObject_0;
    class UFunction_0;

    /**
     * @version 4.0 - 4.0
     */
    namespace EObjectFlags_0 {
        enum Type {
            RF_Public = 0x00000001,
            RF_Standalone = 0x00000002,
            RF_Native = 0x00000004,
            RF_Transactional = 0x00000008,
            RF_ClassDefaultObject = 0x00000010,
            RF_ArchetypeObject = 0x00000020,
            RF_Transient = 0x00000040,
            RF_RootSet = 0x00000080,
            RF_Unreachable = 0x00000100,
            RF_TagGarbageTemp = 0x00000200,
            RF_NeedLoad = 0x00000400,
            RF_AsyncLoading = 0x00000800,
            RF_NeedPostLoad = 0x00001000,
            RF_NeedPostLoadSubobjects = 0x00002000,
            RF_PendingKill = 0x00004000,
            RF_BeginDestroyed = 0x00008000,
            RF_FinishDestroyed = 0x00010000,
            RF_BeingRegenerated = 0x00020000,
            RF_DefaultSubObject = 0x00040000,
            RF_WasLoaded = 0x00080000,
            RF_TextExportTransient = 0x00100000,
            RF_AllFlags = 0x001fffff,
            RF_NoFlags = 0x00000000,
            RF_Load = RF_Public | RF_Standalone | RF_Native | RF_Transactional | RF_ClassDefaultObject
                | RF_ArchetypeObject | RF_DefaultSubObject | RF_TextExportTransient,
            RF_PropagateToSubObjects = RF_Public | RF_ArchetypeObject | RF_Transactional | RF_Transient
        };
    }

    /**
     * @version 4.1 - 4.6
     */
    namespace EObjectFlags_1 {
        enum Type : uint32_t {
            RF_Public = 0x00000001,
            RF_Standalone = 0x00000002,
            RF_Native = 0x00000004,
            RF_Transactional = 0x00000008,
            RF_ClassDefaultObject = 0x00000010,
            RF_ArchetypeObject = 0x00000020,
            RF_Transient = 0x00000040,
            RF_RootSet = 0x00000080,
            RF_Unreachable = 0x00000100,
            RF_TagGarbageTemp = 0x00000200,
            RF_NeedLoad = 0x00000400,
            RF_AsyncLoading = 0x00000800,
            RF_NeedPostLoad = 0x00001000,
            RF_NeedPostLoadSubobjects = 0x00002000,
            RF_PendingKill = 0x00004000,
            RF_BeginDestroyed = 0x00008000,
            RF_FinishDestroyed = 0x00010000,
            RF_BeingRegenerated = 0x00020000,
            RF_DefaultSubObject = 0x00040000,
            RF_WasLoaded = 0x00080000,
            RF_TextExportTransient = 0x00100000,
            RF_LoadCompleted = 0x00200000,
            RF_AllFlags = 0x003fffff,
            RF_NoFlags = 0x00000000,
            RF_Load = RF_Public | RF_Standalone | RF_Native | RF_Transactional | RF_ClassDefaultObject
                | RF_ArchetypeObject | RF_DefaultSubObject | RF_TextExportTransient,
            RF_PropagateToSubObjects = RF_Public | RF_ArchetypeObject | RF_Transactional | RF_Transient
        };
    }

    /**
     * @version 4.7 - 4.7
     */
    namespace EObjectFlags_2 {
        enum Type : uint32_t {
            RF_Public = 0x00000001,
            RF_Standalone = 0x00000002,
            RF_Native = 0x00000004,
            RF_Transactional = 0x00000008,
            RF_ClassDefaultObject = 0x00000010,
            RF_ArchetypeObject = 0x00000020,
            RF_Transient = 0x00000040,
            RF_RootSet = 0x00000080,
            RF_Unreachable = 0x00000100,
            RF_TagGarbageTemp = 0x00000200,
            RF_NeedLoad = 0x00000400,
            RF_AsyncLoading = 0x00000800,
            RF_NeedPostLoad = 0x00001000,
            RF_NeedPostLoadSubobjects = 0x00002000,
            RF_PendingKill = 0x00004000,
            RF_BeginDestroyed = 0x00008000,
            RF_FinishDestroyed = 0x00010000,
            RF_BeingRegenerated = 0x00020000,
            RF_DefaultSubObject = 0x00040000,
            RF_WasLoaded = 0x00080000,
            RF_TextExportTransient = 0x00100000,
            RF_LoadCompleted = 0x00200000,
            RF_InheritableComponentTemplate = 0x00400000,
            RF_AllFlags = 0x00ffffff,
            RF_NoFlags = 0x00000000,
            RF_Load = RF_Public | RF_Standalone | RF_Native | RF_Transactional | RF_ClassDefaultObject
                | RF_ArchetypeObject | RF_DefaultSubObject | RF_TextExportTransient | RF_InheritableComponentTemplate,
            RF_PropagateToSubObjects = RF_Public | RF_ArchetypeObject | RF_Transactional | RF_Transient
        };
    }

    /**
     * @version 4.8 - 4.8
     */
    namespace EObjectFlags_3 {
        enum Type : uint32_t {
            RF_Public = 0x00000001,
            RF_Standalone = 0x00000002,
            RF_Native = 0x00000004,
            RF_Transactional = 0x00000008,
            RF_ClassDefaultObject = 0x00000010,
            RF_ArchetypeObject = 0x00000020,
            RF_Transient = 0x00000040,
            RF_RootSet = 0x00000080,
            RF_Unreachable = 0x00000100,
            RF_TagGarbageTemp = 0x00000200,
            RF_NeedLoad = 0x00000400,
            RF_AsyncLoading = 0x00000800,
            RF_NeedPostLoad = 0x00001000,
            RF_NeedPostLoadSubobjects = 0x00002000,
            RF_PendingKill = 0x00004000,
            RF_BeginDestroyed = 0x00008000,
            RF_FinishDestroyed = 0x00010000,
            RF_BeingRegenerated = 0x00020000,
            RF_DefaultSubObject = 0x00040000,
            RF_WasLoaded = 0x00080000,
            RF_TextExportTransient = 0x00100000,
            RF_LoadCompleted = 0x00200000,
            RF_InheritableComponentTemplate = 0x00400000,
            RF_Async = 0x00800000,
            RF_AllFlags = 0x00ffffff,
            RF_NoFlags = 0x00000000,
            RF_Load = RF_Public | RF_Standalone | RF_Native | RF_Transactional | RF_ClassDefaultObject
                | RF_ArchetypeObject | RF_DefaultSubObject | RF_TextExportTransient | RF_InheritableComponentTemplate,
            RF_PropagateToSubObjects = RF_Public | RF_ArchetypeObject | RF_Transactional | RF_Transient
        };
    }

    /**
     * @version 4.9 - 4.9
     */
    namespace EObjectFlags_4 {
        enum Type : uint32_t {
            RF_Public = 0x00000001,
            RF_Standalone = 0x00000002,
            RF_Native = 0x00000004,
            RF_Transactional = 0x00000008,
            RF_ClassDefaultObject = 0x00000010,
            RF_ArchetypeObject = 0x00000020,
            RF_Transient = 0x00000040,
            RF_RootSet = 0x00000080,
            RF_Unreachable = 0x00000100,
            RF_TagGarbageTemp = 0x00000200,
            RF_NeedLoad = 0x00000400,
            RF_AsyncLoading = 0x00000800,
            RF_NeedPostLoad = 0x00001000,
            RF_NeedPostLoadSubobjects = 0x00002000,
            RF_PendingKill = 0x00004000,
            RF_BeginDestroyed = 0x00008000,
            RF_FinishDestroyed = 0x00010000,
            RF_BeingRegenerated = 0x00020000,
            RF_DefaultSubObject = 0x00040000,
            RF_WasLoaded = 0x00080000,
            RF_TextExportTransient = 0x00100000,
            RF_LoadCompleted = 0x00200000,
            RF_InheritableComponentTemplate = 0x00400000,
            RF_Async = 0x00800000,
            RF_StrongRefOnFrame = 0x01000000,
            RF_NoStrongReference = 0x02000000,
            RF_AllFlags = 0x03ffffff,
            RF_NoFlags = 0x00000000,
            RF_Load = RF_Public | RF_Standalone | RF_Native | RF_Transactional | RF_ClassDefaultObject
                | RF_ArchetypeObject | RF_DefaultSubObject | RF_TextExportTransient | RF_InheritableComponentTemplate,
            RF_PropagateToSubObjects = RF_Public | RF_ArchetypeObject | RF_Transactional | RF_Transient
        };
    }

    /**
     * @version 4.10 - 4.10
     */
    namespace EObjectFlags_5 {
        enum Type : uint32_t {
            RF_Public = 0x00000001,
            RF_Standalone = 0x00000002,
            RF_Native = 0x00000004,
            RF_Transactional = 0x00000008,
            RF_ClassDefaultObject = 0x00000010,
            RF_ArchetypeObject = 0x00000020,
            RF_Transient = 0x00000040,
            RF_RootSet = 0x00000080,
            RF_Unreachable = 0x00000100,
            RF_TagGarbageTemp = 0x00000200,
            RF_NeedLoad = 0x00000400,
            RF_AsyncLoading = 0x00000800,
            RF_NeedPostLoad = 0x00001000,
            RF_NeedPostLoadSubobjects = 0x00002000,
            RF_PendingKill = 0x00004000,
            RF_BeginDestroyed = 0x00008000,
            RF_FinishDestroyed = 0x00010000,
            RF_BeingRegenerated = 0x00020000,
            RF_DefaultSubObject = 0x00040000,
            RF_WasLoaded = 0x00080000,
            RF_TextExportTransient = 0x00100000,
            RF_LoadCompleted = 0x00200000,
            RF_InheritableComponentTemplate = 0x00400000,
            RF_Async = 0x00800000,
            RF_StrongRefOnFrame = 0x01000000,
            RF_NoStrongReference = 0x02000000,
            RF_AssetExport = 0x04000000,
            RF_AllFlags = 0x07ffffff,
            RF_NoFlags = 0x00000000,
            RF_Load = RF_Public | RF_Standalone | RF_Native | RF_Transactional | RF_ClassDefaultObject
                | RF_ArchetypeObject | RF_DefaultSubObject | RF_TextExportTransient | RF_InheritableComponentTemplate,
            RF_PropagateToSubObjects = RF_Public | RF_ArchetypeObject | RF_Transactional | RF_Transient
        };
    }

    /**
     * @version 4.11 - 4.11
     */
    namespace EObjectFlags_6 {
        enum Type : uint32_t {
            RF_NoFlags = 0x00000000,
            RF_Public = 0x00000001,
            RF_Standalone = 0x00000002,
            RF_MarkAsNative = 0x00000004,
            RF_Transactional = 0x00000008,
            RF_ClassDefaultObject = 0x00000010,
            RF_ArchetypeObject = 0x00000020,
            RF_Transient = 0x00000040,
            RF_MarkAsRootSet = 0x00000080,
            RF_TagGarbageTemp = 0x00000200,
            RF_NeedLoad = 0x00000400,
            RF_NeedPostLoad = 0x00001000,
            RF_NeedPostLoadSubobjects = 0x00002000,
            RF_BeginDestroyed = 0x00008000,
            RF_FinishDestroyed = 0x00010000,
            RF_BeingRegenerated = 0x00020000,
            RF_DefaultSubObject = 0x00040000,
            RF_WasLoaded = 0x00080000,
            RF_TextExportTransient = 0x00100000,
            RF_LoadCompleted = 0x00200000,
            RF_InheritableComponentTemplate = 0x00400000,
            RF_StrongRefOnFrame = 0x01000000,
            RF_Dynamic = 0x04000000,
            RF_AllFlags = 0x03ffffff,
            RF_Load = RF_Public | RF_Standalone | RF_Transactional | RF_ClassDefaultObject | RF_ArchetypeObject
                | RF_DefaultSubObject | RF_TextExportTransient | RF_InheritableComponentTemplate,
            RF_PropagateToSubObjects = RF_Public | RF_ArchetypeObject | RF_Transactional | RF_Transient
        };
    }

    /**
     * @version 4.12 - 4.12
     */
    namespace EObjectFlags_7 {
        enum Type : uint32_t {
            RF_NoFlags = 0x00000000,
            RF_Public = 0x00000001,
            RF_Standalone = 0x00000002,
            RF_MarkAsNative = 0x00000004,
            RF_Transactional = 0x00000008,
            RF_ClassDefaultObject = 0x00000010,
            RF_ArchetypeObject = 0x00000020,
            RF_Transient = 0x00000040,
            RF_MarkAsRootSet = 0x00000080,
            RF_TagGarbageTemp = 0x00000200,
            RF_NeedLoad = 0x00000400,
            RF_NeedPostLoad = 0x00001000,
            RF_NeedPostLoadSubobjects = 0x00002000,
            RF_BeginDestroyed = 0x00008000,
            RF_FinishDestroyed = 0x00010000,
            RF_BeingRegenerated = 0x00020000,
            RF_DefaultSubObject = 0x00040000,
            RF_WasLoaded = 0x00080000,
            RF_TextExportTransient = 0x00100000,
            RF_LoadCompleted = 0x00200000,
            RF_InheritableComponentTemplate = 0x00400000,
            RF_StrongRefOnFrame = 0x01000000,
            RF_Dynamic = 0x04000000,
            RF_AllFlags = 0x07ffffff,
            RF_Load = RF_Public | RF_Standalone | RF_Transactional | RF_ClassDefaultObject | RF_ArchetypeObject
                | RF_DefaultSubObject | RF_TextExportTransient | RF_InheritableComponentTemplate,
            RF_PropagateToSubObjects = RF_Public | RF_ArchetypeObject | RF_Transactional | RF_Transient
        };
    }

    /**
     * @version 4.13 - 4.13
     */
    namespace EObjectFlags_8 {
        enum Type : uint32_t {
            RF_NoFlags = 0x00000000,
            RF_Public = 0x00000001,
            RF_Standalone = 0x00000002,
            RF_MarkAsNative = 0x00000004,
            RF_Transactional = 0x00000008,
            RF_ClassDefaultObject = 0x00000010,
            RF_ArchetypeObject = 0x00000020,
            RF_Transient = 0x00000040,
            RF_MarkAsRootSet = 0x00000080,
            RF_TagGarbageTemp = 0x00000200,
            RF_NeedLoad = 0x00000400,
            RF_KeepForCooker = 0x00000800,
            RF_NeedPostLoad = 0x00001000,
            RF_NeedPostLoadSubobjects = 0x00002000,
            RF_BeginDestroyed = 0x00008000,
            RF_FinishDestroyed = 0x00010000,
            RF_BeingRegenerated = 0x00020000,
            RF_DefaultSubObject = 0x00040000,
            RF_WasLoaded = 0x00080000,
            RF_TextExportTransient = 0x00100000,
            RF_LoadCompleted = 0x00200000,
            RF_InheritableComponentTemplate = 0x00400000,
            RF_StrongRefOnFrame = 0x01000000,
            RF_Dynamic = 0x04000000,
            RF_AllFlags = 0x07ffffff,
            RF_Load = RF_Public | RF_Standalone | RF_Transactional | RF_ClassDefaultObject | RF_ArchetypeObject
                | RF_DefaultSubObject | RF_TextExportTransient | RF_InheritableComponentTemplate,
            RF_PropagateToSubObjects = RF_Public | RF_ArchetypeObject | RF_Transactional | RF_Transient
        };
    }

    /**
     * @version 4.14 - 4.14
     */
    namespace EObjectFlags_9 {
        enum Type : uint32_t {
            RF_NoFlags = 0x00000000,
            RF_Public = 0x00000001,
            RF_Standalone = 0x00000002,
            RF_MarkAsNative = 0x00000004,
            RF_Transactional = 0x00000008,
            RF_ClassDefaultObject = 0x00000010,
            RF_ArchetypeObject = 0x00000020,
            RF_Transient = 0x00000040,
            RF_MarkAsRootSet = 0x00000080,
            RF_TagGarbageTemp = 0x00000200,
            RF_NeedLoad = 0x00000400,
            RF_KeepForCooker = 0x00000800,
            RF_NeedPostLoad = 0x00001000,
            RF_NeedPostLoadSubobjects = 0x00002000,
            RF_BeginDestroyed = 0x00008000,
            RF_FinishDestroyed = 0x00010000,
            RF_BeingRegenerated = 0x00020000,
            RF_DefaultSubObject = 0x00040000,
            RF_WasLoaded = 0x00080000,
            RF_TextExportTransient = 0x00100000,
            RF_LoadCompleted = 0x00200000,
            RF_InheritableComponentTemplate = 0x00400000,
            RF_DuplicateTransient = 0x00800000,
            RF_StrongRefOnFrame = 0x01000000,
            RF_NonPIEDuplicateTransient = 0x02000000,
            RF_Dynamic = 0x04000000,
            RF_AllFlags = 0x07ffffff,
            RF_Load = RF_Public | RF_Standalone | RF_Transactional | RF_ClassDefaultObject | RF_ArchetypeObject
                | RF_DefaultSubObject | RF_TextExportTransient | RF_InheritableComponentTemplate | RF_DuplicateTransient
                | RF_NonPIEDuplicateTransient,
            RF_PropagateToSubObjects = RF_Public | RF_ArchetypeObject | RF_Transactional | RF_Transient
        };
    }

    /**
     * @version 4.15 - 4.15
     */
    namespace EObjectFlags_10 {
        enum Type : uint32_t {
            RF_NoFlags = 0x00000000,
            RF_Public = 0x00000001,
            RF_Standalone = 0x00000002,
            RF_MarkAsNative = 0x00000004,
            RF_Transactional = 0x00000008,
            RF_ClassDefaultObject = 0x00000010,
            RF_ArchetypeObject = 0x00000020,
            RF_Transient = 0x00000040,
            RF_MarkAsRootSet = 0x00000080,
            RF_TagGarbageTemp = 0x00000100,
            RF_NeedInitialization = 0x00000200,
            RF_NeedLoad = 0x00000400,
            RF_KeepForCooker = 0x00000800,
            RF_NeedPostLoad = 0x00001000,
            RF_NeedPostLoadSubobjects = 0x00002000,
            RF_NewerVersionExists = 0x00004000,
            RF_BeginDestroyed = 0x00008000,
            RF_FinishDestroyed = 0x00010000,
            RF_BeingRegenerated = 0x00020000,
            RF_DefaultSubObject = 0x00040000,
            RF_WasLoaded = 0x00080000,
            RF_TextExportTransient = 0x00100000,
            RF_LoadCompleted = 0x00200000,
            RF_InheritableComponentTemplate = 0x00400000,
            RF_DuplicateTransient = 0x00800000,
            RF_StrongRefOnFrame = 0x01000000,
            RF_NonPIEDuplicateTransient = 0x02000000,
            RF_Dynamic = 0x04000000,
            RF_AllFlags = 0x07ffffff,
            RF_Load = RF_Public | RF_Standalone | RF_Transactional | RF_ClassDefaultObject | RF_ArchetypeObject
                | RF_DefaultSubObject | RF_TextExportTransient | RF_InheritableComponentTemplate | RF_DuplicateTransient
                | RF_NonPIEDuplicateTransient,
            RF_PropagateToSubObjects = RF_Public | RF_ArchetypeObject | RF_Transactional | RF_Transient
        };
    }

    /**
     * @version 4.16 - 4.25
     */
    namespace EObjectFlags_11 {
        enum Type : uint32_t {
            RF_NoFlags = 0x00000000,
            RF_Public = 0x00000001,
            RF_Standalone = 0x00000002,
            RF_MarkAsNative = 0x00000004,
            RF_Transactional = 0x00000008,
            RF_ClassDefaultObject = 0x00000010,
            RF_ArchetypeObject = 0x00000020,
            RF_Transient = 0x00000040,
            RF_MarkAsRootSet = 0x00000080,
            RF_TagGarbageTemp = 0x00000100,
            RF_NeedInitialization = 0x00000200,
            RF_NeedLoad = 0x00000400,
            RF_KeepForCooker = 0x00000800,
            RF_NeedPostLoad = 0x00001000,
            RF_NeedPostLoadSubobjects = 0x00002000,
            RF_NewerVersionExists = 0x00004000,
            RF_BeginDestroyed = 0x00008000,
            RF_FinishDestroyed = 0x00010000,
            RF_BeingRegenerated = 0x00020000,
            RF_DefaultSubObject = 0x00040000,
            RF_WasLoaded = 0x00080000,
            RF_TextExportTransient = 0x00100000,
            RF_LoadCompleted = 0x00200000,
            RF_InheritableComponentTemplate = 0x00400000,
            RF_DuplicateTransient = 0x00800000,
            RF_StrongRefOnFrame = 0x01000000,
            RF_NonPIEDuplicateTransient = 0x02000000,
            RF_Dynamic = 0x04000000,
            RF_WillBeLoaded = 0x08000000,
            RF_AllFlags = 0x0fffffff,
            RF_Load = RF_Public | RF_Standalone | RF_Transactional | RF_ClassDefaultObject | RF_ArchetypeObject
                | RF_DefaultSubObject | RF_TextExportTransient | RF_InheritableComponentTemplate | RF_DuplicateTransient
                | RF_NonPIEDuplicateTransient,
            RF_PropagateToSubObjects = RF_Public | RF_ArchetypeObject | RF_Transactional | RF_Transient
        };
    }

    /**
     * @version 4.26 - 4.27
     */
    namespace EObjectFlags_12 {
        enum Type : uint32_t {
            RF_NoFlags = 0x00000000,
            RF_Public = 0x00000001,
            RF_Standalone = 0x00000002,
            RF_MarkAsNative = 0x00000004,
            RF_Transactional = 0x00000008,
            RF_ClassDefaultObject = 0x00000010,
            RF_ArchetypeObject = 0x00000020,
            RF_Transient = 0x00000040,
            RF_MarkAsRootSet = 0x00000080,
            RF_TagGarbageTemp = 0x00000100,
            RF_NeedInitialization = 0x00000200,
            RF_NeedLoad = 0x00000400,
            RF_KeepForCooker = 0x00000800,
            RF_NeedPostLoad = 0x00001000,
            RF_NeedPostLoadSubobjects = 0x00002000,
            RF_NewerVersionExists = 0x00004000,
            RF_BeginDestroyed = 0x00008000,
            RF_FinishDestroyed = 0x00010000,
            RF_BeingRegenerated = 0x00020000,
            RF_DefaultSubObject = 0x00040000,
            RF_WasLoaded = 0x00080000,
            RF_TextExportTransient = 0x00100000,
            RF_LoadCompleted = 0x00200000,
            RF_InheritableComponentTemplate = 0x00400000,
            RF_DuplicateTransient = 0x00800000,
            RF_StrongRefOnFrame = 0x01000000,
            RF_NonPIEDuplicateTransient = 0x02000000,
            RF_Dynamic = 0x04000000,
            RF_WillBeLoaded = 0x08000000,
            RF_HasExternalPackage = 0x10000000,
            RF_AllFlags = 0x1fffffff,
            RF_Load = RF_Public | RF_Standalone | RF_Transactional | RF_ClassDefaultObject | RF_ArchetypeObject
                | RF_DefaultSubObject | RF_TextExportTransient | RF_InheritableComponentTemplate | RF_DuplicateTransient
                | RF_NonPIEDuplicateTransient,
            RF_PropagateToSubObjects = RF_Public | RF_ArchetypeObject | RF_Transactional | RF_Transient
        };
    }

    /**
     * @version 5.0 - Latest (5.0)
     */
    namespace EObjectFlags_13 {
        enum Type : uint32_t {
            RF_NoFlags = 0x00000000,
            RF_Public = 0x00000001,
            RF_Standalone = 0x00000002,
            RF_MarkAsNative = 0x00000004,
            RF_Transactional = 0x00000008,
            RF_ClassDefaultObject = 0x00000010,
            RF_ArchetypeObject = 0x00000020,
            RF_Transient = 0x00000040,
            RF_MarkAsRootSet = 0x00000080,
            RF_TagGarbageTemp = 0x00000100,
            RF_NeedInitialization = 0x00000200,
            RF_NeedLoad = 0x00000400,
            RF_KeepForCooker = 0x00000800,
            RF_NeedPostLoad = 0x00001000,
            RF_NeedPostLoadSubobjects = 0x00002000,
            RF_NewerVersionExists = 0x00004000,
            RF_BeginDestroyed = 0x00008000,
            RF_FinishDestroyed = 0x00010000,
            RF_BeingRegenerated = 0x00020000,
            RF_DefaultSubObject = 0x00040000,
            RF_WasLoaded = 0x00080000,
            RF_TextExportTransient = 0x00100000,
            RF_LoadCompleted = 0x00200000,
            RF_InheritableComponentTemplate = 0x00400000,
            RF_DuplicateTransient = 0x00800000,
            RF_StrongRefOnFrame = 0x01000000,
            RF_NonPIEDuplicateTransient = 0x02000000,
            RF_Dynamic = 0x04000000,
            RF_WillBeLoaded = 0x08000000,
            RF_HasExternalPackage = 0x10000000,
            RF_PendingKill = 0x20000000,
            RF_Garbage = 0x40000000,
            RF_AllocatedInSharedPage = 0x80000000,
            RF_AllFlags = 0xffffffff,
            RF_InternalPendingKill = RF_PendingKill,
            RF_InternalGarbage = RF_Garbage,
            RF_InternalMirroredFlags = RF_PendingKill | RF_Garbage,
            RF_Load = RF_Public | RF_Standalone | RF_Transactional | RF_ClassDefaultObject | RF_ArchetypeObject
                | RF_DefaultSubObject | RF_TextExportTransient | RF_InheritableComponentTemplate | RF_DuplicateTransient
                | RF_NonPIEDuplicateTransient,
            RF_PropagateToSubObjects = RF_Public | RF_ArchetypeObject | RF_Transactional | RF_Transient
        };
    }

    /**
     * @version 4.0 - ?
     */
    class UObjectBase_0 {
    public:

    };

    /**
     * @version ? - Latest (5.0)
     */
    class UObjectBaseUtility_0 : public UObjectBase_0 {};

    /**
     * @version ? - Latest (5.0)
     */
    class UObject_0 : public UObjectBaseUtility_0 {
    public:
        class ProcessEvent_0 : public ACacheableMember<void __cdecl(UObject_0*, UFunction_0*, void*), UObject_0> {
        public:
            ProcessEvent_0(UObject_0* parent) : ACacheableMember(parent) {}

            bool RefreshCache() override;
            void operator ()(UFunction_0* function, void* params);
        } ProcessEvent = { this };
    };

    class UFunction_0 : public UObject_0 {};
}