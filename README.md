nofly-tiles
===========

First, read the original proposal here:  
https://docs.google.com/document/d/14j6oO0IH-jobzfKbocxTKpZ5DFexVcmsRoLfbSQg0DI/edit?usp=sharing

One of the main issues with that proposal is that embedded systems would have trouble parsing JSON, so a binary format was preferred. This repo is an example of a binary format that I created as a demo. Try it out like so:

```
# create an example tile
./demo_tile_generator.py

gcc parser.cpp -o parser

hexdump -C tile42N72W.dat

./parser ./tile42N72W.dat
```

Here is a brief explanation of how the format works:
https://docs.google.com/document/d/1J613P7NyRpYtVD9TY1b0cSYFwYa-gDX6dayTttCsBmw/edit?usp=sharing
