#![cfg(test)]

use crate::ue_sdk;

fn test_local_clone() {
    ue_sdk! {
        path: r"D:\Unreal Engine\EpicGames",
        modules: ["Core", "CoreUObject", "D3D11RHI"],
    }
}

#[test]
fn test_store_installation() {
    ue_sdk! {
        version: "4.27",
        modules: ["Core", "CoreUObject", "D3D11RHI"],
    }
}
