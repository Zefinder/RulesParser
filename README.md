# Rules Parser

This repository is an example of simple rule parsing for a small IDS (Intrusion Detection System). It contains: 
- A flex lexer 
- A bison parser
- A CSV formatter in C (to transfer information to Python)
- A C code generator in Python
- An IDS that can parse a specific type of packets (see below)

## The parser

The parser is designed to parse "stateless" and "stateful" rules. A stateless rule is a rule that only has a measure (A -> Alert, D -> Drop). A stateful rule is a succession of rule separated by a semicolon that has a start state and an action (either a state or a measure). A rule is composed of a body, being a message id and a function code. A message id must be a 16 bits integer with high byte being 08h, 18h, 09h or 19h. A rule can have a parameter (optional) representing the packet value, being an offset in the packet (in bits), the value type, the value length in the packet (in bits) and the value. Below is the syntax for a simple rule, for more details look at the example or the valid inputs in the `tests` directory:

`<header> <message_id> <function_code> [<offset>:<type>:<length> <value>]`

Examples: 
- `D 0x09A5 1`
- `E0|E1 0x18FF 78 ; E1|A 0x0930 4 256:STRING:24 NO ; E1|E0 0x0930 4 256:STRING:24 YES`
- `A 0x1977 3 128:INT:32 5000`

## IDS Packet

The packet for the IDS is made as such: `<message_id upper byte> <message_id lower byte> XX XX XX XX <function code byte> <offset + value>` (`XX` represents any value).