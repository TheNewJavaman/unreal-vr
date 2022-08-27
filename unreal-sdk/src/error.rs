use std::fmt::{Display, Formatter};
use std::path::PathBuf;
use std::string::FromUtf16Error;
use std::{error, fmt, io, result};

use windows;
use windows::core::HRESULT;
use windows::Win32::Foundation::WIN32_ERROR;

use crate::path::TryToStr;

pub type Result<T> = result::Result<T, Error>;

pub type FmtResult<T> = result::Result<T, fmt::Error>;

pub type RegexResult<T> = result::Result<T, regex::Error>;

#[derive(Debug)]
pub struct Error {
    pub kind: ErrorKind,
    pub source: Option<Box<dyn error::Error + 'static>>,
}

#[derive(Debug)]
pub enum ErrorKind {
    // Strings
    StrFromOsStr,
    StrFromUtf16,
    StrFromPath,

    // Filesystem
    PathInvalid { path: PathBuf },
    DirRead { path: PathBuf },
    FileOpen { path: PathBuf },
    FileRead { path: PathBuf },

    // FFI
    WVecInvalid,

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

pub trait Catcher<T, E> {
    fn catch(&self, kind: ErrorKind) -> Result<T>;

    fn catch_it<F>(&self, f: F) -> Result<T>
    where
        F: FnOnce(E) -> ErrorKind;
}

pub trait ErrorSource {
    fn as_source(&self) -> Option<Box<dyn error::Error>> {
        None
    }
}

impl<T, E> Catcher<T, E> for result::Result<T, E>
where
    E: ErrorSource,
{
    fn catch(&self, kind: ErrorKind) -> Result<T> {
        self.map_err(|source| Error {
            kind,
            source: source.as_source(),
        })
    }

    fn catch_it<F: FnOnce(E) -> ErrorKind>(&self, f: F) -> Result<T> {
        self.map_err(|source| Error {
            kind: f(source),
            source: source.as_source(),
        })
    }
}

macro_rules! error_source {
    ($err:ty : true) => {
        impl ErrorSource for $err {
            fn as_source(&self) -> Option<Box<dyn error::Error>> {
                Some(Box::new(self))
            }
        }
    };
    ($err:ty : false) => {
        impl ErrorSource for $err {
            fn as_source(&self) -> Option<Box<dyn error::Error>> {
                None
            }
        }
    };
}

macro_rules! error_sources {
    {$($err:ty:$lit: tt),+$(,)?} => {
        $(error_source!($err: $lit);)+
    };
}

error_sources! {
    FromUtf16Error: true,
    io::Error: true,
    regex::Error: true,
    serde_json::Error: true,
    windows::core::Error: false,
}

impl Error {
    pub fn new(kind: ErrorKind) -> Self {
        Self { kind, source: None }
    }
}

impl From<WIN32_ERROR> for Error {
    fn from(e: WIN32_ERROR) -> Self {
        Self {
            kind: ErrorKind::Win32 { code: e.into() },
            source: None,
        }
    }
}

impl Display for Error {
    fn fmt(&self, f: &mut Formatter<'_>) -> fmt::Result {
        use ErrorKind::*;
        match &self.kind {
            StrFromOsStr => write!(f, "Couldn't create string from OS string"),
            StrFromUtf16 => write!(f, "Couldn't create string from UTF-16 byte buffer"),
            StrFromPath => write!(f, "Couldn't create string from path"),

            PathInvalid { path } => write!(f, r#"Invalid path "{}""#, path.try_to_str()?),
            DirRead { path } => write!(f, r#"Couldn't read dir "{}""#, path.try_to_str()?),
            FileOpen { path } => write!(f, r#"Couldn't open file "{}""#, path.try_to_str()?),
            FileRead { path } => write!(f, r#"Couldn't read file "{}""#, path.try_to_str()?),

            WVecInvalid => write!(f, "Vec<Vec<String>> was invalid"),

            RegexInvalid { hint } => write!(f, r#"Failed to compile regex "{}""#, hint),

            JsonInvalid { path } => match path {
                Some(path) => write!(f, r#"Couldn't parse JSON file "{}""#, path.try_to_str()?),
                None => write!(f, "Couldn't parse JSON"),
            },
            JsonTypeCast { key, to_type } => match key {
                JsonKey::Root => write!(f, "Couldn't cast JSON root to type {}", to_type),
                JsonKey::Key(key) => {
                    write!(f, r#"Couldn't cast JSON key "{}" to type {}"#, key, to_type)
                }
            },
            JsonKeyInvalid { key } => write!(f, r#"Invalid JSON key "{}""#, key),

            Win32 { code } => write!(f, "Win32 error; HR 0d{}", code.0),
        }
    }
}

impl error::Error for Error {
    fn source(&self) -> Option<&(dyn error::Error + 'static)> {
        self.source.map(|it| &*it)
    }
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
