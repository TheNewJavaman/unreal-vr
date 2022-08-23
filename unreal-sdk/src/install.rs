use std::path::PathBuf;
use std::ptr;

use windows::core::HSTRING;
use windows::Win32::Foundation::{ERROR_MORE_DATA, NO_ERROR};
use windows::Win32::System::Registry::{HKEY, HKEY_LOCAL_MACHINE, KEY_QUERY_VALUE, RegOpenKeyExW, RegQueryValueExW};

use crate::error::{DynError, Win32Error};
use crate::version::UeVersion;

pub struct UeInstall {
    pub version: UeVersion,
    pub path: PathBuf,
}

impl UeInstall {
    /// A local clone
    fn from_path(path: PathBuf) -> Result<UeInstall, DynError> {
        let version = UeVersion::try_from(&path)?;
        Ok(UeInstall { version, path: path.to_path_buf() })
    }

    /// An installation (with source) from the Epic Games Store
    fn from_store(version: String) -> Result<UeInstall, DynError> {
        unsafe {
            let mut reg_key = HKEY::default();
            RegOpenKeyExW(
                HKEY_LOCAL_MACHINE,
                &HSTRING::from(format!(r"SOFTWARE\EpicGames\Unreal Engine\{}", version)),
                0,
                KEY_QUERY_VALUE,
                &mut reg_key,
            ).ok()?;
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
                    _ => return Err(Box::new(Win32Error::from(e)))
                }
            }
            let path = String::from_utf8(buffer)?;
            Self::from_path(PathBuf::from(path))
        }
    }
}
