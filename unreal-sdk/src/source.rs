use std::collections::{BTreeSet, HashSet};
use std::fs::File;
use std::hash::Hash;
use std::io::Read;
use std::path::{Path, PathBuf};
use std::{ptr, result};

use lazy_static::lazy_static;
use regex::Regex;
use windows::core::HSTRING;
use windows::Win32::Foundation::{ERROR_MORE_DATA, NO_ERROR};
use windows::Win32::System::Registry::{
    RegOpenKeyExW,
    RegQueryValueExW,
    HKEY,
    HKEY_LOCAL_MACHINE,
    KEY_QUERY_VALUE,
};

use crate::error::{Catcher, Error, ErrorKind, RegexResult, Result};
use crate::path::PathChecker;
use crate::version::UeVersion;

#[derive(Debug)]
pub struct Engine {
    pub version: UeVersion,
    pub root_dir: PathBuf,
    pub binaries_dir: PathBuf,
    pub build_dir: PathBuf,
    pub source_dir: PathBuf,
    pub modules: HashSet<Module>,
}

#[derive(Debug, PartialEq, Eq, Hash)]
pub struct Module {
    pub name: String,
    pub root_dir: PathBuf,
    pub build_file: PathBuf,
    pub header_files: BTreeSet<PathBuf>,
    pub dependencies: BTreeSet<String>,
}

/// Metadata for an Unreal Engine installation or repository clone
impl Engine {
    /// A local repository clone
    fn from_path(path: &Path, modules: HashSet<&str>) -> Result<Engine> {
        let build_dir = path.join("Engine/Build").check_dir()?;
        let source_dir = path.join("Engine/Source/Runtime").check_dir()?;
        let mut engine_modules: HashSet<Module> = HashSet::new();
        for module in modules.iter() {
            if !engine_modules.iter().any(|it| it.name == *module) {
                Module::add_to(&mut engine_modules, &source_dir, module)?;
            }
        }
        Ok(Engine {
            version: UeVersion::try_from(&build_dir)?,
            root_dir: path.check_dir()?.into(),
            binaries_dir: path.join("Engine/Build").check_dir()?,
            build_dir,
            source_dir,
            modules: engine_modules,
        })
    }

    /// A local repository cloned
    fn from_path_str(path: &str, modules: HashSet<&str>) -> Result<Engine> {
        Self::from_path(&Path::new(path), modules)
    }

    /// An Epic Games Store installation. Note: you *must* enable source in the
    /// "Options" menu
    fn from_store(version: &str, modules: HashSet<&str>) -> Result<Engine> {
        unsafe {
            let mut reg_key = HKEY::default();
            RegOpenKeyExW(
                HKEY_LOCAL_MACHINE,
                &HSTRING::from(format!(r"SOFTWARE\EpicGames\Unreal Engine\{}", version)),
                0,
                KEY_QUERY_VALUE,
                &mut reg_key,
            )
            .ok()
            .catch_it(|it| ErrorKind::Win32 { code: it.code() })?;
            const REG_BUFFER_SIZE: u32 = 512;
            let mut buffer = vec![];
            let mut tmp_buffer = Vec::with_capacity(REG_BUFFER_SIZE as usize);
            loop {
                let mut tmp_buffer_size = REG_BUFFER_SIZE;
                let e = RegQueryValueExW(
                    reg_key,
                    &HSTRING::from("InstalledDirectory"),
                    ptr::null_mut(),
                    ptr::null_mut(),
                    tmp_buffer.as_mut_ptr(),
                    &mut tmp_buffer_size,
                )
                .ok();
                match e {
                    Ok(_) => {
                        buffer.extend_from_slice(&tmp_buffer[0..tmp_buffer_size as usize]);
                        break;
                    }
                    Err(_) => match e {
                        ERROR_MORE_DATA => {
                            buffer.append(&mut tmp_buffer);
                            continue;
                        }
                        _ => Error::from(e),
                    },
                }
            }
            let path = String::from_utf8(buffer).catch(ErrorKind::StrFromUtf8)?;
            Self::from_path(&Path::new(&path), modules)
        }
    }
}

macro_rules! lazy_regex {
    ($pat:literal) => {{
        lazy_static! {
            static ref RE: RegexResult<Regex> = Regex::new($pat);
        }
        RE.catch(ErrorKind::RegexInvalid {
            hint: $pat.to_string(),
        })?
    }};
}

impl Module {
    fn add_to(modules: &mut HashSet<Module>, source_dir: &Path, name: &str) -> Result<()> {
        let root_dir = source_dir.join(name).check_dir()?;
        let build_file = root_dir.join(format!("{}.build.cs", name)).check_file()?;
        let mut file = File::open(build_file).catch(ErrorKind::FileOpen {
            path: build_file.into(),
        })?;
        let mut contents = String::new();
        file.read_to_string(&mut contents)
            .catch(ErrorKind::FileRead {
                path: build_file.into(),
            })?;
        let mut header_files = HashSet::new();
        Self::find_header_files(&mut header_files, &root_dir)?;
        modules.insert(Self {
            name: name.into(),
            root_dir,
            build_file: build_file.clone(),
            header_files: Default::default(),
            dependencies: Default::default(),
        });
        Ok(())
    }

    fn find_header_files(header_files: &mut HashSet<PathBuf>, dir: &Path) -> Result<()> {
        let entries = dir.read_dir().map_err(|source| Error::ReadDir {
            path: dir.into(),
            source,
        })?;
        for entry in entries {
            let entry = entry
                .map_err(|source| Error::ReadDir {
                    path: dir.into(),
                    source,
                })?
                .path();
            if entry.is_dir() {
                Self::find_header_files(header_files, &entry)?;
            } else if entry.is_file() {
                if let Some(extension) = entry.extension() {
                    let extension = extension.to_str().ok_or(Error::StringFromOsStr {
                        os_str: Box::from(extension),
                    })?;
                    if extension == "h" || extension == "hpp" {
                        // TODO: Process header using bindgen
                        header_files.insert(entry);
                    }
                }
            }
        }
        Ok(())
    }
}
