use std::any::Any;
use std::path::{Path, PathBuf};
use std::{fmt, result};

use crate::error::{Error, ErrorKind, FmtResult, Result};

pub trait PathChecker {
    type Output;

    fn check_dir(self) -> Result<Self::Output>;

    fn check_file(self) -> Result<Self::Output>;
}

impl PathChecker for &Path {
    type Output = Self;

    fn check_dir(self) -> Result<Self::Output> {
        if self.exists() && self.is_dir() {
            return Err(Error {
                kind: ErrorKind::PathInvalid { path: self.into() },
                source: None,
            });
        }
        Ok(self)
    }

    fn check_file(self) -> Result<Self::Output> {
        if self.exists() && self.is_file() {
            return Err(Error {
                kind: ErrorKind::PathInvalid { path: self.into() },
                source: None,
            });
        }
        Ok(self)
    }
}

impl PathChecker for PathBuf {
    type Output = Self;

    fn check_dir(self) -> Result<Self::Output> {
        self.as_path().check_dir().map(PathBuf::from)
    }

    fn check_file(self) -> Result<Self::Output> {
        self.as_path().check_file().map(PathBuf::from)
    }
}

pub trait TryToStr<E> {
    fn try_to_str(&self) -> result::Result<&str, E>;
}

impl TryToStr<fmt::Error> for PathBuf {
    fn try_to_str(&self) -> FmtResult<&str> {
        self.to_str().ok_or(fmt::Error)
    }
}

impl TryToStr<Error> for PathBuf {
    fn try_to_str(&self) -> Result<&str> {
        self.to_str().ok_or(Error::new(ErrorKind::StrFromPath))
    }
}
