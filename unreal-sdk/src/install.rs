use std::collections::{BTreeSet, HashSet};
use std::fs::{File};
use std::hash::Hash;
use std::io::Read;
use std::path::{Path, PathBuf};
use std::ptr;
use std::result;

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

use crate::check_dir;
use crate::error::{Error,ErrorKind, Result};
use crate::version::UeVersion;

#[derive(Debug)]
pub struct Engine {
    pub version: UeVersion,
    pub root_dir: PathBuf,
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

macro_rules! check_dir {
    ($path:expr) => {{
        if !$path.exists() && $path.is_dir() {
            return Err(ErrorKind::PathExists { path: $path.into() });
        }
        $path
    }};
}

macro_rules! maybe_throw {
    ($ty:item, $e:ident) => {
        Error {
            kind: $ty,
            ..$e
        }
    }
}

/// Metadata for an Unreal Engine installation or repository clone
impl Engine {
    /// A local repository clone
    fn from_path(path: &Path, modules: HashSet<&str>) -> Result<Engine> {
        let a = Error { kind: ErrorKind::LateInit, source: None };
        let b = maybe_throw!(ErrorKind::LateInit, a);
        let build_dir = check_dir!(path.join("Engine/Build"));
        let source_dir = check_dir!(path.join("Engine/Source"));
        let mut engine_modules: HashSet<Module> = HashSet::new();
        for module in modules.iter() {
            if !engine_modules.iter().any(|it| it.name == *module) {
                let module = Module::new(&source_dir, &mut engine_modules, module)?;
                engine_modules.insert(module);
            }
        }
        Ok(Engine {
            version: UeVersion::try_from(&build_dir)?,
            root_dir: check_dir!(path).into(),
            build_dir,
            source_dir,
            modules: engine_modules,
        })
    }

    /// A local repository clone
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
            .ok()?;
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
                );
                match e {
                    NO_ERROR => {
                        buffer.extend_from_slice(&tmp_buffer[0..tmp_buffer_size as usize]);
                        break;
                    }
                    ERROR_MORE_DATA => {
                        buffer.extend_from_slice(&tmp_buffer[0..tmp_buffer_size as usize]);
                        continue;
                    }
                    _ => {
                        return Err(match e.ok() {
                            Ok(_) => unreachable!(),
                            Err(e) => e.into(),
                        });
                    }
                }
            }
            let path = String::from_utf8(buffer)?;
            Self::from_path(&Path::new(&path), modules)
        }
    }
}

lazy_static! {
static ref RE: result::Result<Regex, regex::Error> = Regex::new(r#"DependencyModuleNames\.Add.*?\(.*?(?:"(.*?)".*?)+\)"#);
}

impl Module {
    fn add_to(modules: &mut HashSet<Module>, source_dir: &Path, name: &str) -> Result<()> {
        let root_dir = check_dir!(source_dir.join(name));
        let build_file = check_dir!(root_dir.join(format!("{}.build.cs", name)));

        let file = File::open(build_file).map_err(|source| Error::OpenFile { path: build_file.into(), source })?;

        let mut contents = String::new();
        file.read_to_string(&mut contents).map_err(|source| Error::Read)?;
        let captures = RE.captures_iter()
        if RE?.is_match(build_file)
        let mut header_files = HashSet::new();
        Self::find_header_files(&mut header_files, &root_dir)?;
        modules.insert(Self {
            name: name.into(),
            root_dir,
            build_file,
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
