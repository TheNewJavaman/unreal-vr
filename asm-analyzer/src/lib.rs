#![allow(dead_code)]

use std::collections::HashMap;
use std::rc::Rc;

use capstone::{InsnId, Instructions};

mod tests;

/// Arbitrary max number of instructions per function to keep memory usage down and accelerate
/// training
pub type MaxInsnPerFn = u16;

/// Arbitrary max number of push/pop/ret instructions per function to keep memory usage down and
/// accelerate training
pub type MaxBookendOpPerFn = u8;

/// Represents the flow of conditional branching within a function
pub struct FnTree<'a> {
    instructions: &'a Instructions<'a>,
    blocks: HashMap<MaxInsnPerFn, Rc<BlkNode<'a>>>,
    root: Rc<BlkNode<'a>>,
}

/// A set of instructions that eventually ends in a [BlkEnd](BlkEnd)
pub struct BlkNode<'a> {
    instructions: &'a Instructions<'a>,
    next: BlkEnd<'a>,
}

impl<'a> FnTree<'a> {
    fn new(instructions: &'a Instructions<'a>) -> FnTree<'a> {
        FnTree {
            instructions,
            blocks: HashMap::new(),
            root: Rc::new(BlkNode {
                instructions,
                next: BlkEnd::Ret,
            }),
        }
    }
}

/// How a [BlkNode](BlkNode) ends
///
/// Note: Any [Option](Option) that is [None](None) is out of the scope of the [FnTree](FnTree)
enum BlkEnd<'a> {
    /// End of the function
    Ret,
    /// Continue execution; this indicates that other code merges back here
    Cont(Option<Rc<BlkNode<'a>>>),
    /// Unconditional jump elsewhere
    Jmp(Option<Rc<BlkNode<'a>>>),
    /// Conditional jump, resulting in branching
    Branch(Option<Rc<BlkNode<'a>>>, Option<Rc<BlkNode<'a>>>),
}

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
