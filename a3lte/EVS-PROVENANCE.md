# Stock EVS codec

`proprietary/lib/libevs_float.so` is the unmodified ARM32 library extracted from
SM-A300FU A300FUXXU1CSG3 Android 6.0.1 `/system/lib/libevs_float.so`.
SHA-256: `3c599e83bca7ca132cb058341f821a421e53d0e283611522d4e4fe0ea97f3444`.

The handwritten Android.bp builds this as a prebuilt shared library; do not
replace it with a copy-file rule or a second generated module of the same name.
Device-side JNI, supported profile, tests and implementation limitations are
in device/samsung/a3lte/ims/EVS.md. The file retains its original proprietary
licensing; it is not covered by the GPL license of the device IMS source.
