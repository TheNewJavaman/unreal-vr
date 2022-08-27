use crate::error::{Catcher, Error, ErrorKind, Result};
use crate::path::TryToStr;
use crate::source::Engine;

pub fn whatever(engine: &Engine) -> Result<()> {
    let sln = include_str!("unreal_module_interface/UnrealModuleInterface.template.sln");
    let csproj = include_str!("unreal_module_interface/UnrealModuleInterface.template.csproj");
    let cs = include_str!("unreal_module_interface/UnrealModuleInterface.template.cs");

    let module_map_entries = &engine
        .modules
        .iter()
        .map(|it| {
            format!(
                r#"MyModuleMap["{}"] = new {}(MyReadOnlyTargetRules);"#,
                it.name, it.name
            )
        })
        .collect::<Vec<String>>()
        .join("        \n");
    let cs_gen = cs.replace("{ModuleMapEntries}", module_map_entries);
    let compile_includes_gen = &engine.modules.iter().map(|it| -> Result<&str> {
        let build_file: Result<&str> = it.build_file.try_to_str();
        Ok(&format!(r#"<Compile Include="{}"/>"#, build_file?))
    });
    let mut try_to_str_err = compile_includes_gen.filter(|it| it.is_err());
    if let Some(err) = try_to_str_err.next() {
        return Err(err.err().unwrap());
    }
    let compile_includes_gen = compile_includes_gen
        .map(|it| it.ok().unwrap())
        .collect::<Vec<&str>>()
        .join("        \n");
    let csproj_gen = csproj
        .replace("{CompileIncludes}", &compile_includes_gen)
        .replace("{EngineSourceDir}", {
            let it: Result<&str> = engine.source_dir.try_to_str();
            it?
        })
        .replace("{EngineBinariesDir}", {
            let it: Result<&str> = engine.binaries_dir.try_to_str();
            it?
        });
    let sln = sln.replace("{EngineSourceDir}", {
        let it: Result<&str> = engine.source_dir.try_to_str();
        it?
    });

    Ok(())
}

#[link(name = "UnrealModuleInterface")]
extern "C" {
    fn borrow_unreal_module_info(name: *const u16) -> *const u16;

    fn release_unreal_module_info(info_ptr: *const u16);
}

#[derive(Debug)]
pub struct UnrealModuleInfo {
    public_include_paths: Vec<String>,
    private_include_paths: Vec<String>,
    public_module_dependencies: Vec<String>,
    private_module_dependencies: Vec<String>,
}

impl TryFrom<*const u16> for UnrealModuleInfo {
    type Error = Error;

    fn try_from(wchars: *const u16) -> Result<Self> {
        let mut iter: WVecIter = WVecIter(&wchars);
        Ok(Self {
            public_include_paths: iter.next().ok_or(Error::new(ErrorKind::WVecInvalid))?,
            private_include_paths: iter.next().ok_or(Error::new(ErrorKind::WVecInvalid))?,
            public_module_dependencies: iter.next().ok_or(Error::new(ErrorKind::WVecInvalid))?,
            private_module_dependencies: iter.next().ok_or(Error::new(ErrorKind::WVecInvalid))?,
        })
    }
}

const TERMINATOR: u16 = 0x0000;

#[derive(Debug)]
struct WCharIter<'a>(&'a *const u16);

#[derive(Debug)]
struct WStringIter<'a>(&'a *const u16);

#[derive(Debug)]
struct WVecIter<'a>(&'a *const u16);

impl Iterator for WCharIter<'_> {
    type Item = u16;

    fn next(&mut self) -> Option<Self::Item> {
        match unsafe { **self.0 } {
            TERMINATOR => {
                self.0 = unsafe { &mut self.0.offset(1) };
                None
            }
            not_null => {
                self.0 = unsafe { &mut self.0.offset(1) };
                Some(not_null)
            }
        }
    }
}

impl Iterator for WStringIter<'_> {
    type Item = String;

    fn next(&mut self) -> Option<Self::Item> {
        match unsafe { **self.0 } {
            TERMINATOR => {
                self.0 = unsafe { &mut self.0.offset(1) };
                None
            }
            _ => {
                let string = WCharIter(&mut self.0).collect::<Vec<u16>>().as_slice();
                String::from_utf16(string).ok()
            }
        }
    }
}

impl Iterator for WVecIter<'_> {
    type Item = Vec<String>;

    fn next(&mut self) -> Option<Self::Item> {
        match unsafe { **self.0 } {
            TERMINATOR => {
                self.0 = unsafe { &mut self.0.offset(1) };
                None
            }
            _ => Some(WStringIter(&mut self.0).collect::<Vec<String>>()),
        }
    }
}
