#![allow(dead_code)]

pub mod error;
pub mod install;
pub mod version;

mod tests;

#[macro_export]
macro_rules! ue_sdk {
    {
        version: $version: expr,
        modules: [$($module: literal),+$(,)?]$(,)?
    } => {
        {

        }
    };
    {
        path: $version: expr,
        modules: [$($module: literal),+$(,)?]$(,)?
    } => {
        {

        }
    };
}
