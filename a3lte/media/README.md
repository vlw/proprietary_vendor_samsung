# A3 ARM32 native software decoders

Only SM-A300FU enables these replacements (`a3_media.native_decoders` in
`device/samsung/a3lte/media/media.mk`). Public Codec2 component names and the
existing media.swcodec APEX library names are retained.

* AV1: VideoLAN dav1d 1.5.4, tag commit
  `54706fc6bc0cdecab7e9593974a4039cc038fca7`, BSD-2-Clause.
  Source: https://github.com/videolan/dav1d/tree/1.5.4
  The `dav1d` directory retains upstream src/include, COPYING, README and NEWS;
  Android.bp and ARM32 generated config are local build integration. The latter
  was generated with Meson for Android ARMv7/API29, release, assembly enabled,
  both 8/16-bit implementations. ARM32 NEON routines target the Cortex-A53.
* Codec2 adapters derive from LineageOS frameworks/av commit
  `83fd5bcecf703ee7b105d58c442c89fae0a8d494`,
  `media/codec2/components/{gav1,vpx}/C2Soft{Gav1,Vpx}Dec.{cpp,h}`,
  Apache-2.0. Their sources are copied here; framework sources are not modified.
* VP9 retains the platform libvpx and enables its supported `VP9D_SET_ROW_MT`
  control. Thread count uses configured CPUs, rather than whichever CPUs happen
  to be online when the component is constructed. No loop filters are skipped.
* AV1 uses up to four dav1d workers and two-frame maximum parallel delay. The
  Codec2 output-delay declaration matches this delay. Packet timestamps, output
  order, flush, drain, film grain and 8/10-bit output remain supported. The
  original Codec2 graphic buffer/colour handling is retained.

## Regenerate after source changes

From the Android root, run `vendor/samsung/a3lte/media/rebuild.sh` (JOBS defaults
8, OUT_DIR defaults `out-codec-opt`). This builds both source producers, refreshes
`prebuilt/*.so` and SHA256SUMS, then validates the final logical codec modules.
Normal ROM builds select these checked-in generated binaries. Changes to the
sources **must** be followed by this explicit regeneration step.

The fixed prebuilts avoid an Android 12 Soong APEX dependency traversal problem
with a generated `srcs: [":source_module"]` replacement. They are deliberately
paired with full source and this regeneration command. Producers install into
`system/lib/a3codec-build` only when explicitly built for regeneration; they are
not in PRODUCT_PACKAGES. The two preferred logical modules are packaged in the
usual media.swcodec location. Other devices keep the platform codecs.

See `device/samsung/a3lte/media/NATIVE_DECODERS.md` for runtime validation and
measured limits. Rebuilding does not deploy, restart services, or reboot a phone.
