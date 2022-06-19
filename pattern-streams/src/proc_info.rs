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

struct ProcInfo {
    proc_handle: Mutex<Option<HANDLE>>,
    module_intervals: Mutex<HashMap<Option<String>, PtrInterval>>,
}

impl ProcInfo {
    fn new() -> Self {
        ProcInfo {
            proc_handle: Mutex::new(None),
            module_intervals: Mutex::new(HashMap::new()),
        }
    }
}

static mut PROC_INFO: Option<ProcInfo> = None;

pub fn init() {
    unsafe {
        PROC_INFO = Some(ProcInfo::new());
    }
}

pub fn get_proc_handle() -> HANDLE {
    let mut proc_handle = unsafe { PROC_INFO.unwrap() }.proc_handle.lock().unwrap();
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
    let mut module_intervals = unsafe { PROC_INFO.unwrap() }.module_intervals.lock().unwrap();
    match module_intervals.get(&module.map(|m| String::from(m))) {
        Some(v) => v.clone(),
        None => {
            let handle = if let Some(v) = module {
                unsafe { GetModuleHandleW(v) }.unwrap()
            } else {
                unsafe { GetModuleHandleW(PCWSTR::default()) }.unwrap()
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
            module_intervals.insert(module.map(|m| String::from(m)), interval);
            interval
        }
    }
}