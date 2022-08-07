pub mod f_name;
pub mod f_name_entry_id;

pub mod f_script_container_element {
    /// Todo: Fields
    #[repr(C)]
    pub struct X {}
}

pub mod f_heap_allocator {
    /// Version: 4.0 - 4.22
    #[repr(C)]
    pub struct V0 {}

    impl V0 {
        const NEEDS_ELEMENT_TYPE: bool = false;
        const REQUIRE_RANGE_CHECK: bool = true;
    }

    pub mod for_any_element_type {
        use crate::ue_api::core::f_script_container_element;

        /// Version: 4.0 - 4.22
        #[repr(C)]
        pub struct V0 {
            data: f_script_container_element::X,
        }
    }

    pub mod for_element_type {
        use crate::ue_api::core::f_heap_allocator::for_any_element_type;

        /// Version: 4.0 - 4.22
        #[repr(C)]
        struct V0 {
            parent: for_any_element_type::V0,
        }
    }
}

pub mod t_sized_heap_allocator {
    use crate::ue_api::core::t_bits_to_size_type;

    /// Version: 4.23 - Latest (5.0)
    #[repr(C)]
    pub struct V0<const INDEX_SIZE: i32> {}

    impl<const INDEX_SIZE: i32> V0<INDEX_SIZE> {
        type SizeType = t_bits_to_size_type::V0<INDEX_SIZE>;

        const NEEDS_ELEMENT_TYPE: bool = false;
        const REQUIRE_RANGE_CHECK: bool = true;
    }

    pub mod for_any_element_type {
        use crate::ue_api::core::f_script_container_element;

        /// Version: 4.23 - Latest (5.0)
        #[repr(C)]
        pub struct V0 {
            data: f_script_container_element::X,
        }
    }

    pub mod for_element_type {
        use crate::ue_api::core::t_sized_heap_allocator::for_any_element_type;

        /// Version: 4.23 - Latest (5.0)
        #[repr(C)]
        struct V0 {
            parent: for_any_element_type::V0,
        }
    }
}

pub mod t_sized_default_allocator {
    use crate::ue_api::core::t_sized_heap_allocator;

    /// Version: 4.23 - Latest (5.0)
    #[repr(C)]
    pub struct V0<const INDEX_SIZE: i32> {
        parent: t_sized_heap_allocator::V0<INDEX_SIZE>,
    }

    impl<const INDEX_SIZE: i32> V0<INDEX_SIZE> {
        type Typedef = t_sized_heap_allocator::V0<INDEX_SIZE>;
    }
}

pub mod t_bits_to_size_type {
    /// Version: 4.23 - Latest (5.0)
    #[repr(C)]
    pub struct V0<const INDEX_SIZE: i32> {}

    impl V0<8> {
        type Type = i8;
    }

    impl V0<16> {
        type Type = i16;
    }

    impl V0<32> {
        type Type = i32;
    }

    impl V0<64> {
        type Type = i64;
    }
}

pub mod f_default_allocator {
    use crate::ue_api::core::{f_heap_allocator, t_sized_heap_allocator};

    /// Version: 4.0 - 4.22
    #[repr(C)]
    pub struct V0 {
        parent: f_heap_allocator::V0,
    }

    impl V0 {
        type Typedef = f_heap_allocator::V0;
    }

    /// Version: 4.23 - ?
    type V1 = t_sized_heap_allocator::V0<32>;
}

pub mod t_choose_class {
    use std::marker::PhantomData;

    /// Version: 4.0 - Latest (5.0)
    #[repr(C)]
    pub struct V0<const PREDICATE: bool, TrueClass, FalseClass> {
        _true_class: PhantomData<TrueClass>,
        _false_class: PhantomData<FalseClass>,
    }

    impl<TrueClass, FalseClass> V0<true, TrueClass, FalseClass> {
        pub type Result = TrueClass;
    }

    impl<TrueClass, FalseClass> V0<false, TrueClass, FalseClass> {
        pub type Result = FalseClass;
    }
}

/*
pub mod t_array {
    use crate::ue_api::core::{f_default_allocator, t_choose_class};

    pub struct V0<InElementType, InAllocator = f_default_allocator::V0> {
        allocator_instance:
        array_num: i32,
        array_max: i32,
    }

    impl<InElementType, InAllocator> V0<InElementType, InAllocator> {
        type ElementAllocatorType = t_choose_class::V0<InAllocator::NeedsElementType, TrueClass, FalseClass>
    }
}
 */
