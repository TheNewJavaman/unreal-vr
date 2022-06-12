pub enum PatternByte {
    Value(u8),
    Wildcard,
}

pub type WildcardPattern = Vec<PatternByte>;

pub trait PatternMatcher {
    fn is_match(&self, ptr: *const u8);
}

impl PatternMatcher for WildcardPattern {

}

pub struct PatternStream {
    matches: Vec<*mut u8>,
}