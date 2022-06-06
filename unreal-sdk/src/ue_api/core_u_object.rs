pub mod e_object_flags {
    use bitflags::bitflags;

    bitflags! {
        /// Todo: Values
        #[repr(C)]
        pub struct X: u32 {}
    }
}

pub mod u_object_base {
    use crate::ue_api::core_u_object::{e_object_flags, u_class, u_object};

    /// Version: 4.0 - ?
    #[repr(C)]
    pub struct V0 {
        object_flags: e_object_flags::X,
        internal_index: i32,
        class: *mut u_class::X,
        name: f_name::X,
        outer: *mut u_object::X
    }
}

pub mod u_object {
    /// Todo: Fields
    #[repr(C)]
    pub struct X {}
}

pub mod u_class {
    /// Todo: fields
    #[repr(C)]
    pub struct X {}
}