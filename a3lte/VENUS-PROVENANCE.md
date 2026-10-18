# SM-A300FU Venus firmware

Unmodified signed Venus VIDEO.VE.1.8-00155 from Android 6.0.1:
`CP_A300FUXXU1CQI1_REV00_CL2127430_QB7553660.tar.md5`, NON-HLOS.bin/image/venus.*,
provided alongside A300FUXXU1CSG3 stock firmware. The six split files must stay
matched. No venus.mbn from a different revision is used.

Installed in vendor/firmware/venus. Device init sets firmware_class.path to this
narrow directory before codec services start. The kernel's normal firmware
search paths remain the fallback for other filenames; firmware/modem partitions
are not flashed. Android 12 MediaCodec tests confirmed the running driver
version, decoded reference pixels, encoded/independently decoded frames and
Surface delivery. See device/samsung/a3lte/media/STOCK6_COMPARISON.md.
