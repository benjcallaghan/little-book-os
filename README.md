# Ben's Little OS

Developed by following [The little book about OS development](https://littleosbook.github.io/) by Erik Helin and Adam Renberg.

Developed using Windows 10 + WSL + Ubuntu 24.04.

## Bochs

Use `display_library: sdl2` instead of `display_library: sdl`. https://stackoverflow.com/questions/63159511/bochs-display-library-sdl-not-available

Use `romimage: file=/usr/share/bochs/BIOS-bochs-legacy` instead of `romimage: file=/usr/share/bochs/BIOS-bochs-latest`. https://stackoverflow.com/questions/73067357/bochs-can-not-load-bootloader-using-a-floppy-image#comment137117980_75516029

## GRUB Legacy

GRUB Legacy v0.97 is used as the main bootloader. I was unable to make it compile locally, nor could I find a pre-made binary package. Since I only needed the `stage2_eltorito` loader, I used the copy found in the littleosbook GitHub repository, which is available in the root of this repository as well. https://github.com/littleosbook/littleosbook/blob/master/files/stage2_eltorito