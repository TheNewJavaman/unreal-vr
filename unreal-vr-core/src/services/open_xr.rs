use services::d3d::D3DVersion;

/// Interface to send frames/receive tracking from VR HMDs; OpenXR performs substantially better vs.
/// OpenVR/SteamVR
pub struct OpenXrService {
    d3d_state: D3DState
}

impl OpenXrService {
    /// First stage of initialization:
    /// - Enumerate available extensions
    /// - Load debug extension
    /// - Load D3D extension; could be 11 or 12, depending on the user's preference
    pub fn begin_init(d3d_version: D3DVersion) -> Err{}

    /// Runtime support optional, but it's nice to see debug output in the console
    fn load_debug_ext() {}

    /// Runtime must support this or d3d12, depending on which D3D version the user specifies
    fn load_d3d11_ext() {}

    /// Runtime must support this or d3d11, depending on which D3D version the user specifies
    fn load_d3d12_ext() {}
}

/// Holds the graphics resources state for a specific implementation of D3D
enum D3DState {
    D3D11 {},
    D3D12 {},
}