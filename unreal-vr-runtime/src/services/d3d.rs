/// Abstraction for the two Windows graphics APIs
pub trait D3DService {
    fn get_version() -> D3DVersion;
}

/// For now, target both D3D11 (more common) and D3D12 (more performant)
pub enum D3DVersion {
    D3D11,
    D3D12,
}
