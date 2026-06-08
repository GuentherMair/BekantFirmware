# The retlw lookup table at 0x1046-0x10D7

We believe the OEM firmware stores a 141-byte lookup table in
flash as a `retlw` dispatch. A function at 0x1046 appears to
take a 1-byte index in `WREG`, subtract 1, and use the result
to compute a `PCLATH` offset for a series of `retlw <byte>`
instructions. Each `retlw` returns one byte of the table to
the caller.

## The dispatch function

```
0x1046:  clrwdt                ; clear watchdog
0x1047:  clrf    0x00          ; WREG = 0
0x1048:  addfsr  4, .1         ; FSR1 += 1 (advance to next byte)
0x1049:  decfsz  0x09, 0x1     ; decrement count in 0x09
0x104A:  goto    0x0047        ; loop
0x104B:  retlw   0x00          ; first byte returned
0x104C:  retlw   0x2c
...
0x10D7:  retlw   0x01          ; last byte returned
```

The function appears to be called with `0x09` (count) set
to the index of the byte to fetch. The first `retlw` is at
0x104B; subsequent ones are at 0x104C, 0x104D, etc.

## The data

The full 141-byte table is recovered byte-for-byte in
`bekant/oem_tables.c`. The structure is what we *think* it
is — none of the field meanings have been verified against a
running controller:

### Section 1: header (0x104B-0x105F, 21 bytes)

```
0x00, 0x2c, 0x38, 0x80, 0x81, 0x08, 0xc1, 0x80, 0x03, 0x50, 0x80, 0x03,
0x11, 0x81, 0x03, 0x92, 0x81, 0x03, 0x80, 0x00, 0x80,
```

The `0x80` and `0x81` look like sentinel values that
delimit records. The first 3 bytes (`0x00 0x2c 0x38`)
*might* be a magic/version number; the rest looks like the
start of the EEPROM image.

### Section 2: ASCII IDs (0x1069-0x1080, 24 bytes)

```
0x45, 0x52, 0x01, 0x00, 0x41, 0x02, 0x01, 0x01,  ; "ER\x01\x00A\x02\x01\x01"
0x00, 0x00, 0x00, 0x00,
0x45, 0x52, 0x01, 0x00, 0x46, 0x03, 0x09, 0x09,  ; "ER\x01\x00F\x03\x09\x09"
0x00, 0x00, 0x00, 0x00,
```

`0x45 0x52` is ASCII `"ER"`. The two records look like:

1. `"ER"` + record-type `0x01` + length `0x0000` + data
   `"A\x02\x01\x01"`.
2. `"ER"` + record-type `0x01` + length `0x0000` + data
   `"F\x03\x09\x09"`.

These look like *EEPROM page identifiers* — Intel HEX-style
record types. The `0x01` after `"ER"` would be a "record
type 01" (EOF in Intel HEX). The data that follows is then
non-standard.

### Section 3: EEPROM image (0x1081-0x10D7, 87 bytes)

```
0x01, 0x00, 0x11,
0x02, 0x00,
0x50, 0x03, 0x00, 0x92,
0x04, 0x00,
0x11, 0x05, 0x00, 0xc1,
0x7d, 0x00,
0x00, 0x08, 0xe8,
0x03, 0x11, 0x00,
0x80, 0x03, 0x32, 0x00,        <-- start of repeating pattern
0x50, 0x00, 0x80, 0x03, 0x32, 0x00,  (×12)
0xc1, 0x00, 0x80, 0x03,
0x32, 0x00, 0x92, 0x00,
0x80, 0x03, 0xf4, 0x01,
```

The repeating pattern `0x80 0x03 0x32 0x00` *appears* to be
an EEPROM write unlock sequence:

- `0x80` to `EECON2` (register 0x197 on PIC16LF1938)
- `0x03` is the next byte
- `0x32` is the data byte to write to `EEDATA`
- `0x00` is a separator

This *could* be the PIC16LF1938's EEPROM programming
sequence: write `0x55` to EECON2, then `0xAA`, then set the
`WR` bit. We *guess* that the OEM firmware uses the
`0x80 0x03` pattern as a *macro-instruction* that means
"write byte 0x32 to EEPROM".

So the entire table at 0x1081+ might be a sequence of
EEPROM-write macros:

| Macro       | Bytes  | What we think it does          |
| ----------- | ------ | ------------------------------ |
| `0x80 0x03` | header | EEPROM write preamble          |
| `0x32 0x00` | data   | Data byte 0x32, then separator |
| `0x50 0x00` | ???    | Unknown                        |

If our reading is right, this *could* be the OEM EEPROM
image that the firmware writes on first boot. The 87 bytes
of "data" would then be the actual EEPROM contents. The
four default positions (0x0021, 0x00a0, 0x0120, 0x01d2) are
written by what we believe to be separate function calls
at 0x0221-0x0242 (recovered as `eeprom_init_defaults()`), and
the rest of the EEPROM is initialised from this table.

### Section 4: tail records (0x10CC-0x10D7, 12 bytes)

```
0xc1, 0x00, 0x80, 0x03,
0x32, 0x00, 0x92, 0x00,
0x80, 0x03, 0xf4, 0x01,
```

These look like the end-of-table marker and a final
checksum. The exact semantics are not recovered.

## How we think the table is used

The function at 0x1046 appears to be called from the EEPROM
init sequence at 0x0221 and the factory-reset handler. The
caller specifies which byte to fetch; the function returns
it in `WREG`. The caller then writes `WREG` to the EEPROM
data register `EEDATA`.

The dispatch looks like it goes via PCLATH (computed goto),
which is the standard PIC16 way to implement a switch-on-byte.
The C equivalent is a `switch` statement or a `const` array.

## What is NOT recovered

The exact meaning of each byte in the table is not fully
decoded. The repeating `0x80 0x03 0x32 0x00` pattern looks
like an EEPROM-write macro, but the surrounding bytes
(0x50, 0x11, 0xc1, 0x92, etc.) have uncertain meaning.

A full reverse engineering of this table would require
comparing the EEPROM image byte-by-byte against a working
BEKANT controller's EEPROM, then mapping each byte to the
firmware variable it represents. This is beyond the scope
of a static reverse engineering pass.

## See also

- `bekant/oem_tables.c` — the byte-for-byte transcription
- `bekant/eeprom_defaults.c` — the EEPROM init function
  that consumes this table
- `docs/FACTORY_RESET.md` — the factory-reset path that
  re-uses this table
