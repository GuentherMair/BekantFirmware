# IMPORTANT

Analysis of the original OEM firmware (disassembly and rebuild in C) was done
using the MiniMax-M3 LLM and this result is as yet COMPLETELY untested, while
I am waiting for the PIC programmer.

If you want to give it a try, clear the path of the desk and be ready to cut
power in case of a malfunction.

**- USE - AT - YOUR - OWN - RISK -**


# BEKANT Firmware

![Position memory diagram](https://github.com/ivanwick/bekantfirmware/wiki/images/diagram.png)

Control firmware for IKEA BEKANT adjustable-height desk with lower and
upper memory positions. Can be flashed onto the OEM controller,
without changing any hardware.

Pre-program a sitting and standing position for your desk and move
between them with a single button press.

## About this fork

The original release at
[ivanwick/bekantfirmware](https://github.com/ivanwick/bekantfirmware)
adds position memory to the OEM controller. The
[DieSteinhose/bekantfirmware](https://github.com/DieSteinhose/bekantfirmware)
fork changes the gesture you use to invoke a memory position.

This project:

1. **Supports both gesture styles at the same time.** You can use
   the ivanwick "combo-press" (hold one button, tap the other), the
   DieSteinhose "double-tap", or mix and match. The unified state
   machine in `btn/btn.c` handles both.
2. **Includes a best-effort C reconstruction** of the original IKEA
   firmware in `oem_reconstruction/`, for future
   reference and to verify the endstop behaviour described in
   [ivanwick issue #4](https://github.com/ivanwick/bekantfirmware/issues/4).

The rest of the source is mostly identical to the ivanwick project
(the original release), with these additions / fixes:

- **`btn/`** rewritten to support both gesture styles at the same time.
- **`bekant/bctrl.{c,h}`** now uses the OEM BCMD values recovered
  from the disassembly (0xfc/0x50/0x49/0xca/0x8b/0x4c/0x0d/0x8e/0xcf).
  See [docs/ENDSTOP_ANALYSIS.md](docs/ENDSTOP_ANALYSIS.md) for the
  BCMD table.
- **`bekant/endstop.{c,h}`** is the recovered endstop / over-travel
  detector (issue #4 fix); it is registered from `user.c::InitApp()`
  and ticked once per scheduler slot from `bctrl_timer()`.
- **`interrupts.c`** declares the ISR with the `__interrupt()`
  qualifier — without it, XC8 free mode dead-strips the function
  and the interrupt vector stays empty.

## Quick links

- **[USAGE.md](USAGE.md)** — end-user docs: which buttons do what
- **[docs/COMPARISON.md](docs/COMPARISON.md)** — how the two upstreams differ and how this project handles both
- **[docs/TRACE.md](docs/TRACE.md)** — state-machine traces you can audit by hand
- **[docs/ENDSTOP_ANALYSIS.md](docs/ENDSTOP_ANALYSIS.md)** — endstop / over-travel analysis (issue #4)
- **[oem_reconstruction/](oem_reconstruction/)** — reconstructed C code of the original IKEA firmware, with the disassembly

## Project layout

```
BekantFirmware/
├── README.md
├── USAGE.md
├── docs/
│   ├── COMPARISON.md
│   ├── DELIVERABLE.md
│   ├── ENDSTOP_ANALYSIS.md
│   └── TRACE.md
├── oem_reconstruction/                ← reverse-engineered reference impl
│   ├── README.md
│   ├── ANALYSIS.md
│   ├── main.c / system.c / user.c / interrupts.c / configuration_bits.c
│   ├── btn/ lin/ bekant/
│   ├── bekant/endstop.c                ← recovered endstop detector
│   ├── bekant/bcmd_table.c / bcmd_table.h / oem_tables.c
│   ├── docs/                           ← STATE_MACHINE / ENCODER_TABLE / FACTORY_RESET
│   └── disassembly/                    ← orginafirm.hex + .asm dumps
└── src/
    ├── main.c
    ├── configuration_bits.c
    ├── system.c / system.h
    ├── user.c / user.h
    ├── interrupts.c                    ← void __interrupt() isr(void) (the ISR fix)
    ├── Makefile
    ├── btn/btn.h                        ← INPUT_t enum, gesture documentation
    ├── btn/btn.c                        ← unified state machine (both gestures)
    ├── lin/lin_d.c / lin_d.h            ← LIN driver
    ├── bekant/bscan.c / bscan.h         ← startup bus scan
    ├── bekant/bctrl.c / bctrl.h         ← LIN control plane (now uses OEM BCMD values)
    ├── bekant/bui.c / bui.h             ← upper-level UI
    ├── bekant/endstop.c / endstop.h     ← recovered endstop detector, wired in
    └── test_sfr_stubs.h / .c            ← syntax-only shim (real build uses src/ directly)
```

## Building and uploading

These instructions assume a Linux host, an MPLAB PICkit 3 (or 4)
programmer, and a fresh out-of-the-box BEKANT controller that you
are about to overwrite. **Flashing the firmware is irreversible
without a backup of the original `.hex`**, so read the [disclaimer](#disclaimer)
first.

> **Before you start** — please read [Hardware variants & code protection](#hardware-variants--code-protection)
> below. Newer BEKANT desks (manufactured 2019 or later) have code
> protection enabled on the PIC, which means you cannot read the
> original firmware back from the chip. If yours is a newer unit,
> back up with a known-good dump instead (one is bundled in the
> ivanwick project) and accept that you may not be able to roll
> back to the OEM firmware.

### 0. Disassemble the controller box

The hardest part of the whole install is opening the controller
box. IKEA has progressively welded the back plate with more
ultrasonic spots to deter modifications like this one, so do not
expect the case to come apart easily. Two excellent community
guides for getting inside the metal case without destroying it:

- YouTube walkthrough: <https://www.youtube.com/watch?v=jCPlM2KYwDQ>
- iFixit teardown: <https://www.ifixit.com/Teardown/Ikea+Bekant+Controller+Teardown/142516>

These were originally collected in
[ivanwick/bekantfirmware issue #11](https://github.com/ivanwick/bekantfirmware/issues/11).

Once the case is open, you'll see a small PCB with a 6-pin (2×3)
ICSP header. The PIC16LF1938 is the largest IC on the board. Pin 1
of the ICSP header is usually marked with a square pad or the
number "1" on the silkscreen.

### 1. Get the toolchain

You need Microchip's XC8 C compiler. The free mode is fine — it
optimises less than the PRO mode but produces working code.

- Download XC8: <https://www.microchip.com/mplab/compilers>

Confirm installation works (`xc8-cc` should be added to the PATH during installation):

```sh
xc8-cc --version
```

### 2. Build with make

```sh
make
```

The build produces `dist/bekantfirmware.elf`
and `dist/bekantfirmware.hex`.

### 3. Open the BEKANT controller

The BEKANT controller is a small metal box attached to the desk
frame, with two buttons on the front and a power cord at the back.

1. **Unplug the desk from mains.** Capacitors inside the controller
   hold enough charge to damage your programmer and the PIC.
2. Open the controller enclosure. See
   [step 0 above](#0-disassemble-the-controller-box) for guides —
   the case is **ultrasonically welded** on newer units and you may
   need to cut or pry the welds to get it open. Do not expect this
   to be reversible; the box will not sit as flat when you put it
   back together.
3. Locate the **ICSP header** on the PCB — a 6-pin (2×3) header
   labelled `ICSP`, `PICKIT`, or similar. The ivanwick project's
   [Installation Guide](https://github.com/ivanwick/bekantfirmware/wiki/Installation-Guide)
   has photos of the header location.
4. Plug your PICkit 3 / 4 into the ICSP header. The arrow / notch
   on the PICkit connector indicates pin 1 — match it to the
   silkscreen on the PCB (pin 1 is usually marked with a square pad
   or a "1").
5. **Check the date code** on the white sticker on the surface that
   touches the underside of the desk. It's formatted like
   `2017/34` (year / week of year). See the
   [Hardware variants](#hardware-variants--code-protection) section
   below for what to do with that information.

### 4. Back up the original firmware (strongly recommended, with caveats)

> **Read [Hardware variants](#hardware-variants--code-protection)
> first.** On desks manufactured 2019 or later the PIC is code-protected
> and a backup will read back as all zeros. You cannot recover the
> OEM firmware from a newer chip.

On a desk that is **not** code-protected (typically manufactured
before 2019), read the original PIC's flash and EEPROM and save
the `.hex` file before you write anything:

```sh
# In MPLAB X IDE: Production → Read Device Memory → Save...
# Or from the command line with pk3cmd (if installed):
pk3cmd -P PIC16LF1938 -J HEX -F firmware-backup.hex -M
```

Verify the backup is real (not all zeros) by checking the file
size and contents:

```sh
# A real firmware dump is ~94 KB. A code-protected dump is 0 bytes
# or all 0x00 / 0xFF.
ls -l firmware-backup.hex
# Read the first 16 bytes; for a real dump you should see a mix of
# non-zero values that look like Intel HEX records.
head -3 firmware-backup.hex
```

If your backup is all zeros, your PIC is code-protected. Use the
reference dump
[`orginafirm.hex.zip`](https://github.com/ivanwick/bekantfirmware/files/7874509/orginafirm.hex.zip)
instead — see
[Hardware variants](#hardware-variants--code-protection) for the
caveats.

The reference dump was posted by user `WantedLange` in
[ivanwick/bekantfirmware issue #2 comment 1013637468](https://github.com/ivanwick/bekantfirmware/issues/2#issuecomment-1013637468)
("This is NOT verified or tested, but its the read from my original
firmware"). It is from a pre-2019 unit and was confirmed to work by
user `iLLiac4` in the following comment
([#2 comment 1013638297](https://github.com/ivanwick/bekantfirmware/issues/2#issuecomment-1013638297):
"Great TNX. It works.").

### 5. Flash the new firmware

```sh
pk3cmd -P PIC16LF1938 -F dist/bekantfirmware.hex -M -Y
```

### 6. Restore the EEPROM defaults (optional)

The ivanwick project's firmware stores the memory positions in the
PIC's EEPROM at offsets `0x00` and `0x02`, with defaults
`0x0636` and `0x1600`. If you flashed a fresh unit, these defaults
are already correct. If you reprogrammed a unit that already had
custom positions, you'll need to re-set them:

- Move the desk to the new sitting height, then press and hold both
  buttons for 3 seconds.
- Move the desk to the new standing height, then press and hold
  both buttons for 3 seconds.

See [USAGE.md](USAGE.md) for the full button reference.

### 7. Close the controller and test

1. Reassemble the controller enclosure.
2. Plug the desk back into mains.
3. Test the basic motion (hold `△` / `▽`).
4. Test the memory position — see [USAGE.md](USAGE.md).

### Troubleshooting

- **PICkit not detected.** Check the USB cable. On Linux you may need
  udev rules; Microchip publishes them in
  `/opt/microchip/mplabx/<version>/mplab_ide/docs/` after install.
- **"Device ID mismatch".** The PIC you have is not a PIC16LF1938.
  Double-check the chip marking on the top of the IC.
- **"Programming failed at address 0x0000".** The ICSP cable is
  reversed or a pin is shorted. Inspect the connector and try again.
- **The desk doesn't move but the controller's lights are on.** The
  bus scan failed. Power-cycle the desk and try again.
- **The relay stays engaged at the endstop.** The endstop detector
  is included in the `oem_reconstruction/` folder and is wired into
  the main build (`src/bekant/endstop.c`, registered in
  `user.c::InitApp()` and ticked from `bctrl_timer()`). See
  [docs/ENDSTOP_ANALYSIS.md](docs/ENDSTOP_ANALYSIS.md) for the analysis.

## Hardware variants & code protection

The BEKANT controller has shipped in at least two distinct
hardware revisions. The variants differ in two ways that matter
for this project: **how easy the case is to open**, and **whether
the firmware can be read back from the PIC**.

### Disassembly difficulty

The controller case is ultrasonically welded. IKEA has progressively
added more weld spots to deter modifications. Newer units (2019+)
have noticeably more welds and may require cutting or prying to
get the back plate off. The damage is cosmetic and the case still
holds together when you put it back. See the disassembly guides
linked in [step 0](#0-disassemble-the-controller-box) above and
the discussion in
[ivanwick/bekantfirmware issue #11](https://github.com/ivanwick/bekantfirmware/issues/11).

### Code protection on newer units

**The most important variant to be aware of:** desks manufactured
roughly 2019 or later ship with the PIC's **code-protection (CP)
config bit enabled**. When CP is set, the PIC refuses to read back
its own flash memory over ICSP. The programmer will still **write**
the chip (which is what you need to install the new firmware), but
a backup "read" will return all zeros.

Symptoms reported in
[ivanwick/bekantfirmware issue #5](https://github.com/ivanwick/bekantfirmware/issues/5)
and
[ivanwick/bekantfirmware issue #2 comments 1013629545 / 1013637468](https://github.com/ivanwick/bekantfirmware/issues/2)
include:

- *"I was able to flash the pic but not to be able to read ... the
  backup ... was zeros only."* — code-protected.
- *"It did not autodetect it and was not able to make a working
  backup of the flash."* — usually code-protected.
- *Backup dump is suspiciously small (0 bytes or only `0x00` /
  `0xFF` records).* — code-protected.

If your backup reads back as all zeros, **do not assume the
firmware is gone** — it is just locked. You can still flash the
new firmware, you just cannot recover the OEM firmware from your
specific unit.

To flash a code-protected unit, you must temporarily erase the
chip (which clears the CP bit along with the firmware). After
erasing, you can write the new firmware normally:

```sh
pk3cmd -P PIC16LF1938 -E  # erase
pk3cmd -P PIC16LF1938 -F dist/bekantfirmware.hex -M -Y
```

### Reference firmware dump for code-protected units

For code-protected units where you cannot read the OEM firmware
back, the ivanwick project bundles a reference dump
([`orginafirm.hex.zip`](https://github.com/ivanwick/bekantfirmware/files/7874509/orginafirm.hex.zip))
that you can flash to restore factory behaviour. This dump is
bundled with this project too — see
`oem_reconstruction/disassembly/orginafirm.hex`.

**Caveats:**

- The reference dump is **unverified by the project author**. It
  was posted in
  [issue #2 comment 1013637468](https://github.com/ivanwick/bekantfirmware/issues/2#issuecomment-1013637468)
  by a user reading their own pre-2019 unit ("This is NOT verified
  or tested, but its the read from my original firmware"). It was
  then tested by another user
  ([issue #2 comment 1013638297](https://github.com/ivanwick/bekantfirmware/issues/2#issuecomment-1013638297))
  who reported it worked on a 2017-vintage unit, but that is
  anecdotal.
- The dump is for a **specific hardware revision** of the BEKANT
  controller. Newer revisions may use a different PIC, a different
  LIN protocol, or a different leg controller, in which case the
  reference dump will not work. If flashing the reference dump
  causes the desk to misbehave (e.g. relay engaged but legs not
  moving, LIN errors, etc.), your hardware is incompatible.
- If you flash the reference dump and later want to go back to the
  ivanwick firmware, the round-trip has not been tested.

In short: **treat the reference dump as a recovery option, not a
guaranteed rollback**.

## See also

- [USAGE.md](USAGE.md) — button reference
- [docs/COMPARISON.md](docs/COMPARISON.md) — ivanwick vs DieSteinhose
- [docs/TRACE.md](docs/TRACE.md) — state-machine traces
- [docs/ENDSTOP_ANALYSIS.md](docs/ENDSTOP_ANALYSIS.md) — endstop fix analysis

## References

1. <a name="1">https://web.archive.org/web/20190116092248/https://blog.rnix.de/hacking-ikea-bekant/</a>
2. <a name="2">https://github.com/trainman419/bekant</a>
3. <a name="3">https://github.com/robin7331/IKEA-Hackant</a>
4. <a name="4">https://github.com/diodenschein/TableMem</a>
5. <a name="5">https://en.wikipedia.org/wiki/Local_Interconnect_Network</a>

## Disclaimer

Use at your own risk. The original IKEA BEKANT firmware does not
expose memory positions for safety and liability reasons. The
ivanwick project preserves a human-in-the-loop requirement: any
button press or power cut cancels an automatic move. This project
follows the same pattern.

Position memory is convenient but assumes you have verified the
saved height is safe. Ensure the desk's path is clear during all
movement.
