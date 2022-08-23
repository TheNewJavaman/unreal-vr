use proc_macro::TokenStream;

use syn::DeriveInput;

//#[proc_macro]
//pub fn ue_sdk(input: TokenStream) -> TokenStream {
//// get default
//// or path
//// generate for specific modules
//}
//
//fn impl_ue_sdk(ast: &DeriveInput) -> TokenStream {
//    let ue_path = &ast.ident
//    quote::quote! {
//        impl HelloMacro for #name {
//            fn hello_macro() {
//                println!("Hello, Macro! My name is {}!", stringify!(#name));
//            }
//        }
//    }.into()
//}
//
//fn parse_attr(input: &TokenStream) -> Option<TokenStream> {}
