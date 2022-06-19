use services::d3d::{D3DService, D3DVersion};

/// Direct3D11 service; older, more common API, but often worse performance vs. D3D12
pub struct D3D11Service {}

impl D3DService for D3D11Service {
    fn get_version() -> D3DVersion {
        D3DVersion::D3D11
    }
}