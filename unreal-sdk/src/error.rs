use std::fmt::{Display, Formatter};
use std::path::PathBuf;
use std::{error, fmt, result};

use windows::core::HRESULT;

use crate::error::ErrorKind::LateInit;

pub type Result<T> = result::Result<T, Error>;

#[derive(Debug)]
pub struct Error {
    pub kind: ErrorKind,
    pub source: Option<Box<dyn error::Error>>,
}

#[derive(Debug)]
pub enum ErrorKind {
    // Uninitialized; will be set after original error is mapped to this wrapper
    LateInit,

    // Strings
    StrFromOsStr,
    StrFromUtf8,

    // Filesystem
    PathExists { path: PathBuf },
    DirRead { path: PathBuf },
    FileOpen { path: PathBuf },
    FileRead { path: PathBuf },

    // Regex
    RegexInvalid { hint: String },

    // Serde/JSON
    JsonInvalid { path: Option<PathBuf> },
    JsonTypeCast { key: JsonKey, to_type: JsonType },
    JsonKeyInvalid { key: String },

    // Windows
    Win32 { code: HRESULT },
}

#[derive(Debug)]
pub enum JsonKey {
    Root,
    Key(String),
}

#[derive(Debug)]
pub enum JsonType {
    Object,
    U64,
}

impl Display for Error {
    fn fmt(&self, f: &mut Formatter<'_>) -> fmt::Result {
        use ErrorKind::*;
        match &self.kind {
            LateInit => write!(f, "Unitialized -- something is very wrong here :O"),

            StrFromOsStr => write!(f, "Couldn't create string from OS string"),
            StrFromUtf8 => write!(f, "Couldn't create string from UTF-8 byte buffer"),

            PathExists { path } => write!(f, r#"Path "{}" doesn't exist"#, path.try_fmt()?),
            DirRead { path } => write!(f, r#"Couldn't read dir "{}""#, path.try_fmt()?),
            FileOpen { path } => write!(f, r#"Couldn't open file "{}""#, path.try_fmt()?),
            FileRead { path } => write!(f, r#"Path "{}" doesn't exist"#, path.try_fmt()?),

            RegexInvalid { hint } => write!(f, r#"Failed to compile lazy regex "{}""#, hint),

            JsonInvalid { path } => match path {
                Some(path) => write!(f, r#"Couldn't parse JSON file "{}""#, path.try_fmt()?),
                None => write!(f, "Couldn't parse JSON"),
            },
            JsonTypeCast { key, to_type } => match key {
                JsonKey::Root => write!(f, "Couldn't type cast JSON root to {}", to_type),
                JsonKey::Key(key) => {
                    write!(f, r#"Could type cast JSON key "{}" to {}"#, key, to_type)
                }
            },
            JsonKeyInvalid { key } => write!(f, r#"Invalid JSON key "{}""#, key),

            Win32 { code } => write!(f, "HRESULT: 0d{}", code.0),
        }
    }
}

impl From<&dyn error::Error> for Error {
    fn from(source: &dyn error::Error) -> Self {
        Self {
            kind: LateInit,
            source: Some(Box::new(source)),
        }
    }
}

impl error::Error for Error {
    fn source(&self) -> Option<&(dyn error::Error + 'static)> {
        self.source.map(|ref it| **it)
    }

impl Display for JsonType {
    fn fmt(&self, f: &mut Formatter<'_>) -> fmt::Result {
        use JsonType::*;
        match self {
            Object => write!(f, "object"),
            U64 => write!(f, "u64"),
        }
    }
}

trait TryFmt {
    fn try_fmt(&self) -> result::Result<&str, fmt::Error>;
}

impl TryFmt for PathBuf {
    fn try_fmt(&self) -> result::Result<&str, fmt::Error> { self.to_str().ok_or(fmt::Error) }
}
