use std::collections::HashMap;
use std::rc::Rc;

use capstone::Instructions;

use crate::MaxInsnPerFn;

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
    pub fn new(instructions: &'a Instructions<'a>) -> FnTree<'a> {
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
pub enum BlkEnd<'a> {
    /// End of the function
    Ret,
    /// Continue execution; this indicates that other code merges back here
    Cont(Option<Rc<BlkNode<'a>>>),
    /// Unconditional jump elsewhere
    Jmp(Option<Rc<BlkNode<'a>>>),
    /// Conditional jump, resulting in branching
    Branch(Option<Rc<BlkNode<'a>>>, Option<Rc<BlkNode<'a>>>),
}

pub enum AsmFormat {
    Raw,
    UnescapedAscii,
    EscapedAscii,
}
