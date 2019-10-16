# MyCode
This repository will contain a Lexer/Parser/Compiler/Virtual Machine that will be very modular, written in C (Linux / Windows).

With MyCode, you can create your own language, or just add anything you want to an existing.

This project will use my C-modular library available here: https://github.com/Spiikesan/C-modulaire

The programm can run a (very) small language with C like syntax, based on the work of Marc Feeley (Tiny-C).

Here is the BNF

``` BNF
<program> ::= <statement>
<statement> ::= "if" <paren_expr> <statement> |
                "if" <paren_expr> <statement> "else" <statement> |
                "while" <paren_expr> <statement> |
                "do" <statement> "while" <paren_expr> ";" |
                "{" { <statement> } "}" |
                <expr> ";" |
                ";"
<paren_expr> ::= "(" <expr> ")"
<comma_expr> ::= <expr> [, <expr>]+
<expr> ::= <test> | <id> "=" <expr>
<test> ::= <sum> | <sum> "<" <sum>
<sum> ::= <term> | <sum> "+" <term> | <sum> "-" <term>
<term> ::= <id> | <int> | <paren_expr> | <id> "(" <comma_expr> ? ")"
<id> ::= ["a" | "b" | "c" | "d" | ... | "z"]+
<int> ::= <an_unsigned_decimal_integer>
```
There is only 2 syscalls, char read() and write(char)

This line can be run and the word typed will be crypted with Caesar algorithm.

    { dec=2; char=97; while (char>96) {char=read(); if (char>96) { char = char + dec; if (char > 122) char = char - 26; if (char < 97) char = char + 26; } write(char);} }

Build the program (make)

Run it (./MyCode), paste this line and press return button

press CTRL+D twice to get the program running.

Wait for the "Running..." message...

Type your word and then return.

Enjoy !

[Work in progress]
