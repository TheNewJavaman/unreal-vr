use unreal_sdk::ue_sdk;

pub fn something() {
    let ue = ue_sdk! {
        env!("UE_HOME")
    };
}
