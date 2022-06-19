use crate::proc_info::get_module_interval;

pub trait Pattern {
    fn is_match(&self, ptr: *const u8) -> bool;
}

#[derive(Debug, PartialEq, Eq)]
pub enum WildcardByte {
    Value(u8),
    Wildcard,
}

#[derive(Debug, Default)]
pub struct WildcardPattern {
    bytes: Vec<WildcardByte>,
}

impl Pattern for WildcardPattern {
    fn is_match(&self, ptr: *const u8) -> bool {
        self.bytes
            .iter()
            .enumerate()
            .all(|(i, byte)| match byte {
                WildcardByte::Value(v) => v == &(unsafe { *ptr.offset(i as isize) }),
                WildcardByte::Wildcard => true
            })
    }
}

#[derive(Debug, Default)]
pub struct PatternStream {
    matches: Vec<*const u8>,
}

impl PatternStream {
    pub fn new(pattern: &dyn Pattern, module: Option<&str>) -> Self {
        let interval = get_module_interval(module);
        let mut stream = PatternStream::default();
        for ptr in interval {
            if pattern.is_match(ptr) {
                stream.matches.push(ptr);
            }
        }
        stream
    }

    pub fn has_pattern_in_range(
        &self,
        pattern: &dyn Pattern,
        range_offset: isize,
        range_length: isize,
        replace_prev_match: bool,
    ) -> Self {
        let mut stream = PatternStream::default();
        for ptr in self.matches.iter() {
            for i in 0..range_length {
                let offset_ptr = unsafe { ptr.offset(range_offset + i) };
                if pattern.is_match(offset_ptr) {
                    if replace_prev_match {
                        stream.matches.push(offset_ptr);
                    } else {
                        stream.matches.push(*ptr);
                    }
                }
            }
        }
        stream
    }

    pub fn for_each<F: Fn(&*const u8) -> *const u8>(&self, f: F) -> PatternStream {
        PatternStream {
            matches: self.matches
                .iter()
                .map(f)
                .collect()
        }
    }

    pub fn add_offset(&self, offset: isize) -> PatternStream {
        self.for_each(|ptr| unsafe { ptr.offset(offset) })
    }

    pub fn first(&self) -> Option<*const u8> {
        self.matches.first().copied()
    }
}

pub fn init() {

}