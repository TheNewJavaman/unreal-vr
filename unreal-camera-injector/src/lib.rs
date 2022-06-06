use unreal_sdk::{ue_api, ue_sdk};

fn stuff() {
    let ue = ue_sdk::Sdk::new::<{ ue_sdk::Version::V4_0 }>();
}