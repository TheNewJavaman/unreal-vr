#![feature(proc_macro_quote)]

use proc_macro::{quote, TokenStream};

use syn::{Error, Lit, parse_macro_input, Token};
use syn::parse::{Parse, ParseStream};

#[proc_macro]
pub fn pattern(tokens: TokenStream) -> TokenStream {
    let body = parse_macro_input!(tokens as Pattern)
        .bytes
        .iter()
        .map(|byte| {
            match byte {
                PatternByte::Value(v) => quote! {
                    pattern.push(PatternByte::Value(#v));
                },
                PatternByte::Wildcard => quote! {
                    pattern.push(PatternByte::Wildcard);
                }
            }
        });
    quote! {
        {
            use pattern_streams::ps::{PatternByte, WildcardPattern};
            let pattern: WildcardPattern = Default::default();
            #(body)*
            pattern
        }
    }
}

struct Pattern {
    bytes: Vec<PatternByte>,
}

enum PatternByte {
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
                pattern.bytes.push(PatternByte::Wildcard);
            } else if lookahead.peek(Lit) {
                pattern.bytes.push(PatternByte::Value(stream.parse()?));
            } else {
                break;
            }
        }
        Ok(pattern)
    }
}