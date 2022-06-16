use std::collections::HashMap;
use std::mem::size_of;
use std::ptr::addr_of_mut;
use std::sync::Mutex;

use windows::core::PCWSTR;
use windows::Win32::Foundation::HANDLE;
use windows::Win32::System::LibraryLoader::GetModuleHandleW;
use windows::Win32::System::ProcessStatus::{K32GetModuleInformation, MODULEINFO};
use windows::Win32::System::Threading::GetCurrentProcess;

#[derive(Debug, Copy, Clone)]
pub struct PtrInterval {
    start: *const u8,
    length: usize,
}

unsafe impl Send for PtrInterval {}

static MODULE_INTERVALS: Mutex<HashMap<&str, PtrInterval>> = Mutex::new(HashMap::new());
static PROC_HANDLE: Mutex<Option<HANDLE>> = Mutex::new(None);

pub fn get_proc_handle() -> HANDLE {
    let mut proc_handle = PROC_HANDLE.lock().unwrap();
    match *proc_handle {
        Some(v) => v,
        None => {
            let handle = unsafe { GetCurrentProcess() };
            *proc_handle = Some(handle);
            handle
        }
    }
}

pub fn get_module_interval(module: &str) -> PtrInterval {
    let mut module_intervals = MODULE_INTERVALS.lock().unwrap();
    match module_intervals.get(module) {
        Some(v) => v.clone(),
        None => {
            let handle = unsafe { GetModuleHandleW(PCWSTR::default()) }.unwrap();
            let module_info = MODULEINFO::default();
            unsafe {
                K32GetModuleInformation(
                    get_proc_handle(),
                    handle,
                    addr_of_mut!(module_info),
                    size_of::<MODULEINFO>() as u32,
                );
            }
            let interval = PtrInterval {
                start: handle.0 as *const u8,
                length: module_info.SizeOfImage as usize,
            };
            module_intervals.insert(String::from(module).as_str(), interval);
            interval
        }
    }
}