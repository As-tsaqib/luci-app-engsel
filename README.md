# luci-app-engsel

OpenWrt LuCI app for MyXL account, multi-number quota history, store, checkout, notifications, and transaction status.

Native C backend. No Python runtime on router.

Upstream reference:

```text
https://github.com/purplemashu/me-cli-sunset.git
```

## Packages

```text
engsel            1.0.0-r5
luci-app-engsel  1.0.0-r4
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

# Balance + Decoy Standard: token target (index 0), payment_for target
engsel pay-balance-decoy-standard <option_code...>

# Balance + Decoy V2: token decoy, payment_for kosong
engsel pay-balance-decoy-v2 <option_code...>

# JSON API (tambahkan confirm=1 untuk mengeksekusi pembayaran)
engsel json payment balance-decoy-standard <option_code...> confirm=1
engsel json payment balance-decoy-v2 <option_code...> confirm=1

# Repeat purchase khusus CLI C; quote/token dimuat ulang setiap putaran
engsel purchase-n-times <family_code> <variant_code> <order> <count> \
  [--delay <seconds>] [--use-decoy <y|n>] \
  [--token-confirmation-idx <idx>]

engsel purchase-n-times-by-option-code <option_code> <count> \
  [--delay <seconds>] [--use-decoy <y|n>] \
  [--token-confirmation-idx <idx>]
```

Untuk repeat purchase, tanpa decoy indeks yang valid adalah `0`/`-1`. Dengan decoy,
`0`/`-2` memilih token target (semantik Standard) dan `1`/`-1` memilih token decoy
(semantik V2). Ringkasan sukses/gagal ditampilkan setelah seluruh putaran selesai.

Alias lama direct CLI `pay-decoy` dan `pay-balance-decoy`, serta alias JSON
`balance-decoy`, `decoy`, dan `prio`, tetap tersedia dan diarahkan ke V2.
Alias ringkas baru `pay-decoy-standard`, `pay-decoy-v2`, `decoy-standard`, dan
`decoy-v2` juga tersedia.

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

## Build

```sh
make
make clean
```

GitHub Actions builds IPK and APK packages for the OpenWrt SDK matrix.

## Notes

```text
OpenWrt 24.10.x  -> IPK
OpenWrt 25.x     -> APK
```
