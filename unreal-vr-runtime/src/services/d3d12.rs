use services::d3d::{D3DService, D3DVersion};

/// Direct3D11 service; newer, lower-level API, but often better performance vs. D3D12
pub struct D3D12Service {}

impl D3DService for D3D12Service {
    fn get_version() -> D3DVersion {
        D3DVersion::D3D12
    }
}
