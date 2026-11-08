# LSB Steganography in C — Engineering Journey

## Repository status
The git history visible in this local clone is shallow; the repository contains only a single visible merge/initial commit. Because of that, the strongest evidence comes from the code itself rather than a long historical timeline.

## Challenge matrix

| Challenge | Evidence from code/history | What changed | Engineering reasoning | Learning outcome |
|---|---|---|---|---|
| Pixel-level data hiding | `encode_byte_to_lsb()` modifies only the LSB of each chosen byte | Data is hidden one bit at a time | Keeps the image visually stable while still embedding data | Learned bit-level manipulation in low-level C |
| Metadata-aware payload format | `check_capacity()` encodes magic string, extension, size, and payload | Hidden payload is structured for later decode | Decoder knows exactly how to parse embedded data | Learned to design reversible binary formats |
| Avoiding BMP corruption | `copy_bmp_header()` preserves the first 54 bytes of the file | Header is separated from pixel-data modifications | A valid image must retain its header and file structure | Learned format-aware file handling |
| Capacity validation | `get_image_size_for_bmp()` and `check_capacity()` calculate whether data fits | Encoding is gated before writing | Prevents truncated or incomplete hidden payloads | Learned to validate before performing transformations |
| Reliable decode logic | Decoder checks for a magic string before reading extension/size/data | Hidden data is read in a defined sequence | Prevents random BMP files from being misread as stego images | Learned robust validation in binary parsing |

## Interview questions and evidence-backed answers
1. Why is the BMP header preserved instead of altered?
   Answer: `copy_bmp_header()` reads the first 54 bytes and writes them unchanged to the stego image, which preserves the file structure.

2. What does the magic string do?
   Answer: It acts as a marker to confirm that the image contains hidden data before the decoder reads the rest of the payload.

3. Why does the implementation use the least significant bit?
   Answer: The code specifically modifies only the last bit of target bytes, which keeps the image visually close to the original.

4. What is the crucial line in the encoding logic?
   Answer: `image_buffer[i] = (image_buffer[i] & 0xFE) | ((data >> (7 - i)) & 1);` modifies one bit while preserving the rest of the byte.

5. Why is capacity checked before encoding?
   Answer: `check_capacity()` calculates the image capacity and compares it with the size of the encoded metadata and payload to avoid writing incomplete or invalid data.

6. What engineering challenge does the file-size metadata address?
   Answer: The decoder must know how much of the embedded payload to read before writing the extracted text, so the size is stored before the actual data.

7. How does the decoder know which part of the image contains hidden content?
   Answer: It reads the magic string first, then reads stored metadata like extension and payload length, then extracts the hidden bytes.

8. Why is byte ordering important here?
   Answer: The code reads BMP dimensions and payload sizes from binary offsets, which demands strict knowledge of file format conventions and storage order.

9. How did you avoid corrupting the image while hiding data?
   Answer: Only the LSBs of selected bytes are modified; the header is left intact and the rest of the file remains structurally valid.

10. What was the hardest low-level problem in this project?
   Answer: Designing a reversible binary format so metadata, file size, and payload can be encoded and decoded consistently.

11. Why does the code write the extension before the secret text?
   Answer: The decoder needs to know the output file type or a reconstruction clue before it writes the extracted payload.

12. What trade-off is inherent in the approach?
   Answer: The more data you embed, the more LSBs are changed, which increases the chance of visible or detectable alteration.

13. How is invalid input handled?
   Answer: The code validates usage and file openings before starting the encode/decode process; it fails gracefully instead of continuing blindly.

14. What would you change if rebuilding this project?
   Answer: I would likely add a more robust file-format validation layer and support more file types, but this is not directly evidenced in the current repo.

15. What is the key lesson from this implementation?
   Answer: Binary-file engineering is less about high-level abstractions and more about exact offsets, bit ordering, and reversible metadata design.

## Strongest evidence-backed takeaways
- This repository is a sound demonstration of low-level bit manipulation and binary file understanding.
- The most concrete engineering challenge is designing a reversible metadata format that allows a decoder to reconstruct the hidden payload reliably.
- The code shows disciplined awareness of offsets, header safety, and data validation rather than ad hoc image mutation.
