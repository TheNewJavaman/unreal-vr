use std::collections::HashMap;
use std::mem::size_of;
use std::ptr::addr_of_mut;
use std::sync::Mutex;

use lazy_static::lazy_static;
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

impl Iterator for PtrInterval {
    type Item = *const u8;

    fn next(&mut self) -> Option<Self::Item> {
        if self.length == 0 {
            None
        } else {
            let ptr = unsafe { self.start.offset(1) };
            self.start = ptr;
            self.length -= 1;
            Some(ptr)
        }
    }
}

lazy_static! {
    static ref PROC_HANDLE: Mutex<Option<HANDLE>> = Mutex::new(None);
    static ref MODULE_INTERVALS: Mutex<HashMap<Option<String>, PtrInterval>> = Mutex::new(HashMap::new());
}

pub fn get_proc_handle() -> HANDLE {
    let mut proc_handle = PROC_HANDLE.lock().unwrap();
    match *proc_handle {
        Some(v) => v,
        None => {
            let handle = unsafe { GetCurrentProcess() };
            if handle.is_invalid() {
                panic!("Current process handle must be valid");
            }
            *proc_handle = Some(handle);
            handle
        }
    }
}

pub fn get_module_interval(module: Option<&str>) -> PtrInterval {
    let mut module_intervals = MODULE_INTERVALS.lock().unwrap();
    match module_intervals.get(&module.map(String::from)) {
        Some(v) => *v,
        None => {
            let handle = if let Some(v) = module {
                unsafe { GetModuleHandleW(v).unwrap() }
            } else {
                unsafe { GetModuleHandleW(PCWSTR::default()).unwrap() }
            };
            let mut module_info = MODULEINFO::default();
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
            module_intervals.insert(module.map(String::from), interval);
            interval
        }
    }
}