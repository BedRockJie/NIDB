# NIDB (Nova Intel Decode Bin)
Parse Intel rpd file to bin

## Working Principle

The core functionality of the NIDB tool is to convert between Intel FPGA's RPD file format and binary BIN files:

According to Intel's official description: https://www.intel.com/content/www/us/en/docs/programmable/683710/current/programming-and-configuration-file-support.html

![alt text](image/bitswap.drawio.png)

## How to Build

```bash
cmake -B ./build
cmake --build ./build
```

## How to Use

```bash
# Convert RPD to BIN file
nidb -r2b input.rpd -o output.bin

# Convert BIN to RPD file
nidb -b2r input.bin -o output.rpd

# Display help information
nidb --help
```

## Important Notes

When using the NIDB tool, please ensure:

- Input file format is correct
- Sufficient disk space is available
- Write permissions for the target directory

## Contribution Guidelines

Pull Requests to improve the NIDB tool are welcome. Please ensure:

- Code follows the project's coding standards
- Appropriate test cases are added
- Related documentation is updated

## License

This project is licensed under the MIT License. See LICENSE file for details.