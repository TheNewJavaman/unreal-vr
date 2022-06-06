use crate::ue_api::core::f_name_entry_id;

/// Version: 4.0 - 4.4
#[repr(C)]
pub struct V0 {
    index: i32,
    number: i32,
}

/**
 * Version: 4.5 - 4.10
 *
 * Variant A: `#if WITH_CASE_PRESERVING_NAME`; "Currently this is enabled for the Editor and any
 * Programs (such as UHT), but not the Runtime"
 */
#[repr(C)]
#[derive(Copy, Clone)]
pub struct V1A {
    comparison_index: i32,
    display_index: i32,
    number: u32,
}

/**
 * Version: 4.5 - 4.10
 *
 * Variant B: Not preserving case name, this is probably the one you want for shipped games
 */
#[repr(C)]
#[derive(Copy, Clone)]
pub struct V1B {
    comparison_index: i32,
    number: u32,
}

/**
 * Version: 4.11 - 4.21
 *
 * Variant A: `#if WITH_CASE_PRESERVING_NAME`; "Currently this is enabled for the Editor and any
 * Programs (such as UHT), but not the Runtime"
 */
#[repr(C)]
pub union V2A {
    opt_0: V2AOpt0,
}

/// Must define a type because anonymous structs don't exist in Rust
pub type V2AOpt0 = V1A;

/**
 * Version: 4.11 - 4.21
 *
 * Variant B: Not preserving case name, this is probably the one you want for shipped games
 */
#[repr(C)]
pub union V2B {
    opt_0: V2BOpt0,
    composite_comparison_value: u64,
}

/// Must define a type because anonymous structs don't exist in Rust
pub type V2BOpt0 = V1B;

/**
 * Version: 4.22
 *
 * Variant A: `#if WITH_CASE_PRESERVING_NAME`; "Currently this is enabled for the Editor and any
 * Programs (such as UHT), but not the Runtime"
 */
pub type V3A = V1A;

/**
 * Version: 4.22
 *
 * Variant B: Not preserving case name, this is probably the one you want for shipped games
 */
pub type V3B = V1B;

/**
 * Version: 4.23 - Latest (5.0)
 *
 * Variant A: `#if WITH_CASE_PRESERVING_NAME`; "Currently this is enabled for the Editor and any
 * Programs (such as UHT), but not the Runtime"
 */
#[repr(C)]
pub struct V4A {
    comparison_index: f_name_entry_id::V0,
    display_index: f_name_entry_id::V0,
    number: u32,
}

/**
 * Version: 4.23 - Latest (5.0)
 *
 * Variant B: Not preserving case name, this is probably the one you want for shipped games
 */
#[repr(C)]
pub struct V4B {
    comparison_index: f_name_entry_id::V0,
    number: u32,
}