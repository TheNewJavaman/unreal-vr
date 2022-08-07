use std::collections::HashMap;

use capstone::InsnId;

use crate::{MaxBookendOpPerFn, MaxInsnPerFn};

/// Numerical inputs for training/prediction of a function in pass 1
pub struct FnParams1 {
    num_push: MaxBookendOpPerFn,
    num_pop: MaxBookendOpPerFn,
    num_ret: MaxBookendOpPerFn,
}

/// Numerical inputs for training/prediction of a block in pass 1
pub struct BlkParams1 {
    insn_counts: HashMap<InsnId, MaxInsnPerFn>,
}

/// Numerical inputs for training/prediction of a function in pass 2
pub struct FnParams2 {
    num_push: MaxBookendOpPerFn,
    num_pop: MaxBookendOpPerFn,
    num_ret: MaxBookendOpPerFn,
}

/// Numerical inputs for training/prediction of a block in pass 2
pub struct BlkParams2 {
    insn_counts: HashMap<InsnId, MaxInsnPerFn>,
}
