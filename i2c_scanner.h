#ifndef I2C_SCANNER_H
#define I2C_SCANNER_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"

class I2CScanner {
private:
    i2c_inst_t* i2c_port;
    
public:
    I2CScanner(i2c_inst_t* i2c) : i2c_port(i2c) {}
    
    void scan() {
        printf("\nScanning I2C bus...\n");
        printf("     0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");
        
        for (int addr = 0; addr < (1 << 7); ++addr) {
            if (addr % 16 == 0) {
                printf("%02x:", addr);
            }
            
            // Skip reserved addresses
            if ((addr & 0x78) == 0 || (addr & 0x78) == 0x78) {
                printf("   ");
            } else {
                uint8_t rxdata;
                int result = i2c_read_blocking(i2c_port, addr, &rxdata, 1, false);
                
                if (result < 0) {
                    printf(" --");
                } else {
                    printf(" %02X", addr);
                }
            }
            
            if (addr % 16 == 15) {
                printf("\n");
            }
        }
        printf("\nScan complete.\n");
    }
    
    bool test_address(uint8_t addr) {
        uint8_t rxdata;
        int result = i2c_read_blocking(i2c_port, addr, &rxdata, 1, false);
        return result >= 0;
    }
};

#endif // I2C_SCANNER_H
