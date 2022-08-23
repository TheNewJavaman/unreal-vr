#![allow(dead_code)]

use std::error::Error;
use std::fmt::{self, Debug, Display, Formatter};

pub mod install;
pub mod version;
pub mod error;

#[macro_export]
macro_rules! ue_sdk {
    {
        .version = $version: literal,
        .modules = [$($module: literal),+]$(,)?
    } => {

    };
}

#[derive(Debug)]
pub enum UeSdkErrorKind {
    InstallPathInvalid,
    InstallVersionInvalid,
    VersionStringInvalid,
}
