use std::fs::File;
use std::io::BufReader;
use std::path::PathBuf;

use serde_json::Value;

use crate::error::{Error, JsonType};

#[derive(Debug)]
pub struct UeVersion {
    major: u64,
    minor: u64,
    patch: u64,
}

macro_rules! get_version_field {
    ($root:ident, $key:literal) => {
        $root
            .get($key)
            .ok_or(Error::JsonInvalidKey {
                key: $key.to_string(),
            })?
            .as_u64()
            .ok_or(Error::JsonTypeCast {
                to_type: JsonType::U64,
            })?
    };
}

impl TryFrom<&PathBuf> for UeVersion {
    type Error = Error;

    fn try_from(install: &PathBuf) -> Result<Self, Self::Error> {
        let path = install.join("Build/Build.version");
        let file = File::open(path.clone()).map_err(|source| Error::OpenFile {
            path: path.clone(),
            source,
        })?;
        let reader = BufReader::new(file);
        let value: Value = serde_json::from_reader(reader)
            .map_err(|source| Error::JsonFileInvalid { path, source })?;
        let root = value.as_object().ok_or(Error::JsonTypeCast {
            to_type: JsonType::Object,
        })?;
        Ok(UeVersion {
            major: get_version_field!(root, "MajorVersion"),
            minor: get_version_field!(root, "MinorVersion"),
            patch: get_version_field!(root, "PatchVersion"),
        })
    }
}
