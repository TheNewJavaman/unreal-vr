use std::fs::File;
use std::io::BufReader;
use std::path::PathBuf;

use serde_json::Value;

use crate::error::{Catcher, Error, ErrorKind, JsonKey, JsonType};

#[derive(Debug)]
pub struct UeVersion {
    major: u64,
    minor: u64,
    patch: u64,
}

macro_rules! try_get_version_prop {
    ($root:ident, $key:literal) => {
        $root
            .get($key)
            .ok_or(Error::new(ErrorKind::JsonKeyInvalid {
                key: $key.to_string(),
            }))?
            .as_u64()
            .ok_or(Error::new(ErrorKind::JsonTypeCast {
                key: JsonKey::Key($key.to_string()),
                to_type: JsonType::U64,
            }))
    };
}

impl TryFrom<&PathBuf> for UeVersion {
    type Error = Error;

    fn try_from(install: &PathBuf) -> Result<Self, Self::Error> {
        let path = install.join("Build/Build.version");
        let file = File::open(path.clone()).catch(ErrorKind::FileOpen { path: path.clone() })?;
        let reader = BufReader::new(file);
        let value: Value = serde_json::from_reader(reader).catch(ErrorKind::JsonInvalid {
            path: Some(path.clone()),
        })?;
        let root = value
            .as_object()
            .ok_or(Error::new(ErrorKind::JsonTypeCast {
                key: JsonKey::Root,
                to_type: JsonType::Object,
            }))?;
        Ok(UeVersion {
            major: try_get_version_prop!(root, "MajorVersion")?,
            minor: try_get_version_prop!(root, "MinorVersion")?,
            patch: try_get_version_prop!(root, "PatchVersion")?,
        })
    }
}
