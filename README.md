# luci-app-engsel

OpenWrt LuCI app for MyXL account, multi-number quota history, store, checkout, notifications, and transaction status.

Native C backend. No Python runtime on router.

Upstream reference:

```text
https://github.com/purplemashu/me-cli-sunset.git
Audited commit: 0e8a06f2b52583b2d91b1fc1a4270b6cf8dfc205
```

## Packages

```text
engsel            1.0.0-r7
luci-app-engsel  1.0.0-r6
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

# Balance + Decoy: token target (index 0), payment_for target
engsel pay-balance-decoy-standard <option_code...>

# Balance + Decoy V2: token decoy, payment_for "🤫"
engsel pay-balance-decoy-v2 <option_code...>

# JSON API (tambahkan confirm=1 untuk mengeksekusi pembayaran)
engsel json payment balance-decoy-standard <option_code...> confirm=1
engsel json payment balance-decoy-v2 <option_code...> confirm=1

# Ulangi satu paket yang sama; subcommand lama tetap tersedia
engsel purchase-n-times <family_code> <variant_code> <order> <count> \
  [--delay <seconds>] [--use-decoy <y|n>] \
  [--token-confirmation-idx <idx>]

engsel purchase-n-times-by-option-code <option_code> <count> \
  [--delay <seconds>] [--use-decoy <y|n>] \
  [--token-confirmation-idx <idx>]

# Beli semua opsi dalam satu family mulai dari nilai order opsi API tertentu
engsel purchase-by-family <family_code> \
  [--delay <seconds>] [--use-decoy <y|n>] \
  [--start-from-option <number>] [--pause-on-success <y|n>]

# Menu utama CLI:
# engsel -> 7. Beli Semua Paket dalam Family Code (Loop)
```

`Pulsa N Kali` tersedia di metode pembayaran pada detail satu paket CLI. Target
langsung memakai paket yang sedang dibuka; menu ini hanya menanyakan penggunaan
decoy, jumlah pembelian, delay, dan konfirmasi. Menu tersebut tidak tersedia pada
checkout cart multi-paket, JSON API, atau LuCI.

Untuk pembelian satu paket N kali, tanpa decoy indeks yang valid adalah `0`/`-1`. Dengan decoy,
`0`/`-2` memilih token target dan `1`/`-1` memilih token decoy. Mengikuti upstream,
settlement repeat selalu memakai `payment_for="🤫"`. Token aktif diambil ulang,
quote target/decoy dimuat ulang, dan nama item diberi prefix acak pada setiap
putaran. Kegagalan satu putaran tidak menghentikan putaran berikutnya; delay tetap
diterapkan di antara putaran dan ringkasan sukses/gagal ditampilkan di akhir.

`purchase-by-family` adalah flow terpisah: setiap package option dalam family
dibeli satu kali menurut urutan API. `--start-from-option` mengikuti field `order`
opsi API seperti upstream. Settlement awal memakai `payment_for="🤑"`;
retry `Bizz-err.Amount.Total` memakai `payment_for="SHARE_PACKAGE"`. Tanpa decoy,
token target selalu index `0`. Flow ini juga khusus CLI dan tidak diekspos melalui
JSON API atau LuCI.

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

Optional Decoy overrides are managed from `Modem > Engsel > Settings > Decoy`.
Each subscription type uses an atomic family/package pair:

```text
DECOY_PREPAID_FAMILY_CODE
DECOY_PREPAID_PACKAGE_NUMBER
DECOY_PRIORITAS_FAMILY_CODE
DECOY_PRIORITAS_PACKAGE_NUMBER
DECOY_PRIOHYBRID_FAMILY_CODE
DECOY_PRIOHYBRID_PACKAGE_NUMBER
```

Leave both values in a pair blank to use the built-in decoy. If one value is
filled, the other is required. `PACKAGE_NUMBER` is a 1-based position after
flattening every package variant and its options in API order; it is not the
package option `order` field. A blank PRIOHYBRID pair falls back to the complete
PRIORITAS pair, then to the built-in PRIORITAS decoy.

The configured pair never stores a resolved option code. The backend fetches the
family and validates package detail with the active subscriber token before every
Decoy settlement. A package-detail code `151` clears the account token cache and
performs one fresh-token re-resolution; a second failure stops before settlement.
Older option-code settings remain untouched in UCI but are hidden and ignored.

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
