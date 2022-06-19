use std::sync::Mutex;

use services::d3d::D3DService;
use services::open_xr::OpenXrService;
use services::pipe::PipeService;
use services::ue::UeService;

mod services;

/// This should only be initialized once, but must be an `Option` because `Mutex` has no const 
/// default, which services use
static mut CORE: Option<Core> = None;

/// Holds the mod's entire state
struct Core {
    d3d: Mutex<D3DService>,
    xr: Mutex<OpenXrService>,
    pipe: Mutex<PipeService>,
    ue: Mutex<UeService>,
}

/// Every type of mod-related error should be indexed here
enum CoreError {
    Unknown
}