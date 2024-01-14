# RP2040-Geed PNG display

Just a base project to load PNG pictures from the SDCard and show them on the
RP2040-Geek display 240x135 pixels.

## Build

```sh
mkdir -p build
cd build
cmake ..
make
```

## Deploy

Just upload the `u2f` file to the RP2040 as usual.

# Convert Images to 240x135 pixels

Just use `imagemagick`

```sh
convert laurasiffredi-c.png -resize 240x135 -gravity Center -extent 240x135 laurasiffredi-r.png
```

Install with

```sh
sudo apt install graphicsmagick-imagemagick-compat
```