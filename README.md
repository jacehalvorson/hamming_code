# Hamming Codes

C Program used to encode files to be more resistant to single bit errors. Uses (15, 11) Hamming Codes to correct single bit errors and detect double bit errors. With 3 bit errors or more, this program cannot reliably detect or correct errors.

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

### Encode file with hamming code and save a copy
```
make
./hamming <fileName>
```
### Run all tests
```
make test
./test
```
### Run specific test
```
make test
./test [testIndex]
```

## Credits
This program was developed by Jace Halvorson. If you have any questions or suggestions, please contact me at jacehalvorson33@gmail.com. Inspiration for this project came from a 3-part YouTube series from 3Blue1Brown and Ben Eater. They provide an engaging and in-depth explanation of Hamming Codes, so look to these videos to further understand the concepts and logic used in this project.

- [Part 1](https://www.youtube.com/watch?v=X8jsijhllIA)
- [Part 2](https://www.youtube.com/watch?v=b3NxrZOu_CE&t=0s)
- [Part 3](https://www.youtube.com/watch?v=h0jloehRKas&t=64s)