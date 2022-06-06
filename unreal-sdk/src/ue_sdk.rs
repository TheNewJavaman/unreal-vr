#[derive(PartialEq, Eq)]
pub enum Version {
    V4_0,
    V4_1,
    V4_2,
    V4_3,
    V4_4,
    V4_5,
    V4_6,
    V4_7,
    V4_8,
    V4_9,
    V4_10,
    V4_11,
    V4_12,
    V4_13,
    V4_14,
    V4_15,
    V4_16,
    V4_17,
    V4_18,
    V4_19,
    V4_20,
    V4_21,
    V4_22,
    V4_23,
    V4_24,
    V4_25,
    V4_26,
    V4_27,
    V5_0,
}

pub struct Sdk<const VERSION: Version> {}

impl<const VERSION: Version> Sdk<VERSION> {
    pub fn new<const NEW_VERSION: Version>() -> Sdk<NEW_VERSION> {
        Sdk {}
    }
}
