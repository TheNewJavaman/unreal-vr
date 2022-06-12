pub trait PatternMatcher {
    fn is_match(&self, ptr: *const u8) -> bool;
}

#[derive(PartialEq, Eq)]
pub enum WildcardByte {
    Value(u8),
    Wildcard,
}

pub struct WildcardPattern {
    bytes: Vec<WildcardByte>,
}

impl PatternMatcher for WildcardPattern {
    fn is_match(&self, ptr: *const u8) -> bool {
        self.bytes
            .iter()
            .enumerate()
            .all(|(i, byte)| match byte {
                WildcardByte::Value(v) => v == &(unsafe { *(ptr.add(i)) }),
                WildcardByte::Wildcard => true
            })
    }
}

pub struct PatternStream {
    matches: Vec<*mut u8>,
}