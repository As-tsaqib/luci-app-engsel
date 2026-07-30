# luci-app-engsel

OpenWrt LuCI app for MyXL account, quota, store, checkout, notifications, and transaction status.

Native C backend. No Python runtime on router.

Upstream reference:

```text
https://github.com/purplemashu/me-cli-sunset.git
```

## Packages

```text
engsel            1.1.0-r1
luci-app-engsel  1.1.0-r1
```

## LuCI

```text
Modem > Engsel
Modem > Engsel > Riwayat > Kuota History
Modem > Engsel > Notifikasi
```

## CLI

```sh
engsel --help
```

## Config

```text
/etc/engsel/.env
/etc/config/engsel
/root/.engsel/
```

API keys are managed from:

```text
Modem > Engsel > Settings > Environment
```

Custom Decoy pairs are managed from `Modem > Engsel > Settings > Decoy`:

```text
DECOY_PREPAID_FAMILY_CODE
DECOY_PREPAID_PACKAGE_NUMBER
DECOY_PRIORITAS_FAMILY_CODE
DECOY_PRIORITAS_PACKAGE_NUMBER
DECOY_PRIOHYBRID_FAMILY_CODE
DECOY_PRIOHYBRID_PACKAGE_NUMBER
```

## Build

```sh
make
make clean
```

GitHub Actions builds IPK and APK packages for the OpenWrt SDK matrix.

## Notes

```text
OpenWrt 24.10.8  -> IPK
OpenWrt 25.12.5  -> APK
```
