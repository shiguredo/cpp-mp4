# 変更履歴

- CHANGE
    - 下位互換のない変更
- UPDATE
    - 下位互換がある変更
- ADD
    - 下位互換がある追加
- FIX
    - バグ修正


## develop

## 2023.2.1

- [FIX] Version 上げ忘れを修正
    - @haruyama

## 2023.2.0

- [UPDATE] 依存ライブラリの更新
    - `boost` を `1.83.0` に
    - `rapidcsv` を `8.77` に
    - @haruyama
- [ADD] H.264 Track のサポート
    - @haruyama
- [UPDATE] AV1 Track の調整
    - @haruyama

## 2023.1.1

- [FIX] Version 上げ忘れを修正 #25 
    - @haruyama
    - https://github.com/shiguredo/cpp-mp4/pull/25

## 2023.1.0

- [UPDATE] 依存ライブラリの更新
    - `boost` を `1.81.0` に
    - `CLI11` を `2.3.2` に
    - `fmt` を `9.1.0` に
    - `spdlog` を `1.11.0` に
    - `rapidcsv` を `8.69` に
    - @haruyama
    - https://github.com/shiguredo/cpp-mp4/pull/20
- [FIX] Safari, Windows Media Player などでの再生の問題を修正
    - @haruyama
    - https://github.com/shiguredo/cpp-mp4/pull/19
- [FIX] typo の修正
    - @haruyama
    - https://github.com/shiguredo/cpp-mp4/pull/18

## 2022.1.0

- [UPDATE] 依存ライブラリの更新
    - `boost` を `1.78.0` に
    - `CLI11` を `2.1.2` に
    - `fmt` を `8.0.1` に
    - `spdlog` を `1.9.2` に
    - `rapidcsv` を `8.53` に
    - @haruyama
- [FIX] 宇宙船演算子を利用している箇所に #include <compare> を追加
    - @haruyama
    - https://github.com/shiguredo/cpp-mp4/pull/12

## 2021.3

- [UPDATE] `boost` を `1.76.0` に
    - @haruyama
    - https://github.com/shiguredo/cpp-mp4/pull/8

## 2021.2

- [ADD] av01, av1C box のサポート
    - @haruyama
- [ADD] get_version_string() の追加
    - @haruyama
- [UPDATE] `rapidcsv` を `8.48` に
    - @haruyama
- [FIX] MP4 ファイルの読み込み時に, ファイル終端を越える box のサイズを指定されたら例外を投げる
    - @haruyama
- [FIX] VPCodecConfiguration::getDataSize() のバグ修正
    - @haruyama

## 2021.1

**祝リリース**
