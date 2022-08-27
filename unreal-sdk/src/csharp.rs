use std::path::PathBuf;

pub mod unreal_module_interface;

#[derive(Debug)]
pub struct CSharpBuild {
    sln_file: PathBuf,
    csproj_files: Vec<PathBuf>,
    cs_files: Vec<PathBuf>,
}
