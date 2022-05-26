#pragma once

#include <cstdint>

namespace UEVersionedApi {
    /// @todo Fields
    struct FScriptContainerElement_X {};;

    /// @version 4.0 - 4.22
    struct FHeapAllocator_0 {
        enum { NeedsElementType = false };
        enum { RequireRangeCheck = true };

        struct ForAnyElementType {
            FScriptContainerElement_X* Data;
        };

        template<typename ElementType>
        struct ForElementType : ForAnyElementType {};
    };

    /// @version 4.23 - Latest (5.0)
    template<int IndexSize>
    struct TBitsToSizeType_0;

    template<>
    struct TBitsToSizeType_0<8> { using Type = int8_t; };

    template<>
    struct TBitsToSizeType_0<16> { using Type = int16_t; };

    template<>
    struct TBitsToSizeType_0<32> { using Type = int32_t; };

    template<>
    struct TBitsToSizeType_0<64> { using Type = int64_t; };

    /// @version 4.23 - Latest (5.0)
    template<int IndexSize>
    struct TSizedHeapAllocator_0 {
        using SizeType = typename TBitsToSizeType_0<IndexSize>::Type;

        enum { NeedsElementType = false };
        enum { RequireRangeCheck = true };

        struct ForAnyElementType {
            FScriptContainerElement_X* Data;
        };

        template<typename ElementType>
        struct ForElementType : ForAnyElementType {};
    };

    /// @version 4.23 - Latest (5.0)
    template<int IndexSize>
    struct TSizedDefaultAllocator_0 : TSizedHeapAllocator_0<IndexSize> {
        typedef TSizedHeapAllocator_0<IndexSize> Typedef;
    };

    /// @version 4.0 - 4.22
    struct FDefaultAllocator_0 : FHeapAllocator_0 {
        typedef FHeapAllocator_0 Typedef;
    };

    /// @version 4.23 - ?
    using FDefaultAllocator_1 = TSizedDefaultAllocator_0<32>;

    /// @version 4.0 - Latest (5.0)
    template<bool Predicate, typename TrueClass, typename FalseClass>
    struct TChooseClass_0;

    template<typename TrueClass, typename FalseClass>
    struct TChooseClass_0<true, TrueClass, FalseClass> {
        typedef TrueClass Result;
    };

    template<typename TrueClass, typename FalseClass>
    struct TChooseClass_0<false, TrueClass, FalseClass> {
        typedef FalseClass Result;
    };

    /// @version 4.0 - 4.22
    template<typename InElementType, typename InAllocator = FDefaultAllocator_0>
    struct TArray_0 {
        typedef InElementType ElementType;
        typedef InAllocator Allocator;
        typedef typename TChooseClass_0<
            Allocator::NeedsElementType,
            typename Allocator::template ForElementType<ElementType>,
            typename Allocator::ForAnyElementType
        >::Result ElementAllocatorType;

        ElementAllocatorType AllocatorInstance;
        int32_t ArrayNum;
        int32_t ArrayMax;
    };

    /// @version 4.23 - Latest (5.0)
    template<typename InElementType, typename InAllocator = FDefaultAllocator_1>
    struct TArray_1 {
        typedef InElementType ElementType;
        typedef InAllocator Allocator;
        typedef typename TChooseClass_0<
            Allocator::NeedsElementType,
            typename Allocator::template ForElementType<ElementType>,
            typename Allocator::ForAnyElementType
        >::Result ElementAllocatorType;
        typedef typename InAllocator::SizeType SizeType;

        ElementAllocatorType AllocatorInstance;
        SizeType ArrayNum;
        SizeType ArrayMax;
    };

    /// @version 4.0 - 4.18
    template<typename ElementType, int32_t MaxTotalElements, int32_t ElementsPerChunk>
    struct TStaticIndirectArrayThreadSafeRead_0 {
        enum { ChunkTableSize = (MaxTotalElements + ElementsPerChunk - 1) / ElementsPerChunk };

        ElementType** Chunks[ChunkTableSize];
        int32_t NumElements;
        int32_t NumChunks;
    };

    /// @version 4.19 - ?
    /// @todo Fields
    template<typename ElementType, int32_t MaxTotalElements, int32_t ElementsPerChunk>
    struct TStaticIndirectArrayThreadSafeRead_1 {};
}