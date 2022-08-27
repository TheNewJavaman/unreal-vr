#![allow(dead_code)]

extern crate core;

pub mod csharp;
pub mod dotenv;
pub mod error;
pub mod path;
pub mod source;
pub mod version;

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
        path: $path: expr,
        modules: [$($module: literal),+$(,)?]$(,)?
    } => {
        {

        }
    };
}
