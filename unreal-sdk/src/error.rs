use std::error::Error;
use std::fmt::{Display, Formatter};

use windows::Win32::Foundation::WIN32_ERROR;

pub type DynError = Box<dyn Error>;

#[derive(Debug)]
pub struct Win32Error {
    error: WIN32_ERROR,
}

impl From<WIN32_ERROR> for Win32Error {
    fn from(error: WIN32_ERROR) -> Self {
        Self { error }
    }
}

impl Display for Win32Error {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        write!(f, "Error code: {} (base 10)", self.error.0)
    }
}

impl Error for Win32Error {}
