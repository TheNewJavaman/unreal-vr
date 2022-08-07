#[allow(dead_code)]
pub mod tree;
#[allow(dead_code)]
pub mod model;

mod tests;

/// Arbitrary max number of instructions per function to keep memory usage down and accelerate
/// training
pub type MaxInsnPerFn = u16;

/// Arbitrary max number of push/pop/ret instructions per function to keep memory usage down and
/// accelerate training
pub type MaxBookendOpPerFn = u8;
