# Vendor libraries for BASIC example

The `vendor` directory bundles third-party code used by the BASIC compiler example.

- **Ryu**: https://github.com/ulfjack/ryu — fast floating-point to string conversion.
  Snapshot of upstream code (circa 2018) distributed under Apache-2.0 or Boost Software License 1.0.
  See `vendor/ryu/LICENSE-Apache2` and `vendor/ryu/LICENSE-Boost` for details.
- **Kitty graphics helpers**: minimal Kitty terminal image support with embedded
  [LodePNG](https://github.com/lvandeve/lodepng) (version 20250506) and rasterm utilities.
  Licenses are provided in `vendor/kitty/lodepng-LICENSE` and `vendor/kitty/rasterm-LICENSE`.
- **Decimal floating point (libdfp decNumber subset)**: https://github.com/libdfp/libdfp —
  Densely Packed Decimal arithmetic with decimal128 support.  Files in `vendor/libdfp`
  are provided under the terms of the GPLv3 with the GCC Runtime Library Exception.
