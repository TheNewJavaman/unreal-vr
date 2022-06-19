use ::{Core, CoreError};
use services::d3d::D3DVersion;

/// Interface to send frames/receive tracking from VR HMDs; OpenXR performs substantially better vs.
/// OpenVR/SteamVR
pub struct OpenXrService {
    d3d_state: D3DState,
}

impl OpenXrService {
    /// First stage of initialization:
    /// - Enumerate available extensions
    /// - Load debug extension
    /// - Load D3D extension; could be 11 or 12, depending on the user's preference
    pub fn begin_init(d3d_version: D3DVersion) -> Result<(), CoreError> { todo!() }

    /// Runtime support optional, but it's nice to see debug output in the console
    fn load_debug_ext() -> Result<(), CoreError> { todo!() }

    fn load_d3d11_ext() -> Result<(), CoreError> { todo!() }

    fn load_d3d12_ext() -> Result<(), CoreError> { todo!() }

    /// Second stage of initialization:
    /// - Create session
    /// - Enumerate views (eyes)
    /// - Create D3D resources and store in a `D3DState`
    /// - Begin session
    /// - Locate views once to get data for projection view calculations
    pub fn finish_init() -> Result<(), CoreError> { todo!() }

    fn create_session() -> Result<(), CoreError> { todo!() }

    fn enumerate_views() -> Result<(), CoreError> { todo!() }

    fn create_d3d_state() -> Result<(), CoreError> { todo!() }

    fn begin_session() -> Result<(), CoreError> { todo!() }

    fn store_proj_view_data() -> Result<(), CoreError> { todo!() }
}

/// Holds the graphics resources state for a specific implementation of D3D
enum D3DState {
    D3D11 {},
    D3D12 {},
}