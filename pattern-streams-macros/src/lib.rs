#![feature(proc_macro_quote)]

use proc_macro::{quote, TokenStream};

use syn::{Error, Lit, parse_macro_input, Token};
use syn::parse::{Parse, ParseStream};

#[proc_macro]
pub fn pattern(tokens: TokenStream) -> TokenStream {
    let body = parse_macro_input!(tokens as Pattern)
        .bytes
        .iter()
        .map(|byte| match byte {
            WildcardByte::Value(v) => quote! {
                pattern.push(WildcardByte::Value(#v));
            },
            WildcardByte::Wildcard => quote! {
                pattern.push(WildcardByte::Wildcard);
            }
        });
    quote! {
        {
            use pattern_streams::ps::{WildcardByte, WildcardPattern};
            let pattern: WildcardPattern = Default::default();
            #(body)*
            pattern
        }
    }
}

struct Pattern {
    bytes: Vec<WildcardByte>,
}

enum WildcardByte {
    Value(Lit),
    Wildcard,
}

impl Pattern {
    fn new() -> Pattern {
        Pattern {
            bytes: vec![],
        }
    }
}

impl Parse for Pattern {
    fn parse(stream: ParseStream) -> Result<Self, Error> {
        let mut pattern = Pattern::new();
        loop {
            let lookahead = stream.lookahead1();
            if lookahead.peek(Token![?]) {
                pattern.bytes.push(WildcardByte::Wildcard);
            } else if lookahead.peek(Lit) {
                pattern.bytes.push(WildcardByte::Value(stream.parse()?));
            } else {
                break;
            }
        }
        Ok(pattern)
    }
}