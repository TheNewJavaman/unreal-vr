use std::collections::HashMap;
use std::fs::File;
use std::path::Path;

use crate::error::{Catcher, Error, ErrorKind, Result};

pub struct Env(HashMap<String, String>);

impl TryFrom<&Path> for Env {
    type Error = Error;

    fn try_from(path: &Path) -> Result<Self> {
        check_file!(path);
        let file = File::open(path).catch(ErrorKind);
    }
}
