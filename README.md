# Hamming Codes

C Program used to encode files to be more resistant to single bit errors. Uses (15, 11) Hamming Codes to correct single bit errors and detect double bit errors. With 3 bit errors or more, this program cannot reliably detect or correct errors. Of a 16-bit chunk, the parity bits are as follows, the data bits occuping the remaining 11 bits.
- Bit 0 contains the parity for the entire chunk.
- Bit 1 contains the parity for all odd bit positions (0bxxx1).
- Bit 2 contains the parity for all even bit positions (0bxx1x).
- Bit 4 contains the parity for bit positions with 0bx1xx pattern.
- Bit 8 contains the parity for bit positions with 0b1xxx pattern.

Using this pattern, the XOR of activated bit positions always gives 0.
> Ex. 16-bit chunk 0110 1000 1000 0001 => 1 ^ 2 ^ 4 ^ 8 ^ 15 = 0

If there is a single bit flip anywhere in the chunk (excluding bit 0), the resulting XOR will be the the exact bit position that flipped. For example, if bit 7 was flipped:
> Ex. 16-bit chunk 0110 1001 1000 0001 => 1 ^ 2 ^ 4 ^ 7 ^ 8 ^ 15 = 7

This program uses that logic to correct single bit errors. In this case, bit 7 would be flipped back to a 0 and the message would be passed along error-free.

The reason bit 0 is excluded from this rule is because it has no effect on the XOR of bit positions. Instead, this bit tracks the overall parity for the whole chunk, which allows for double bit error detection. In the previous example, the single bit error left the overall parity of the chunk odd, which further proves the existence of a single bit error. However, if the overall parity was still even, that would mean at least 2 bits have been flipped in the chunk, and neither can be corrected. The XOR of bit positions correctly equating to 0 but the overall parity of the block being odd is another indicator of 2 or more errors in a block. The program will halt in either of these cases.

## How to use (Linux)
### Clone repository
```
git clone https://github.com/jacehalvorson/hamming_code.git
```
### If necessary, install or update make and gcc
```
sudo apt install make
sudo apt install gcc
```

### Encode/decode file and save copy
```
make
./hamming [encode|decode] <fileName>
```
### Run all tests
```
./test.sh
```
### Run specific test
```
make test
./test [testIndex]
```

## Tests
### Test 1
Tests the populateChunk() function. Each test case tries a different 11-bit binary string and checks it against a pre-determined 16-bit chunk that should result from the Hamming Code.
> Test coverage
- All 0s
- Alternating bits
- Larger-scale pattern
- Random binary string
- All 1s

### Test 2
Tests the encoding and decoding of files from the ground up. Compiles the program and runs it on various files using the 'encode' option. These files are then decoded and asserted to match the original files.
> Test coverage
- Empty file
- Large file
- Divisible by 11 bits
- Nondivisible by 11 bits
- Various file extensions

## Credits
This program was developed by Jace Halvorson. If you have any questions or suggestions, please contact me at jacehalvorson33@gmail.com. Inspiration for this project came from a 3-part YouTube series from 3Blue1Brown and Ben Eater. They provide an engaging and in-depth explanation of Hamming Codes, so look to these videos to further understand the concepts and logic used in this project.

- [Part 1](https://www.youtube.com/watch?v=X8jsijhllIA)
- [Part 2](https://www.youtube.com/watch?v=b3NxrZOu_CE&t=0s)
- [Part 3](https://www.youtube.com/watch?v=h0jloehRKas&t=64s)