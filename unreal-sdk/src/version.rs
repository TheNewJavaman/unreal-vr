use std::fs::File;
use std::io::BufReader;
use std::path::PathBuf;

use serde_json::Value;

use crate::error::DynError;

pub struct UeVersion {
    major: u64,
    minor: u64,
    patch: u64,
}

impl TryFrom<&PathBuf> for UeVersion {
    type Error = DynError;

    fn try_from(install: &PathBuf) -> Result<Self, Self::Error> {
        let path = install.join(PathBuf::from("Build/Build.version"));
        let file = File::open(path)?;
        let reader = BufReader::new(file);
        let value: Value = serde_json::from_reader(reader)?;
        let root = value.as_object().expect("Expected Build.version root to be an object");
        Ok(UeVersion {
            major: root.get("MajorVersion").expect("Could not find major version")
                .as_u64().expect("Expected number (u64) for major version"),
            minor: root.get("MinorVersion").expect("Could not find minor version")
                .as_u64().expect("Expected number (u64) for minor version"),
            patch: root.get("PatchVersion").expect("Could not find patch version")
                .as_u64().expect("Expected number (u64) for patch version"),
        })
    }
}
