# IRC Table Tags

Version 0.1, 24th June 2024.

## Introduction

It is useful for IRC bots and services pseudoclients to be able to show tabular data to other clients. Currently this is done by assuming that clients use a monospace font and then printing out whitespace to render text
in a way that looks like a table. This is non-ideal because it does not work with modern clients that use a variable width font.

This specification defines a backwards-compatible nethod of rendering tables on supporting clients that 

## Tags

### `+anope.org/table={start|end}`


Defines a the start or end of a table. If sent in a `NOTICE` or `PRIVMSG` then the message also includes a table header or table footer and MUST include an `+anope.org/table-row` entry. If sent in a `TAGMSG` then the table has no header or footer.

### `+anope.org/table-row=<start-index>-<end-index>[:<column-span>]:<row-span>]][,<start-index>-<end-index>[:<column-span>]:<row-span>]]]+`

Defines a table row.

- `start-index` &mdash; The byte index of the start of the column.
- `end-index` &mdash; The byte index of the end of the column.
- `column-span` &mdash; The number of columns that the field spans. Defaults to 1 if not specified.
- `row-span` &mdash; The number of rows that the field spans. Defaults to 1 if not specified.

Any data outside of a column is legacy formatting data and should be ignored.

## Examples

A table with no header or footer:

```plaintext
@+anope.org/table=start :NickServ!services@example.com TAGMSG Sadie
@+anope.org/table-row=0-7,13-17 :NickServ!services@example.com NOTICE Sadie :Account:     Sadie
@+anope.org/table-row=0-5,13-27 :NickServ!services@example.com NOTICE Sadie :Email:       sadie@sadie.moe
@+anope.org/table-row=0-10,13-27 :NickServ!services@example.com NOTICE Sadie :Registered: 20th April 1969
@+anope.org/table=end :NickServ!services@example.com TAGMSG Sadie

```
&nbsp;     | &nbsp;
---------- | ------
Account    | Sadie
Email      | sadie@sadie.moe
Registered | 20th April 1969
