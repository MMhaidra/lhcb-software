# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'guicreatedevices.ui'
#
# Created: Thu Aug 16 16:35:25 2007
#      by: The PyQt User Interface Compiler (pyuic) 3.13
#
# WARNING! All changes made in this file will be lost!


from qt import *

image0_data = \
    "\x89\x50\x4e\x47\x0d\x0a\x1a\x0a\x00\x00\x00\x0d" \
    "\x49\x48\x44\x52\x00\x00\x00\x30\x00\x00\x00\x30" \
    "\x08\x06\x00\x00\x00\x57\x02\xf9\x87\x00\x00\x03" \
    "\xc9\x49\x44\x41\x54\x78\x9c\xed\x97\x4b\x4c\x5c" \
    "\x55\x18\x80\xbf\xfb\x9c\x99\xce\x03\x66\x86\xd7" \
    "\xc0\x14\x5a\xa6\xa5\xbc\x4a\x5a\xa2\xd2\xa6\xa1" \
    "\x86\xd8\xaa\x6d\xd3\x74\x57\x17\x76\x61\x13\x63" \
    "\x5c\x90\xb8\x76\x69\x62\xea\x4a\x13\x35\xa6\x89" \
    "\x2e\x9a\xb8\xd4\x60\x42\x2a\xb6\x0d\x31\x90\x34" \
    "\x54\xa3\xf2\x6a\x91\x02\x15\x9c\x02\x03\x32\x94" \
    "\xc1\x61\x86\x19\x66\x86\x7b\xaf\x8b\xa1\x74\xe1" \
    "\xc2\xa4\xf7\x26\xb8\xb8\x5f\x72\x93\xb3\x38\xf9" \
    "\xcf\xff\x9d\xff\x9e\x7b\xfe\x0b\x36\x36\x36\x36" \
    "\x36\x36\x36\x36\x36\x36\x36\xcf\x89\x60\x79\xbc" \
    "\x0b\x9c\xc3\xc1\x6b\x88\xc2\x49\x44\xe1\x08\x00" \
    "\xba\x31\x8d\x6e\xfc\x44\x8e\x3b\xf4\x71\x0b\x30" \
    "\xac\x5b\xd0\xaa\x38\x5d\xd4\x13\xe0\xbd\xa3\x0d" \
    "\xcd\xef\x5c\x6a\x3f\xe7\x68\x3f\xd8\x4e\x6d\xa0" \
    "\x0e\x0c\x58\x58\x7f\xcc\xf0\x9f\x23\xf4\x8e\xdc" \
    "\xca\x3d\x98\x99\xfc\x92\x04\x9f\x32\xc0\x1c\x16" \
    "\x88\x48\x96\x24\xff\x2a\x11\xa9\x92\x8f\x2e\x9e" \
    "\xec\xba\xda\x7d\xe6\x5d\xb9\xb3\xf1\x65\x02\x1e" \
    "\x3f\xa2\x28\x22\x48\x22\x7e\xb7\x9f\xa6\x50\x33" \
    "\x2d\xe1\xc3\x72\x4e\x4a\x75\xcc\x6e\x44\x43\x46" \
    "\x88\x51\x66\x59\xff\x3f\x08\xc8\x4a\x07\x1f\x9c" \
    "\x68\x69\x79\xeb\xca\xa9\x2b\x54\x07\x6a\x58\xcf" \
    "\xc6\xe9\x1b\xef\xe3\xdb\x5f\x7b\x18\x9c\x1a\x64" \
    "\x25\x19\x23\x58\xe2\xa5\xd4\xed\xa7\xc6\x1f\x22" \
    "\x9e\x99\x6f\x59\xde\x5c\x95\xf5\x07\xdc\x01\xf4" \
    "\xbd\x14\x10\xa4\x37\xb8\xd8\x18\x09\x5d\x3b\xdd" \
    "\xf4\x92\x1c\x2a\xad\x24\x91\x5d\x24\x96\x99\xe4" \
    "\xee\xc3\x61\x7a\xbb\x7f\xe4\xf2\x8b\x6f\xf2\x71" \
    "\xff\x87\xf8\x03\xdb\x64\x73\x59\x44\x51\x40\x96" \
    "\x34\x92\xf9\xb5\xb6\xb5\xea\xf4\xb8\xf1\x3b\x33" \
    "\x66\x12\x90\x4d\x0a\xc8\x1e\x1f\xe7\x1b\xc2\x75" \
    "\x8e\x2d\x63\x8d\xb9\xe4\x38\xba\x94\x44\x96\xa1" \
    "\xc0\xc6\xee\xa4\x02\x29\x52\x5a\x8c\xf5\xdc\x22" \
    "\x92\x56\xc2\x96\xb1\x46\x43\xb8\xce\xb1\x10\x5f" \
    "\x3e\x9f\x84\x1f\x80\xc2\x5e\x09\x28\x4e\x37\x1d" \
    "\x41\x5f\x29\xf1\xec\x0c\xd3\x4f\x1e\xb2\xb9\x09" \
    "\x92\x24\x20\x4b\xcf\x8a\x5b\x10\x53\xdc\x1e\x1e" \
    "\x46\xd3\x74\x3c\x6e\x11\xbf\x5b\x25\xe8\x6b\xc0" \
    "\xe9\xa6\x23\x09\xca\xde\x0a\x78\x89\x78\x5c\x0a" \
    "\x09\x23\xcb\x52\x62\x95\xfb\xef\x67\xff\x35\x69" \
    "\xa0\x7b\x6e\x77\xdc\x7c\xcd\x41\xa9\xaf\x02\x8f" \
    "\x4b\xc1\xe9\x25\x42\x51\xe0\xb9\x31\x2b\x20\x3a" \
    "\x9d\xa0\xaa\x22\x25\xc2\x3e\x54\xf5\xbf\xc3\xa9" \
    "\x8a\x88\x6b\x9f\x8c\xaa\x88\x38\x9d\xc5\x18\x66" \
    "\x12\x30\x2b\x80\xa4\x13\x15\x45\xa1\xb5\xdc\x5b" \
    "\xc5\x91\xfd\x1a\x67\xae\x57\xa1\xca\x0e\x14\x41" \
    "\xa2\xf7\xed\xe2\xce\x5f\xf8\xaa\x8e\xbc\x96\x23" \
    "\xa7\xe5\xa9\xaf\x0e\x52\xee\xad\xc2\xd8\xd6\x91" \
    "\x74\xa2\x66\xd7\x37\x2b\xa0\x93\x67\x2c\xa3\x25" \
    "\x5b\x1b\x4b\x9b\xa9\x2d\x0f\x62\xc8\x4b\xf8\x9c" \
    "\x25\xdc\x1c\x5c\xda\x9d\xe4\x52\x5c\x5c\xea\xaa" \
    "\x21\x91\x5d\x47\xd9\xae\xc3\x69\xd4\x30\xbb\x12" \
    "\x85\x3c\x63\x98\xfc\x8c\x9a\x15\x28\xa4\x17\x19" \
    "\x5c\x6a\x9a\xba\xdc\x5e\xfb\x82\xea\xf7\x54\xa2" \
    "\x3a\x22\x14\x98\xc7\x25\x25\x9e\x09\x48\x4e\xbc" \
    "\xaa\x8f\x32\xb5\x0d\x63\xdb\x43\x6a\x53\xe7\xaf" \
    "\xf4\x4c\x3e\xbd\xc8\x20\x26\x0e\x30\x58\x70\x91" \
    "\x25\xc7\x58\x76\x1c\x4f\x87\x3d\x01\xe3\x68\x4b" \
    "\xc5\x09\x82\xae\x6a\xca\x1c\x07\xd0\x74\x9d\xaf" \
    "\xef\x7d\x46\xdf\xc4\x0d\x8e\x1d\x6c\xe0\x58\xb8" \
    "\x13\xb7\x50\x85\xa1\xbb\xf8\x25\xd6\xcf\xc8\xc4" \
    "\xcc\x37\xd3\x9f\x70\x1d\x48\x61\xa2\x0a\x66\x05" \
    "\x0c\xc0\x48\xe5\x89\xe7\x82\x8b\x21\xd5\xb3\x15" \
    "\x09\x7b\xea\x29\x73\xd5\xd2\x16\x3e\xce\xd9\xd6" \
    "\x57\x38\xdd\xd8\xc9\xa1\xca\x46\x44\xc3\xcd\x93" \
    "\xcc\x2a\xf7\x62\xdf\x73\x77\xfc\xe7\xfe\xc9\x9b" \
    "\x7c\x5e\x98\x67\x0e\xd8\x32\x93\x80\x15\xad\x84" \
    "\x5e\x98\xe7\xef\x44\x8a\xe8\x86\x3b\xea\x48\xc8" \
    "\x53\x87\x0b\x46\x46\x92\x44\x05\x87\xe8\x64\x4b" \
    "\xcb\x11\x4b\x3d\x62\x34\x3e\xc0\xc0\xe3\x9e\xfc" \
    "\xd0\x6f\x13\x3d\xf7\xbf\xe3\x8b\xcc\x10\xa3\x40" \
    "\x1a\x93\x67\xc0\x8a\x6e\x54\xa0\x78\x96\x7c\x40" \
    "\xf9\xa1\xab\xbc\x5e\xd9\xca\x29\x4f\x05\xad\xaa" \
    "\x97\xfd\x00\xf9\x14\x0b\xe9\x38\x13\x2b\x13\x0c" \
    "\xfd\x71\x83\xdb\xc0\x2a\xb0\x01\x6c\x63\xb2\x23" \
    "\xb5\xae\x9d\x2e\x56\xd3\x45\x51\xc4\xbb\x33\x7e" \
    "\x7a\x49\x15\x80\x2c\xc5\xf7\x7d\x63\x67\xac\x61" \
    "\x41\x3b\x6d\xfd\x0f\x4d\xb1\x1a\xca\xce\xf3\xf4" \
    "\x92\xd2\x29\x4a\x14\xb0\x60\xd7\x6d\x6c\x6c\x6c" \
    "\x6c\x6c\x6c\x6c\x6c\x6c\x6c\x2c\xe1\x1f\x5a\xdc" \
    "\x4d\xb9\x85\x9e\xf7\x52\x00\x00\x00\x00\x49\x45" \
    "\x4e\x44\xae\x42\x60\x82"
image1_data = \
    "\x89\x50\x4e\x47\x0d\x0a\x1a\x0a\x00\x00\x00\x0d" \
    "\x49\x48\x44\x52\x00\x00\x00\x30\x00\x00\x00\x30" \
    "\x08\x06\x00\x00\x00\x57\x02\xf9\x87\x00\x00\x03" \
    "\xbb\x49\x44\x41\x54\x78\x9c\xed\x97\xcb\x6f\xdc" \
    "\x54\x14\x87\xbf\xeb\xe7\x78\x32\x99\xb8\x79\x4d" \
    "\xf3\xac\x92\x49\x4a\x89\x92\xd0\x52\x44\xfa\x20" \
    "\x3b\xc4\xa3\x08\x21\x16\x01\x81\xc4\x82\x0d\x0b" \
    "\x36\x15\xfc\x11\xb0\x46\xc0\x86\x2d\x2b\x16\xd9" \
    "\x42\xab\x6e\x2a\xa0\x54\x8a\x28\x99\x2a\x55\xd2" \
    "\x84\xa4\x49\x1b\xda\xbc\x9a\x99\xe9\x4c\xec\x99" \
    "\xb1\xc7\x36\x0b\xb7\x1b\x90\x10\x8a\x2d\x65\xe3" \
    "\x4f\xb2\xe4\xcd\xb9\xe7\xf7\xbb\xc7\xe7\xde\x63" \
    "\x48\x48\x48\x48\x48\x48\x48\x48\x48\x48\x48\x38" \
    "\x24\x22\xee\xf5\x4e\x5f\xf8\xe8\x4d\x45\x35\x5e" \
    "\x97\x24\x71\x5e\x08\xe9\x39\x80\x20\xf0\x97\x7d" \
    "\x3f\xb8\xd9\x74\x6b\x57\x0b\xbf\x7d\xff\x13\x10" \
    "\xc4\x96\x30\xae\x75\x46\xce\xbe\x3f\x6c\xb6\xa4" \
    "\x2f\xe7\x87\x06\x3e\x39\xff\xf2\x98\x7e\xf2\xe4" \
    "\x20\xbd\xb9\x0e\x08\x60\x7b\x77\x9f\xbb\x2b\x0f" \
    "\xb8\x39\xb7\xd8\x58\x5b\xdf\xfc\xae\x6c\xd9\x5f" \
    "\xad\xde\xfa\xe1\x1e\x31\x18\x91\xe3\x10\x3f\x76" \
    "\x66\x26\xdf\x6e\x9a\x5f\xbe\x74\xf6\xf9\x8f\xdf" \
    "\x7d\x6b\x5a\x39\xf3\xc2\x28\xd9\x4c\x1a\x21\x04" \
    "\x42\x08\x5a\x33\x69\x86\x4e\xf4\x30\xd0\xd7\xad" \
    "\xd4\x1d\x77\xaa\x52\xb2\x7a\xb2\xe6\xf0\xfc\xde" \
    "\xf6\x62\x29\x6a\x72\x25\x06\x03\x72\xc6\xcc\x7e" \
    "\x96\x1f\xee\x9d\x99\x3e\x37\x49\x9b\xd9\xc2\x5f" \
    "\x5b\x7b\x58\x07\x36\xb6\x5d\x07\x02\xd2\x69\x83" \
    "\x4c\x26\x4d\x5b\x5b\x0b\xd3\xe7\x26\xa9\x3c\x39" \
    "\x98\x59\x59\xf3\xf6\x80\xcb\x40\x33\x52\xf2\x88" \
    "\xe2\xc5\xe9\x0b\x1f\xbe\x3d\x30\xd8\xf3\xc5\xe8" \
    "\x70\x9f\x62\x9a\x19\x76\x77\x8a\xdc\xbf\xbf\x45" \
    "\xa9\x5c\xc5\xb2\x6a\x58\x96\x4d\xb9\x52\x65\x67" \
    "\xa7\x88\x6d\xd7\x11\x12\x78\xcd\x26\x75\xc7\x9d" \
    "\x4c\xb5\x9e\xb8\xbd\xbd\xb9\xb0\x12\x45\x40\xd4" \
    "\x0a\x28\x99\x94\x76\x29\xd7\x69\xea\xc5\x62\x85" \
    "\xc5\xe5\x75\x5c\x27\xdc\xd0\x7f\x36\x57\x00\xec" \
    "\x3e\x2e\xa1\x6a\x0a\x95\xb2\x45\xae\xd3\xd4\x1f" \
    "\x6f\xed\x5c\x02\x7e\x04\xdc\x43\x0b\x38\xb4\xf4" \
    "\x10\x55\x4e\x69\x53\xba\xae\xf2\xe7\xea\x03\x82" \
    "\x0d\x01\xb2\x82\x10\xff\x3e\x1b\x82\x20\x00\x02" \
    "\xf0\x3c\x84\x1c\x30\x3a\x32\x88\x9c\xd2\xa6\x00" \
    "\x95\xa3\x34\x90\xd2\xb5\xbc\x00\x2c\xbb\x81\xe3" \
    "\xc0\x2f\x57\xbf\xf9\xcf\x80\x57\x5e\xfb\x14\x5d" \
    "\x0f\x2b\x94\xd2\xb5\x3c\xa1\x81\x43\x23\x45\x09" \
    "\x06\x24\x59\x11\x48\x42\x46\xd3\x14\x84\xf4\x3f" \
    "\x4e\x65\x21\x21\x49\x0a\x08\x19\x59\x11\x91\x35" \
    "\x44\x3e\x85\x7c\xcf\xd9\x50\x35\x79\xbc\xe3\x98" \
    "\x49\xc0\x01\xaf\xbe\xf3\x39\xb2\x1c\x9a\x11\x08" \
    "\x20\xc0\xf7\x03\x7c\xdf\xa3\xe9\x79\x18\x86\x42" \
    "\x7b\x5b\x06\x45\x09\x63\xa3\xe6\x8f\x6a\xc0\x6f" \
    "\xd4\xec\x82\xe7\x07\xe3\x23\xf9\x7e\xf4\x47\x45" \
    "\xec\x7a\x1d\xc3\x48\xa1\x69\x1a\x8a\x22\xe1\xf9" \
    "\x01\xae\xe3\x50\xab\x35\xb0\xad\x1a\x99\x16\x83" \
    "\xc1\xfe\x2e\x9a\x4e\x83\x46\xcd\x2e\x00\xfe\x51" \
    "\x1a\x70\x4b\xfb\xdb\xd7\xcb\x4f\x4a\xef\x4d\x8c" \
    "\x9d\xd2\xb2\xd9\x2c\xa5\xaa\xc5\x81\x55\xc7\x30" \
    "\x0c\x64\x45\xc2\xf7\x7c\xea\x8d\x06\x8a\x62\xd3" \
    "\xdf\xd3\x8d\xd9\x96\xc6\x48\xa9\x2c\x2d\xaf\x38" \
    "\xa5\xfd\xed\xeb\x44\x68\xe0\x58\x0c\x14\xe6\xae" \
    "\x5d\xe9\xe8\xca\xcd\xb6\xb7\x1f\xfb\x60\x62\xec" \
    "\x14\x7d\x7d\xdd\xb8\x4d\x9f\x9a\xe3\xe2\x38\x4d" \
    "\x5c\xdf\x45\x95\x14\x74\x55\x46\x53\x65\xf0\x7c" \
    "\x16\x97\x57\xb9\xbf\x71\x6f\xb6\x30\x77\xed\x4a" \
    "\x54\x03\x51\x2f\xb2\x00\x08\x2c\xeb\x60\x57\xd5" \
    "\x52\x3d\x29\xc3\xc8\x77\x77\x77\x72\xfc\x78\x27" \
    "\xbd\xb9\x76\x06\xfa\x73\xf4\x77\x77\xd0\xd1\x9e" \
    "\x25\x93\x4e\x51\xaf\x3b\xac\xae\xad\xb3\x70\xa7" \
    "\x70\xad\xf0\xfb\xaf\x5f\x5b\xd5\xd2\x3d\xa0\x7e" \
    "\x94\x06\x00\x7c\xab\x5a\x2a\x97\xcb\xe5\x0d\xcf" \
    "\x13\xba\xdb\x6c\x8e\x7a\x81\x2f\x6b\x8a\x86\xae" \
    "\xab\x78\xbe\x4f\xb1\x58\x66\x7d\x63\x93\xa5\xa5" \
    "\xbb\xce\xed\xc2\xfc\xec\xad\xb9\x9f\xbf\xdd\xdb" \
    "\x5a\x9f\x07\x0e\x88\xd8\x03\x71\x4c\xa3\x82\xf0" \
    "\x53\xcc\x02\x5d\x13\x2f\x5e\x7c\xa3\xb7\x6f\xe8" \
    "\x62\xab\x99\x1d\xd7\x53\xe9\x01\x80\x46\xdd\xde" \
    "\xac\x96\x2b\x77\x1e\x3d\x5c\xbf\xb1\xf0\xc7\x8d" \
    "\x2b\xc0\x1e\x50\x21\x9c\x83\x22\x4d\xa4\xb1\x8d" \
    "\xd3\x84\xd5\x34\x08\x8d\xb4\x3e\x7d\x7f\x76\x49" \
    "\xb9\x40\x0d\xa8\x12\x0a\xaf\x01\x1e\x31\x8c\xd3" \
    "\xb1\xff\xd0\x10\x56\x43\x7d\xfa\x3c\xbb\xa4\x7c" \
    "\x42\x13\x2e\x31\xec\x7a\x42\x42\x42\x42\x42\x42" \
    "\x42\x42\x42\x42\x42\x2c\xfc\x0d\x68\x9c\x6e\x33" \
    "\x03\xc7\x23\x6e\x00\x00\x00\x00\x49\x45\x4e\x44" \
    "\xae\x42\x60\x82"
image2_data = \
    "\x89\x50\x4e\x47\x0d\x0a\x1a\x0a\x00\x00\x00\x0d" \
    "\x49\x48\x44\x52\x00\x00\x00\x18\x00\x00\x00\x18" \
    "\x08\x06\x00\x00\x00\xe0\x77\x3d\xf8\x00\x00\x02" \
    "\xaa\x49\x44\x41\x54\x78\x9c\xed\x94\x4f\x4f\x13" \
    "\x41\x18\x87\x9f\x99\xe9\x96\x56\x41\x40\x0b\x58" \
    "\x12\x08\x46\x0c\x86\x68\x8c\x34\xc6\xe0\x45\x0f" \
    "\x1e\x4c\x8c\x47\xe3\xc9\xef\xe1\x49\xfe\x9c\xf8" \
    "\x1e\x9c\x0c\x47\x63\xe2\xc1\x03\x5c\x4c\x88\x41" \
    "\xc3\xc5\x60\xc4\xa8\x21\x01\xba\xa5\x54\x61\x65" \
    "\xdb\x6e\x67\xd6\xc3\xb6\x4b\x61\x4a\xe1\x6a\xe2" \
    "\x2f\xd9\xec\xec\xcc\x3b\xbf\xe7\x9d\x77\xdf\x0c" \
    "\xfc\xeb\x12\xa7\x05\xcc\xce\x2f\x86\xed\xd6\x5f" \
    "\x3e\x7f\xd0\xd6\xe3\xc4\xc5\x86\xf1\xc4\xb5\x41" \
    "\x1e\xdd\x19\x25\x04\x08\x0f\x77\x09\xe0\xed\x87" \
    "\x75\x3e\x7e\xdd\x6c\x0b\x6a\x39\x39\x3b\xbf\x18" \
    "\xde\x1e\xcd\xf2\x30\x37\x8a\x09\x43\xc2\xba\x71" \
    "\x58\x1f\x08\x21\xea\x6f\x90\x42\xf0\x6e\x65\x9d" \
    "\x4f\xeb\x5b\x2d\x21\xd6\xc4\xec\xfc\x62\x78\xeb" \
    "\x6a\x96\xfb\xb7\xae\x50\x33\xc4\x80\xf7\x9f\x37" \
    "\x58\xdb\xd8\x01\xe0\xfa\x50\x86\x7b\xe3\x43\x31" \
    "\x20\x21\x61\x69\xf5\x3b\xab\xdf\x6c\x48\xa2\xd5" \
    "\x09\x26\x6f\x8c\xe0\x55\x34\x35\x1d\x62\xc2\x10" \
    "\x63\x42\xd6\x36\x76\x78\xf1\xf4\x2e\x00\x73\x0b" \
    "\xcb\xdc\xbc\x72\x19\x29\x45\x04\x50\x82\xc9\x1b" \
    "\x23\xac\x7e\xdb\xb2\xbc\xe4\xf1\xec\xc7\x47\x06" \
    "\xf0\xca\x9a\x7d\x3f\x7a\x3c\x5f\xe3\x95\x8d\xb5" \
    "\xd1\x2b\x1b\x3c\xbf\x29\xae\xac\x19\x1f\x19\xb0" \
    "\x9a\xc2\x3a\xc1\xc4\xd8\x30\x5e\xa5\x46\x25\x30" \
    "\x04\xda\xc4\xf5\xcf\x66\xba\x99\x5b\x58\x8e\xc7" \
    "\x7f\xaa\xb5\xa8\xc6\x02\x1c\x25\x09\x8c\x64\x62" \
    "\x6c\x98\xcf\x3f\xf2\x47\xfc\x2c\x80\x5f\x35\xf8" \
    "\x15\x8d\x5f\xd5\x04\x3a\x72\xff\xb9\x5d\xa0\x50" \
    "\xda\x3b\x52\xa2\xc1\xbe\x4c\xfc\x03\x1d\x65\x30" \
    "\x46\xa1\x84\xdd\x33\x16\xa0\x12\x18\x0e\xaa\x06" \
    "\xbf\x6a\x62\x40\xb3\x79\x73\x22\x0d\x39\x4a\x10" \
    "\x22\x70\x94\x5d\x4a\x0b\xb0\xb6\xed\xb1\x59\xaa" \
    "\x50\x3a\x08\xf0\xab\x1a\x80\x4e\x99\x64\x6e\x61" \
    "\xf9\x08\x64\xe9\x4b\x31\x1e\xa7\x93\x8a\xde\x73" \
    "\x0e\x83\xbd\x1d\x16\x40\x1e\x9f\x50\x52\x20\x25" \
    "\x48\x71\xd8\xc3\x9e\x49\x59\x1b\x1b\x12\x44\xb1" \
    "\x52\x46\x7b\x4f\x05\x38\x4a\xe2\x28\x51\x07\x9d" \
    "\x7a\x93\x20\x65\x14\xeb\x28\x81\xa3\x2c\x3b\x1b" \
    "\xe0\xba\x79\x52\x8e\xc2\x51\x32\x82\xb4\x61\x48" \
    "\x41\xdd\x5c\x92\x72\x14\xae\x9b\xb7\x63\x9a\x3f" \
    "\x72\x3d\xf9\x19\xb7\x58\xa2\x3b\x9d\xa0\x2b\xa5" \
    "\x48\x27\x15\x89\x3a\xe8\xb8\x94\x14\x24\x94\x24" \
    "\x9d\x54\x74\xa5\x14\xdd\xe9\x04\x6e\xb1\x44\xae" \
    "\x27\x3f\xd3\x1c\xd7\xf2\xaa\xc8\xf6\x5d\x24\x7d" \
    "\xe1\x12\xbb\x5e\x95\xfd\xb2\x26\xd0\x06\x55\xfb" \
    "\x43\x22\xac\x00\x50\x13\x1d\xe8\xc4\x79\x1c\x25" \
    "\xe9\x4a\x29\x2e\x76\x26\xf1\xf7\x8a\x6c\x15\x76" \
    "\x4f\xbf\x2a\x72\x3d\xf9\x99\x95\x02\x53\x59\xe0" \
    "\x72\x6f\x86\x74\xb2\x46\x39\xd0\x04\xba\x1b\x53" \
    "\xef\xc2\x0e\x19\xb5\x66\xca\x89\x32\xff\x5d\xda" \
    "\x61\xab\xb0\x6b\x65\xdf\xf2\x04\x00\x6f\x5e\xbf" \
    "\x9a\x5e\xf9\x35\x30\x05\xd0\x7f\xa9\x97\xfe\xfe" \
    "\x01\x02\x6d\xd0\x26\x8c\xcb\xe3\x28\x89\xeb\xe6" \
    "\x71\x8b\xa5\x38\xb1\xc7\x4f\x9e\x4d\x9f\x09\xd0" \
    "\x80\x00\x34\x40\x27\xa9\x91\x75\x2b\xf3\xb6\x80" \
    "\xe3\xa0\x93\x74\x92\xf1\x7f\x9d\x59\x7f\x01\x54" \
    "\x78\x23\xf1\x52\x0a\xe2\xe1\x00\x00\x00\x00\x49" \
    "\x45\x4e\x44\xae\x42\x60\x82"

class GUIcreateDevices(QDialog):
    def __init__(self,parent = None,name = None,modal = 0,fl = 0):
        QDialog.__init__(self,parent,name,modal,fl)

        self.image0 = QPixmap()
        self.image0.loadFromData(image0_data,"PNG")
        self.image1 = QPixmap()
        self.image1.loadFromData(image1_data,"PNG")
        self.image2 = QPixmap()
        self.image2.loadFromData(image2_data,"PNG")
        if not name:
            self.setName("GUIcreateDevices")


        GUIcreateDevicesLayout = QGridLayout(self,1,1,11,6,"GUIcreateDevicesLayout")
        GUIcreateDevicesLayout.setResizeMode(QLayout.Fixed)

        self.textLabel2_4_4_2 = QLabel(self,"textLabel2_4_4_2")

        GUIcreateDevicesLayout.addMultiCellWidget(self.textLabel2_4_4_2,2,2,0,2)

        self._prefixtxtbox = QLineEdit(self,"_prefixtxtbox")
        self._prefixtxtbox.setFrameShape(QLineEdit.LineEditPanel)
        self._prefixtxtbox.setFrameShadow(QLineEdit.Sunken)

        GUIcreateDevicesLayout.addMultiCellWidget(self._prefixtxtbox,3,3,0,2)

        self.textLabel2_4_4 = QLabel(self,"textLabel2_4_4")

        GUIcreateDevicesLayout.addMultiCellWidget(self.textLabel2_4_4,2,2,3,5)

        self._deviceidtxtbox = QLineEdit(self,"_deviceidtxtbox")
        self._deviceidtxtbox.setEnabled(0)
        self._deviceidtxtbox.setMaximumSize(QSize(100,32767))

        GUIcreateDevicesLayout.addMultiCellWidget(self._deviceidtxtbox,3,3,3,4)

        self.textLabel2_3 = QLabel(self,"textLabel2_3")

        GUIcreateDevicesLayout.addMultiCellWidget(self.textLabel2_3,4,4,0,5)

        self._devicetypetxtbox = QComboBox(0,self,"_devicetypetxtbox")

        GUIcreateDevicesLayout.addMultiCellWidget(self._devicetypetxtbox,5,5,0,5)

        self._chk_node = QCheckBox(self,"_chk_node")

        GUIcreateDevicesLayout.addMultiCellWidget(self._chk_node,6,7,0,2)

        self._chk_prommode = QCheckBox(self,"_chk_prommode")

        GUIcreateDevicesLayout.addMultiCellWidget(self._chk_prommode,6,7,3,5)

        layout68 = QHBoxLayout(None,0,6,"layout68")
        spacer1 = QSpacerItem(220,20,QSizePolicy.Expanding,QSizePolicy.Minimum)
        layout68.addItem(spacer1)

        self._portbutton = QPushButton(self,"_portbutton")
        layout68.addWidget(self._portbutton)

        self._okbutton = QPushButton(self,"_okbutton")
        layout68.addWidget(self._okbutton)

        self._cancelbutton = QPushButton(self,"_cancelbutton")
        layout68.addWidget(self._cancelbutton)

        GUIcreateDevicesLayout.addMultiCellLayout(layout68,11,11,0,7)

        layout51 = QVBoxLayout(None,0,6,"layout51")

        self.textLabel2 = QLabel(self,"textLabel2")
        layout51.addWidget(self.textLabel2)

        self._systemnametxtbox = QLineEdit(self,"_systemnametxtbox")
        self._systemnametxtbox.setEnabled(0)
        layout51.addWidget(self._systemnametxtbox)

        GUIcreateDevicesLayout.addMultiCellLayout(layout51,0,1,0,1)

        layout52 = QVBoxLayout(None,0,6,"layout52")

        self.textLabel2_2 = QLabel(self,"textLabel2_2")
        layout52.addWidget(self.textLabel2_2)

        self._sysnamescmbbox = QComboBox(0,self,"_sysnamescmbbox")
        layout52.addWidget(self._sysnamescmbbox)

        GUIcreateDevicesLayout.addMultiCellLayout(layout52,0,1,2,3)

        self._addbutton = QPushButton(self,"_addbutton")
        self._addbutton.setMaximumSize(QSize(30,30))
        self._addbutton.setFocusPolicy(QPushButton.TabFocus)
        self._addbutton.setPixmap(self.image0)
        self._addbutton.setFlat(1)

        GUIcreateDevicesLayout.addMultiCellWidget(self._addbutton,0,1,4,4)

        self._delbutton = QPushButton(self,"_delbutton")
        self._delbutton.setMaximumSize(QSize(30,30))
        self._delbutton.setFocusPolicy(QPushButton.TabFocus)
        self._delbutton.setAutoMask(0)
        self._delbutton.setPixmap(self.image1)
        self._delbutton.setFlat(1)

        GUIcreateDevicesLayout.addMultiCellWidget(self._delbutton,0,1,5,5)

        self.textLabel2_4_3_2 = QLabel(self,"textLabel2_4_3_2")

        GUIcreateDevicesLayout.addWidget(self.textLabel2_4_3_2,0,7)

        self.textLabel2_4_3_2_2_2 = QLabel(self,"textLabel2_4_3_2_2_2")

        GUIcreateDevicesLayout.addWidget(self.textLabel2_4_3_2_2_2,4,7)

        self._hwtypetxtbox = QLineEdit(self,"_hwtypetxtbox")

        GUIcreateDevicesLayout.addWidget(self._hwtypetxtbox,1,7)

        self._responsibletxtbox = QLineEdit(self,"_responsibletxtbox")

        GUIcreateDevicesLayout.addWidget(self._responsibletxtbox,3,7)

        self._locationtxtbox = QLineEdit(self,"_locationtxtbox")

        GUIcreateDevicesLayout.addWidget(self._locationtxtbox,5,7)

        self.textLabel2_4_3_2_2 = QLabel(self,"textLabel2_4_3_2_2")

        GUIcreateDevicesLayout.addWidget(self.textLabel2_4_3_2_2,2,7)

        self.textLabel2_4_3_2_2_2_3 = QLabel(self,"textLabel2_4_3_2_2_2_3")

        GUIcreateDevicesLayout.addWidget(self.textLabel2_4_3_2_2_2_3,6,7)

        self.buttonGroup20 = QButtonGroup(self,"buttonGroup20")
        self.buttonGroup20.setMaximumSize(QSize(32767,210))
        self.buttonGroup20.setColumnLayout(0,Qt.Vertical)
        self.buttonGroup20.layout().setSpacing(6)
        self.buttonGroup20.layout().setMargin(11)
        buttonGroup20Layout = QGridLayout(self.buttonGroup20.layout())
        buttonGroup20Layout.setAlignment(Qt.AlignTop)

        self.textLabel2_4_2_2 = QLabel(self.buttonGroup20,"textLabel2_4_2_2")

        buttonGroup20Layout.addMultiCellWidget(self.textLabel2_4_2_2,2,2,0,2)

        self._paddingspinbox = QSpinBox(self.buttonGroup20,"_paddingspinbox")
        self._paddingspinbox.setEnabled(0)

        buttonGroup20Layout.addMultiCellWidget(self._paddingspinbox,1,1,2,3)

        self.textLabel2_4_2 = QLabel(self.buttonGroup20,"textLabel2_4_2")

        buttonGroup20Layout.addWidget(self.textLabel2_4_2,1,4)

        self.pixmapLabel4 = QLabel(self.buttonGroup20,"pixmapLabel4")
        self.pixmapLabel4.setSizePolicy(QSizePolicy(0,0,0,0,self.pixmapLabel4.sizePolicy().hasHeightForWidth()))
        self.pixmapLabel4.setPixmap(self.image2)
        self.pixmapLabel4.setScaledContents(1)

        buttonGroup20Layout.addWidget(self.pixmapLabel4,0,0)

        self.textLabel2_4 = QLabel(self.buttonGroup20,"textLabel2_4")

        buttonGroup20Layout.addMultiCellWidget(self.textLabel2_4,0,0,1,4)

        self._paddingchkbox = QCheckBox(self.buttonGroup20,"_paddingchkbox")

        buttonGroup20Layout.addMultiCellWidget(self._paddingchkbox,1,1,0,1)

        self._startspinbox = QSpinBox(self.buttonGroup20,"_startspinbox")

        buttonGroup20Layout.addMultiCellWidget(self._startspinbox,3,3,0,2)

        self.textLabel2_4_2_2_2 = QLabel(self.buttonGroup20,"textLabel2_4_2_2_2")

        buttonGroup20Layout.addMultiCellWidget(self.textLabel2_4_2_2_2,2,2,3,4)

        self._endspinbox = QSpinBox(self.buttonGroup20,"_endspinbox")

        buttonGroup20Layout.addMultiCellWidget(self._endspinbox,3,3,3,4)

        GUIcreateDevicesLayout.addMultiCellWidget(self.buttonGroup20,10,10,0,5)
        spacer4 = QSpacerItem(20,20,QSizePolicy.Fixed,QSizePolicy.Minimum)
        GUIcreateDevicesLayout.addItem(spacer4,10,6)

        self._deviceurl = QLineEdit(self,"_deviceurl")

        GUIcreateDevicesLayout.addMultiCellWidget(self._deviceurl,7,8,7,7)

        self.textLabel2_4_3 = QLabel(self,"textLabel2_4_3")

        GUIcreateDevicesLayout.addMultiCellWidget(self.textLabel2_4_3,8,9,0,0)

        self._serialtxtbox = QLineEdit(self,"_serialtxtbox")
        self._serialtxtbox.setFrameShape(QLineEdit.LineEditPanel)
        self._serialtxtbox.setFrameShadow(QLineEdit.Sunken)

        GUIcreateDevicesLayout.addMultiCellWidget(self._serialtxtbox,8,9,1,5)

        self._commentstxtbox = QTextEdit(self,"_commentstxtbox")

        GUIcreateDevicesLayout.addWidget(self._commentstxtbox,10,7)

        self.textLabel2_4_3_2_2_2_2 = QLabel(self,"textLabel2_4_3_2_2_2_2")

        GUIcreateDevicesLayout.addWidget(self.textLabel2_4_3_2_2_2_2,9,7)

        self.languageChange()

        self.resize(QSize(591,439).expandedTo(self.minimumSizeHint()))
        self.clearWState(Qt.WState_Polished)

        self.connect(self._cancelbutton,SIGNAL("released()"),self.reject)
        self.connect(self._okbutton,SIGNAL("released()"),self.accept)

        self.setTabOrder(self._systemnametxtbox,self._sysnamescmbbox)
        self.setTabOrder(self._sysnamescmbbox,self._addbutton)
        self.setTabOrder(self._addbutton,self._delbutton)
        self.setTabOrder(self._delbutton,self._prefixtxtbox)
        self.setTabOrder(self._prefixtxtbox,self._deviceidtxtbox)
        self.setTabOrder(self._deviceidtxtbox,self._devicetypetxtbox)
        self.setTabOrder(self._devicetypetxtbox,self._chk_node)
        self.setTabOrder(self._chk_node,self._chk_prommode)
        self.setTabOrder(self._chk_prommode,self._serialtxtbox)
        self.setTabOrder(self._serialtxtbox,self._paddingchkbox)
        self.setTabOrder(self._paddingchkbox,self._paddingspinbox)
        self.setTabOrder(self._paddingspinbox,self._startspinbox)
        self.setTabOrder(self._startspinbox,self._endspinbox)
        self.setTabOrder(self._endspinbox,self._hwtypetxtbox)
        self.setTabOrder(self._hwtypetxtbox,self._responsibletxtbox)
        self.setTabOrder(self._responsibletxtbox,self._locationtxtbox)
        self.setTabOrder(self._locationtxtbox,self._deviceurl)
        self.setTabOrder(self._deviceurl,self._commentstxtbox)
        self.setTabOrder(self._commentstxtbox,self._portbutton)
        self.setTabOrder(self._portbutton,self._okbutton)
        self.setTabOrder(self._okbutton,self._cancelbutton)


    def languageChange(self):
        self.setCaption(self.__tr("Create Devices"))
        self.setIconText(self.__tr("Create/Modify ports ..."))
        self.textLabel2_4_4_2.setText(self.__tr("Device Name:"))
        self.textLabel2_4_4.setText(self.__tr("Device ID:"))
        self.textLabel2_3.setText(self.__tr("Device Type:"))
        self._chk_node.setText(self.__tr("This is a node (starts or ends a path)"))
        self._chk_prommode.setText(self.__tr("Promiscuous mode"))
        self._portbutton.setText(self.__tr("Create/Modify ports"))
        self._okbutton.setText(self.__tr("Ok"))
        self._cancelbutton.setText(self.__tr("Cancel"))
        self.textLabel2.setText(self.__tr("Create in:"))
        self.textLabel2_2.setText(self.__tr("Add to subsystem:"))
        self._addbutton.setText(QString.null)
        self._delbutton.setText(QString.null)
        self.textLabel2_4_3_2.setText(self.__tr("Hardware type:"))
        self.textLabel2_4_3_2_2_2.setText(self.__tr("Location:"))
        self.textLabel2_4_3_2_2.setText(self.__tr("Responsible:"))
        self.textLabel2_4_3_2_2_2_3.setText(self.__tr("URL :"))
        self.buttonGroup20.setTitle(self.__tr("Create multiple devices"))
        self.textLabel2_4_2_2.setText(self.__tr("Start nr:"))
        self.textLabel2_4_2.setText(self.__tr("digits max."))
        self.textLabel2_4.setText(self.__tr("Prefix:  in the Device Name field put %d where to place number"))
        self._paddingchkbox.setText(self.__tr("Pad with 0s?:"))
        self.textLabel2_4_2_2_2.setText(self.__tr("Total:"))
        self.textLabel2_4_3.setText(self.__tr("Serial Number:"))
        self.textLabel2_4_3_2_2_2_2.setText(self.__tr("Comments:"))


    def __tr(self,s,c = None):
        return qApp.translate("GUIcreateDevices",s,c)
